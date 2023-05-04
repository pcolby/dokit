// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "loggerstartcommand.h"

#include <qtpokit/pokitdevice.h>

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

/*!
 * \class LoggerStartCommand
 *
 * The LoggerStartCommand class implements the `logger` CLI command.
 */

/*!
 * Construct a new LoggerStartCommand object with \a parent.
 */
LoggerStartCommand::LoggerStartCommand(QObject * const parent) : DeviceCommand(parent)
{

}

QStringList LoggerStartCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
        QLatin1String("mode"),
    };
}

QStringList LoggerStartCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser) + QStringList{
        QLatin1String("interval"),
        QLatin1String("range"),
        QLatin1String("timestamp"),
    };
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList LoggerStartCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = DeviceCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    // Parse the (required) mode option.
    const QString mode = parser.value(QLatin1String("mode")).trimmed().toLower();
    if (mode.startsWith(QLatin1String("ac v")) || mode.startsWith(QLatin1String("vac"))) {
        settings.mode = DataLoggerService::Mode::AcVoltage;
    } else if (mode.startsWith(QLatin1String("dc v")) || mode.startsWith(QLatin1String("vdc"))) {
        settings.mode = DataLoggerService::Mode::DcVoltage;
    } else if (mode.startsWith(QLatin1String("ac c")) || mode.startsWith(QLatin1String("aac"))) {
        settings.mode = DataLoggerService::Mode::AcCurrent;
    } else if (mode.startsWith(QLatin1String("dc c")) || mode.startsWith(QLatin1String("adc"))) {
        settings.mode = DataLoggerService::Mode::DcCurrent;
    } else if (mode.startsWith(QLatin1String("temp"))) {
        settings.mode = DataLoggerService::Mode::Temperature;
    } else {
        errors.append(tr("Unknown logger mode: %1").arg(parser.value(QLatin1String("mode"))));
        return errors;
    }

    // Parse the range option.
    if (parser.isSet(QLatin1String("range"))) {
        const QString value = parser.value(QLatin1String("range"));
        QString unit; quint32 sensibleMinimum = 0;
        switch (settings.mode) {
        case DataLoggerService::Mode::Idle:
            Q_ASSERT(false); // Not possible, since the mode parsing above never allows Idle.
            break;
        case DataLoggerService::Mode::DcVoltage:
        case DataLoggerService::Mode::AcVoltage:
            unit = QLatin1String("V");
            sensibleMinimum = 50; // mV.
            break;
        case DataLoggerService::Mode::DcCurrent:
        case DataLoggerService::Mode::AcCurrent:
            unit = QLatin1String("A");
            sensibleMinimum = 5; // mA.
            break;
        case DataLoggerService::Mode::Temperature:
        default:
            qCInfo(lc).noquote() << tr("Ignoring range value: %1").arg(value);
        }
        if (unit.isEmpty()) {
            // The only mode that does not take a range, and thus we don't assign a unit above.
            Q_ASSERT(settings.mode == DataLoggerService::Mode::Temperature);
        } else {
            const quint32 rangeMax = parseMilliValue(value, unit, sensibleMinimum);
            if (rangeMax == 0) {
                errors.append(tr("Invalid range value: %1").arg(value));
            } else {
                settings.range = lowestRange(settings.mode, rangeMax);
            }
        }
    } else if (settings.mode != DataLoggerService::Mode::Temperature) {
        errors.append(tr("Missing required option for logger mode '%1': range")
            .arg(parser.value(QLatin1String("mode"))));
    }

    // Parse the interval option.
    if (parser.isSet(QLatin1String("interval"))) {
        const QString value = parser.value(QLatin1String("interval"));
        const quint32 interval = parseMilliValue(value, QLatin1String("s"), 500);
        if (interval == 0) {
            errors.append(tr("Invalid interval value: %1").arg(value));
        } else {
            settings.updateInterval = interval;
        }
    }

    // Parse the timestamp option.
    settings.timestamp = (quint32)QDateTime::currentSecsSinceEpoch(); // Note, subject to Y2038 epochalypse.
    if (parser.isSet(QLatin1String("timestamp"))) {
        const QString value = parser.value(QLatin1String("timestamp"));
        QLocale locale; bool ok;
        static_assert(sizeof(uint) == sizeof(settings.timestamp), "QLocale has no toUint32().");
        const int timestamp = locale.toUInt(value, &ok);
        if (!ok) {
            errors.append(tr("Invalid timestamp value: %1").arg(value));
        } else {
            settings.timestamp = timestamp;
        }
    }
    return errors;
}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a DataLoggerService object.
 */
AbstractPokitService * LoggerStartCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->dataLogger();
        Q_ASSERT(service);
        connect(service, &DataLoggerService::settingsWritten,
                this, &LoggerStartCommand::settingsWritten);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override fetches the current device's status, and outputs it in the selected format.
 */
void LoggerStartCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    const QString range = DataLoggerService::toString(settings.range, settings.mode);
    qCInfo(lc).noquote() << tr("Logging %1, with range %2, every %L3ms.").arg(
        DataLoggerService::toString(settings.mode),
        (range.isNull()) ? QString::fromLatin1("N/A") : range).arg(settings.updateInterval);
    service->setSettings(settings);
}

/*!
 * Returns the lowest \a mode range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
DataLoggerService::Range LoggerStartCommand::lowestRange(
    const DataLoggerService::Mode mode, const quint32 desiredMax)
{
    switch (mode) {
    case DataLoggerService::Mode::Idle:
        qCWarning(lc).noquote() << tr("Idle has no defined ranges.");
        Q_ASSERT(false); // Should never have been called with this Idle mode.
        break;
    case DataLoggerService::Mode::DcVoltage:
    case DataLoggerService::Mode::AcVoltage:
        return lowestVoltageRange(desiredMax);
    case DataLoggerService::Mode::DcCurrent:
    case DataLoggerService::Mode::AcCurrent:
        return lowestCurrentRange(desiredMax);
    default:
        qCWarning(lc).noquote() << tr("No defined ranges for mode %1.").arg((quint8)mode);
        Q_ASSERT(false); // Should never have been called with this invalid mode.
    }
    return DataLoggerService::Range();
}

#define DOKIT_CLI_IF_LESS_THAN_RETURN(value, label) \
if (value <=  DataLoggerService::maxValue(DataLoggerService::label).toUInt()) { \
    return DataLoggerService::label; \
}

/*!
 * Returns the lowest current range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
DataLoggerService::CurrentRange LoggerStartCommand::lowestCurrentRange(const quint32 desiredMax)
{
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_0_to_10mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_10mA_to_30mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_30mA_to_150mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_150mA_to_300mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_300mA_to_3A)
    return DataLoggerService::CurrentRange::_300mA_to_3A; // Out of range, so go with the biggest.
}

/*!
 * Returns the lowest voltage range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
DataLoggerService::VoltageRange LoggerStartCommand::lowestVoltageRange(const quint32 desiredMax)
{
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_0_to_300mV)
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_300mV_to_2V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_2V_to_6V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_6V_to_12V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_12V_to_30V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_30V_to_60V)
    return DataLoggerService::VoltageRange::_30V_to_60V; // Out of range, so go with the biggest.
}

#undef DOKIT_CLI_IF_LESS_THAN_RETURN

/*!
 * Invoked when the data logger settings have been written.
 */
void LoggerStartCommand::settingsWritten()
{
    qCDebug(lc).noquote() << tr("Settings written; data logger has started.");
    switch (format) {
    case OutputFormat::Csv:
        std::cout << qUtf8Printable(tr("logger_start_result\nsuccess\n"));
        break;
    case OutputFormat::Json:
        std::cout << qUtf8Printable(QLatin1String("true\n"));
        break;
    case OutputFormat::Text:
        std::cout << qUtf8Printable(tr("Done.\n"));
        break;
    }
    if (device) disconnect(); // Will exit the application once disconnected.
}
