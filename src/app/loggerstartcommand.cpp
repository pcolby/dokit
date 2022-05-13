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

#include "loggerstartcommand.h"

#include <qtpokit/pokitdevice.h>

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

/*!
 * \class LoggerStartCommand
 *
 * The LoggerStartCommand class implements the `logger` CLI command.
 */

/*!
 * Construct a new LoggerStartCommand object with \a parent.
 */
LoggerStartCommand::LoggerStartCommand(QObject * const parent) : DeviceCommand(parent),
    service(nullptr), settings{
        DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
        { DataLoggerService::VoltageRange::_30V_to_60V }, 60000, 0}
{

}

QStringList LoggerStartCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
        QLatin1String("mode"),
        QLatin1String("range"),
    };
}

QStringList LoggerStartCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser) + QStringList{
        QLatin1String("interval"),
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
    } else if (mode.startsWith(QLatin1String("dc c")) || mode.startsWith(QLatin1String("aac"))) {
        settings.mode = DataLoggerService::Mode::DcCurrent;
    } else {
        errors.append(tr("Unknown logger mode: %1").arg(parser.value(QLatin1String("mode"))));
        return errors;
    }

    // Parse the (required) range option.
    {
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
        }
        Q_ASSERT(!unit.isEmpty());
        const quint32 rangeMax = parseMilliValue(value, unit, sensibleMinimum);
        if (rangeMax == 0) {
            errors.append(tr("Invalid range value: %1").arg(value));
        } else {
            settings.range = lowestRange(settings.mode, rangeMax);
        }
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
    settings.timestamp = QDateTime::currentSecsSinceEpoch(); // Note, subject to Y2038 epochalypse.
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
    DataLoggerService::Range range{ DataLoggerService::VoltageRange::_6V_to_12V };
    switch (mode) {
    case DataLoggerService::Mode::Idle:
        qCWarning(lc).noquote() << tr("Idle has no defined ranges.");
        Q_ASSERT(false); // Should never have been called with this Idle mode.
        break;
    case DataLoggerService::Mode::DcVoltage:
    case DataLoggerService::Mode::AcVoltage:
        range.voltageRange = lowestVoltageRange(desiredMax);
        break;
    case DataLoggerService::Mode::DcCurrent:
    case DataLoggerService::Mode::AcCurrent:
        range.currentRange = lowestCurrentRange(desiredMax);
        break;
    default:
        qCWarning(lc).noquote() << tr("No defined ranges for mode %1.").arg((quint8)mode);
        Q_ASSERT(false); // Should never have been called with this invalid mode.
    }
    return range;
}

#define POKIT_APP_IF_LESS_THAN_RETURN(value, label) \
if (value <=  DataLoggerService::maxValue(DataLoggerService::label).toUInt()) { \
    return DataLoggerService::label; \
}

/*!
 * Returns the lowest current range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
DataLoggerService::CurrentRange LoggerStartCommand::lowestCurrentRange(const quint32 desiredMax)
{
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_0_to_10mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_10mA_to_30mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_30mA_to_150mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_150mA_to_300mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_300mA_to_3A)
    return DataLoggerService::CurrentRange::_300mA_to_3A; // Out of range, so go with the biggest.
}

/*!
 * Returns the lowest voltage range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
DataLoggerService::VoltageRange LoggerStartCommand::lowestVoltageRange(const quint32 desiredMax)
{
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_0_to_300mV)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_300mV_to_2V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_2V_to_6V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_6V_to_12V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_12V_to_30V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_30V_to_60V)
    return DataLoggerService::VoltageRange::_30V_to_60V; // Out of range, so go with the biggest.
}

#undef POKIT_APP_IF_LESS_THAN_RETURN

/*!
 * Invoked when the data logger settings have been written.
 */
void LoggerStartCommand::settingsWritten()
{
    qCDebug(lc).noquote() << tr("Settings written; data logger has started.");
    /// \todo Output the settings used as CSV, JSON and Text.
    QCoreApplication::exit(EXIT_SUCCESS);
}
