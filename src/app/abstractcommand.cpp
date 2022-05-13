/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "abstractcommand.h"

#include <qtpokit/pokitdevice.h>
#include <qtpokit/pokitdiscoveryagent.h>

#include <QLocale>

/*!
 * \class AbstractCommand
 *
 * The AbstractCommand class provides a consistent base for the classes that implement CLI commands.
 */

/// \enum AbstractCommand::OutputFormat
/// \brief Supported output formats.

/*!
 * Constructs a new command with \a parent.
 */
AbstractCommand::AbstractCommand(QObject * const parent) : QObject(parent),
    discoveryAgent(new PokitDiscoveryAgent(this)), format(OutputFormat::Text)
{
    connect(discoveryAgent, &PokitDiscoveryAgent::pokitDeviceDiscovered,
            this, &AbstractCommand::deviceDiscovered);
    connect(discoveryAgent, &PokitDiscoveryAgent::finished,
            this, &AbstractCommand::deviceDiscoveryFinished);
    connect(discoveryAgent,
        #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
        QOverload<PokitDiscoveryAgent::Error>::of(&PokitDiscoveryAgent::error),
        #else
        &PokitDiscoveryAgent::errorOccurred,
        #endif
    [](const PokitDiscoveryAgent::Error &error) {
        qCWarning(lc).noquote() << tr("Bluetooth controller error:") << error;
        QCoreApplication::exit(EXIT_FAILURE);
    });
}

/*!
 * Returns a list of CLI option names required by this command. The main console appication may
 * use this list to output an eror (and exit) if any of the returned names are not found in the
 * parsed CLI options.
 *
 * The (already parsed) \a parser may be used adjust the returned required options depending on the
 * value of other options. For example, the `logger` command only requires the `--mode` option if
 * the `--command` option is `start`.
 *
 * This base implementation simply returns an empty list. Derived classes should override this
 * function to include any required options.
 */
QStringList AbstractCommand::requiredOptions(const QCommandLineParser &parser) const
{
    Q_UNUSED(parser);
    return QStringList();
}

/*!
 * Returns a list of CLI option names supported by this command. The main console appication may
 * use this list to output a warning for any parsed CLI options not included in the returned list.
 *
 * The (already parsed) \a parser may be used adjust the returned supported options depending on the
 * value of other options. For example, the `logger` command only supported the `--timestamp` option
 * if the `--command` option is `start`.
 *
 * This base implementation simply returns requiredOptions(). Derived classes should override this
 * function to include optional options, such as:
 *
 * ```
 * QStringList Derived::supportedOptions(const QCommandLineParser &parser) const
 * {
 *     const QStringList list = AbstractCommand::supportedOptions(parser) + QStringList{ ... };
 *     list.sort();
 *     list.removeDuplicates(); // Optional, recommended.
 *     return list;
 * }
 * ```
 */
QStringList AbstractCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return requiredOptions(parser) + QStringList{
        QLatin1String("debug"),
        QLatin1String("device"),
        QLatin1String("output"),
        QLatin1String("timeout"),
    };
}

/*!
 * Returns an RFC 4180 compliant version of \a field. That is, if \a field contains any of the
 * the below four characters, than any double quotes are escaped (by addition double-quotes), and
 * the string itself surrounded in double-quotes. Otherwise, \a field is returned verbatim.
 *
 * Some examples:
 * ```
 * QCOMPARE(escapeCsvField("abc"), "abc");       // Returned unchanged.
 * QCOMPARE(escapeCsvField("a,c"), "\"a,c\"");   // Wrapped in double-quotes.
 * QCOMPARE(escapeCsvField("a\"c"), "\"a""c\""); // Existing double-quotes doubles, then wrapped.
 * ```
 */
QString AbstractCommand::escapeCsvField(const QString &field)
{
    if (field.contains(QLatin1Char(','))||field.contains(QLatin1Char('\r'))||
        field.contains(QLatin1Char('"'))||field.contains(QLatin1Char('\n')))
    {
        return QString::fromLatin1("\"%1\"").arg(
            QString(field).replace(QLatin1Char('"'), QLatin1String("\"\"")));
    } else return field;
}

/*!
 * Returns \a value as a number of micros, such as microseconds, or microvolts. The string \a value
 * may end with the optional \a unit, such as `V` or `s`, which may also be preceded with a SI unit
 * prefix such as `m` for `milli`. If \a value contains no SI unit prefix, then the result will be
 * multiplied by 1,000 enough times to be greater than \a sensibleMinimum. This allows for
 * convenient use like:
 *
 * ```
 * const quin32t timeout = parseMicroValue(parser.value("window"), 's', 500*1000);
 * ```
 *
 * So that an unqalified period like "300" will be assumed to be 300 milliseconds, and not 300
 * microseconds, while a period like "1000" will be assume to be 1 second.
 *
 * If conversion fails for any reason, 0 is returned.
 */
quint32 AbstractCommand::parseMicroValue(const QString &value, const QString &unit,
                                         const quint32 sensibleMinimum)
{
    // Remove the optional (whole) unit suffix.
    quint32 scale = 0;
    QString number = value.trimmed();
    if (number.endsWith(unit, Qt::CaseInsensitive)) {
        number.chop(unit.length());
        scale = 1000 * 1000;
    }

    // Parse, and remove, the optional SI unit prefix.
    if (number.endsWith(QLatin1String("m"))) {
        number.chop(1);
        scale = 1000;
    }

    // Parse, and remove, the optional SI unit prefix.
    if (number.endsWith(QLatin1String("u"))) {
        number.chop(1);
        scale = 1;
    }

    // Parse the number as an (unsigned) integer.
    QLocale locale; bool ok;
    const quint32 integer = locale.toUInt(number, &ok);
    if (ok) {
        if ((scale == 0) && (integer != 0)) {
            for (scale = 1; (integer * scale) < sensibleMinimum; scale *= 1000);
        }
        return integer * scale;
    }

    // Parse the number as a (double) floating point number.
    const double dbl = locale.toDouble(number, &ok);
    if (ok) {
        if ((scale == 0) && (dbl > 0.0)) {
            for (scale = 1; (dbl * scale) < sensibleMinimum; scale *= 1000);
        }
        return dbl * scale;
    }

    return 0; // Failed to parse as either integer, or float.
}

/*!
 * Returns \a value as a number of millis, such as milliseconds, or millivolts. The string \a value
 * may end with the optional \a unit, such as `V` or `s`, which may also be preceded with a SI unit
 * prefix such as `m` for `milli`. If \a value contains no SI unit prefix, then the result will be
 * multiplied by 1,000 enough times to be greater than \a sensibleMinimum. This allows for
 * convenient use like:
 *
 * ```
 * const quin32t timeout = parseMilliValue(parser.value("timeout"), 's', 600);
 * ```
 *
 * So that an unqalified period like "300" will be assumed to be 300 seconds, and not 300
 * milliseconds, while a period like "1000" will be assume to be 1 second.
 *
 * If conversion fails for any reason, 0 is returned.
 */
quint32 AbstractCommand::parseMilliValue(const QString &value, const QString &unit,
                                         const quint32 sensibleMinimum)
{
    // Remove the optional (whole) unit suffix.
    quint32 scale = 0;
    QString number = value.trimmed();
    if (number.endsWith(unit, Qt::CaseInsensitive)) {
        number.chop(unit.length());
        scale = 1000;
    }

    // Parse, and remove, the optional SI unit prefix.
    if (number.endsWith(QLatin1String("m"))) {
        number.chop(1);
        scale = 1;
    }

    // Parse the number as an (unsigned) integer.
    QLocale locale; bool ok;
    const quint32 integer = locale.toUInt(number, &ok);
    if (ok) {
        if ((scale == 0) && (integer != 0)) {
            for (scale = 1; (integer * scale) < sensibleMinimum; scale *= 1000);
        }
        return integer * scale;
    }

    // Parse the number as a (double) floating point number.
    const double dbl = locale.toDouble(number, &ok);
    if (ok) {
        if ((scale == 0) && (dbl > 0.0)) {
            for (scale = 1; (dbl * scale) < sensibleMinimum; scale *= 1000);
        }
        return dbl * scale;
    }

    return 0; // Failed to parse as either integer, or float.
}

/*!
 * Returns \a value as a number, with optional SI unit prefix, and optional \a unit suffix. For
 * example:
 *
 * ```
 * QCOMPARE(parseWholeValue("1.2Mohm", "ohm"), 1200000);
 * ```
 *
 * If conversion fails for any reason, 0 is returned.
 */
quint32 AbstractCommand::parseWholeValue(const QString &value, const QString &unit)
{
    // Remove the optional unit suffix.
    QString number = value.trimmed();
    if (number.endsWith(unit, Qt::CaseInsensitive)) {
        number.chop(unit.length());
    }

    // Parse, and remove, the optional SI unit prefix.
    quint32 scale = 1;
    if (number.endsWith(QLatin1String("k"))) {
        number.chop(1);
        scale = 1000;
    } else if (number.endsWith(QLatin1String("M"))) {
        number.chop(1);
        scale = 1000 * 1000;
    }

    // Parse the number as an (unsigned) integer.
    QLocale locale; bool ok;
    const quint16 integer = locale.toUInt(number, &ok);
    if (ok) {
        return integer * scale;
    }

    // Parse the number as a (double) floating point number.
    const double dbl = locale.toDouble(number, &ok);
    if (ok) {
        return dbl * scale;
    }

    return 0; // Failed to parse as either integer, or float.
}

/*!
 * Processes the relevant options from the command line \a parser.
 *
 * On success, returns an empty QStringList, otherwise returns a list of CLI errors that the caller
 * should report appropriately before exiting.
 *
 * This base implementations performs some common checks, such as ensuring that required options are
 * present. Derived classes should override this function to perform further processing, typically
 * inovking this base implementation as a first step, such as:
 *
 * ```
 * QStringList CustomCommand::processOptions(const QCommandLineParser &parser)
 * {
 *     QStringList errors = AbstractCommand::processOptions(parser);
 *     if (!errors.isEmpty()) {
 *         return errors;
 *     }
 *
 *     // Do further procession of options.
 *
 *     return errors;
 * }
 * ```
 */
QStringList AbstractCommand::processOptions(const QCommandLineParser &parser)
{
    // Report any supplied options that are not supported by this command.
    const QStringList suppliedOptions = parser.optionNames();
    const QStringList supportedOptions = this->supportedOptions(parser);
    for (const QString &option: suppliedOptions) {
        if (!supportedOptions.contains(option)) {
            qCInfo(lc).noquote() << tr("Ignoring option: %1").arg(option);
        }
    }
    QStringList errors;

    // Parse the device (name/addr/uuid) option.
    if (parser.isSet(QLatin1String("device"))) {
        deviceToScanFor = parser.value(QLatin1String("device"));
    }

    // Parse the output format options (if supported, and supplied).
    if ((supportedOptions.contains(QLatin1String("output"))) && // Derived classes may have removed.
        (parser.isSet(QLatin1String("output"))))
    {
        const QString output = parser.value(QLatin1String("output")).toLower();
        if (output == QLatin1String("csv")) {
            format = OutputFormat::Csv;
        } else if (output == QLatin1String("json")) {
            format = OutputFormat::Json;
        } else if (output == QLatin1String("text")) {
            format = OutputFormat::Text;
        } else {
            errors.append(tr("Unknown output format: %1").arg(output));
        }
    }

    // Parse the device scan timeout option.
    if (parser.isSet(QLatin1String("timeout"))) {
        const quint32 timeout = parseMilliValue(parser.value(QLatin1String("timeout")),
                                                QLatin1String("s"), 500);
        if (timeout == 0) {
            errors.append(tr("Invalid timeout: %1").arg(parser.value(QLatin1String("timeout"))));
        } else {
            discoveryAgent->setLowEnergyDiscoveryTimeout(timeout);
            qCDebug(lc).noquote() << tr("Set scan timeout to %1").arg(
                discoveryAgent->lowEnergyDiscoveryTimeout());
        }
    }

    // Return errors for any required options that are absent.
    const QStringList requiredOptions = this->requiredOptions(parser);
    for (const QString &option: requiredOptions) {
        if (!parser.isSet(option)) {
            errors.append(tr("Missing required option: %1").arg(option));
        }
    }
    return errors;
}

/*!
 * \fn virtual bool AbstractCommand::start()
 *
 * Begins the functionality of this command, and returns `true` if begun successfully, `false`
 * otherwise.
 */

/*!
 * \fn virtual void AbstractCommand::deviceDiscovered(const QBluetoothDeviceInfo &info) = 0
 *
 * Handles PokitDiscoveryAgent::pokitDeviceDiscovered signal. Derived classes must
 * implement this slot to begin whatever actions are relevant when a Pokit device has been
 * discovered. For example, the 'scan' command would simply output the \a info details, whereas
 * most other commands would begin connecting if \a info is the device they're after.
 */

/*!
 * \fn virtual void AbstractCommand::deviceDiscoveryFinished() = 0
 *
 * Handles PokitDiscoveryAgent::deviceDiscoveryFinished signal. Derived classes must
 * implement this slot to perform whatever actions are appropraite when discovery is finished.
 * For example, the 'scan' command would simply exit, whereas most other commands would verify that
 * an appropriate device was found.
 */
