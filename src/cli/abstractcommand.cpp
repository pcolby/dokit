// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "abstractcommand.h"

#include <qtpokit/pokitdevice.h>
#include <qtpokit/pokitdiscoveryagent.h>

#include <QLocale>

#include <ratio>

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
    discoveryAgent(new PokitDiscoveryAgent(this))
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
    Q_UNUSED(parser)
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
        QLatin1String("device"), QLatin1String("d"),
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
 * QCOMPARE(escapeCsvField("abc"), "abc");           // Returned unchanged.
 * QCOMPARE(escapeCsvField("a,c"), R"("a,c")");      // Wrapped in double-quotes.
 * QCOMPARE(escapeCsvField(R"(a"c)"), R("("a""c")"); // Existing double-quotes doubled, then wrapped.
 * ```
 */
QString AbstractCommand::escapeCsvField(const QString &field)
{
    if (field.contains(QLatin1Char(','))||field.contains(QLatin1Char('\r'))||
        field.contains(QLatin1Char('"'))||field.contains(QLatin1Char('\n')))
    {
        return QString::fromLatin1(R"("%1")").arg(
            QString(field).replace(QLatin1Char('"'), QLatin1String(R"("")")));
    } else return field;
}

/*!
 * \internal
 * A (run-time) class approximately equivalent to the compile-time std::ratio template.
 */
struct Ratio {
    std::intmax_t num { 0 }; ///< Numerator.
    std::intmax_t den { 0 }; ///< Denominator.
    //! Returns \a true if both #num and #den are non-zero.
    bool isValid() const { return (num != 0) && (den != 0); }
};

/*!
 * \internal
 * Returns a (run-time) Ratio representation of (compile-time) ratio \a R.
 */
template<typename R> constexpr Ratio makeRatio() { return Ratio{ R::num, R::den }; }

/*!
 * Returns \a value as an integer multiple of the ratio \a R. The string \a value
 * may end with the optional \a unit, such as `V` or `s`, which may also be preceded with a SI unit
 * prefix such as `m` for `milli`. If \a value contains no SI unit prefix, then the result will be
 * multiplied by 1,000 enough times to be greater than \a sensibleMinimum. This allows for
 * convenient use like:
 *
 * ```
 * const quint32 timeout = parseNumber<std::milli>(parser.value("window"), 's', 500'000);
 * ```
 *
 * So that an unqalified period like "300" will be assumed to be 300 milliseconds, and not 300
 * microseconds, while a period like "1000" will be assume to be 1 second.
 *
 * If conversion fails for any reason, 0 is returned.
 */
template<typename R>
quint32 AbstractCommand::parseNumber(const QString &value, const QString &unit, const quint32 sensibleMinimum)
{
    static const QMap<QChar, Ratio> unitPrefixScaleMap {
        { QLatin1Char('E'), makeRatio<std::exa>()   },
        { QLatin1Char('P'), makeRatio<std::peta>()  },
        { QLatin1Char('T'), makeRatio<std::tera>()  },
        { QLatin1Char('G'), makeRatio<std::giga>()  },
        { QLatin1Char('M'), makeRatio<std::mega>()  },
        { QLatin1Char('K'), makeRatio<std::kilo>()  }, // Not official SI unit prefix, but commonly used.
        { QLatin1Char('k'), makeRatio<std::kilo>()  },
        { QLatin1Char('h'), makeRatio<std::hecto>() },
        { QLatin1Char('d'), makeRatio<std::deci>()  },
        { QLatin1Char('c'), makeRatio<std::centi>() },
        { QLatin1Char('m'), makeRatio<std::milli>() },
        { QLatin1Char('u'), makeRatio<std::micro>() }, // Not official SI unit prefix, but commonly used.
        { QChar   (0x00B5), makeRatio<std::micro>() }, // Unicode micro symbol (μ).
        { QLatin1Char('n'), makeRatio<std::nano>()  },
        { QLatin1Char('p'), makeRatio<std::pico>()  },
        { QLatin1Char('f'), makeRatio<std::femto>() },
        { QLatin1Char('a'), makeRatio<std::atto>()  },
    };

    // Remove the optional (whole) unit suffix.
    Ratio ratio;
    QString number = value.trimmed();
    if ((!unit.isEmpty()) && (number.endsWith(unit, Qt::CaseInsensitive))) {
        number.chop(unit.length());
        ratio = makeRatio<std::ratio<1>>();
    }

    // Parse, and remove, the optional SI unit prefix.
    if (!number.isEmpty()) {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        const QChar siPrefix = number.back(); // QString::back() introduced in Qt 5.10.
        #else
        const QChar siPrefix = number.at(number.size() - 1);
        #endif
        const auto iter = unitPrefixScaleMap.constFind(siPrefix);
        if (iter != unitPrefixScaleMap.constEnd()) {
            Q_ASSERT(iter->isValid());
            ratio = *iter;
            number.chop(1);
        }
    }

    #define DOKIT_RESULT(var) (var * ratio.num * R::den / ratio.den / R::num)
    // Parse the number as an (unsigned) integer.
    QLocale locale; bool ok;
    qulonglong integer = locale.toULongLong(number, &ok);
    if (ok) {
        if (integer == 0) {
            return 0;
        }
        if (!ratio.isValid()) {
            for (ratio = makeRatio<R>(); DOKIT_RESULT(integer) < sensibleMinimum; ratio.num *= 1000);
        }
        return (integer == 0) ? 0u : (quint32)DOKIT_RESULT(integer);
    }

    // Parse the number as a (double) floating point number, and check that it is positive.
    const double dbl = locale.toDouble(number, &ok);
    if ((ok) && (dbl > 0.0)) {
        if (!ratio.isValid()) {
            for (ratio = makeRatio<R>(); DOKIT_RESULT(dbl) < sensibleMinimum; ratio.num *= 1000);
        }
        return (quint32)DOKIT_RESULT(dbl);
    }
    #undef DOKIT_RESULT
    return 0; // Failed to parse as either integer, or float.
}

#define DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(type) template \
quint32 AbstractCommand::parseNumber<type>(const QString &value, const QString &unit, const quint32 sensibleMinimum)
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::exa);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::peta);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::tera);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::giga);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::mega);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::kilo);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::hecto);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::deca);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::ratio<1>);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::deci);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::centi);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::milli);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::micro);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::nano);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::pico);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::femto);
DOKIT_INSTANTIATE_TEMPLATE_FUNCTION(std::atto);
#undef DOKIT_INSTANTIATE_TEMPLATE_FUNCTION

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
    const QStringList suppliedOptionNames = parser.optionNames();
    const QStringList supportedOptionNames = supportedOptions(parser);
    for (const QString &option: suppliedOptionNames) {
        if (!supportedOptionNames.contains(option)) {
            qCInfo(lc).noquote() << tr("Ignoring option: %1").arg(option);
        }
    }
    QStringList errors;

    // Parse the device (name/addr/uuid) option.
    if (parser.isSet(QLatin1String("device"))) {
        deviceToScanFor = parser.value(QLatin1String("device"));
    }

    // Parse the output format options (if supported, and supplied).
    if ((supportedOptionNames.contains(QLatin1String("output"))) && // Derived classes may have removed.
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
        const quint32 timeout = parseNumber<std::milli>(parser.value(QLatin1String("timeout")), QLatin1String("s"), 500);
        if (timeout == 0) {
            errors.append(tr("Invalid timeout: %1").arg(parser.value(QLatin1String("timeout"))));
        } else {
            discoveryAgent->setLowEnergyDiscoveryTimeout(timeout);
            qCDebug(lc).noquote() << tr("Set scan timeout to %1").arg(
                discoveryAgent->lowEnergyDiscoveryTimeout());
        }
    }

    // Return errors for any required options that are absent.
    const QStringList requiredOptionNames = this->requiredOptions(parser);
    for (const QString &option: requiredOptionNames) {
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
