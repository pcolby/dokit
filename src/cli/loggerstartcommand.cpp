// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "loggerstartcommand.h"
#include "../stringliterals_p.h"

#include <qtpokit/pokitdevice.h>

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

DOKIT_USE_STRINGLITERALS

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
        u"mode"_s,
    };
}

QStringList LoggerStartCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser) + QStringList{
        u"interval"_s,
        u"range"_s, // May still be required by processOptions(), depending on the --mode option's value.
        u"timestamp"_s,
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
    if (const QString mode = parser.value(u"mode"_s).trimmed().toLower();
        mode.startsWith(u"ac v"_s) || mode.startsWith(u"vac"_s)) {
        settings.mode = DataLoggerService::Mode::AcVoltage;
        minRangeFunc = minVoltageRange;
    } else if (mode.startsWith(u"dc v"_s) || mode.startsWith(u"vdc"_s)) {
        settings.mode = DataLoggerService::Mode::DcVoltage;
        minRangeFunc = minVoltageRange;
    } else if (mode.startsWith(u"ac c"_s) || mode.startsWith(u"aac"_s)) {
        settings.mode = DataLoggerService::Mode::AcCurrent;
        minRangeFunc = minCurrentRange;
    } else if (mode.startsWith(u"dc c"_s) || mode.startsWith(u"adc"_s)) {
        settings.mode = DataLoggerService::Mode::DcCurrent;
        minRangeFunc = minCurrentRange;
    } else if (mode.startsWith(u"temp"_s)) {
        settings.mode = DataLoggerService::Mode::Temperature;
        minRangeFunc = nullptr;
    } else {
        minRangeFunc = nullptr;
        errors.append(tr("Unknown logger mode: %1").arg(parser.value(u"mode"_s)));
        return errors;
    }

    // Parse the range option.
    rangeOptionValue = 0;
    if (parser.isSet(u"range"_s)) {
        const QString value = parser.value(u"range"_s);
        switch (settings.mode) {
        case DataLoggerService::Mode::DcVoltage:
        case DataLoggerService::Mode::AcVoltage:
            rangeOptionValue = parseNumber<std::milli>(value, u"V"_s, 50); // mV.
            break;
        case DataLoggerService::Mode::DcCurrent:
        case DataLoggerService::Mode::AcCurrent:
            rangeOptionValue = parseNumber<std::milli>(value, u"A"_s, 5); // mA.
            break;
        default:
            qCInfo(lc).noquote() << tr("Ignoring range value: %1").arg(value);
        }
        if ((minRangeFunc != nullptr) && (rangeOptionValue == 0)) {
            errors.append(tr("Invalid range value: %1").arg(value));
        }
    } else if (settings.mode != DataLoggerService::Mode::Temperature) {
        errors.append(tr("Missing required option for logger mode '%1': range").arg(parser.value(u"mode"_s)));
    }

    // Parse the interval option.
    if (parser.isSet(u"interval"_s)) {
        const QString value = parser.value(u"interval"_s);
        const quint32 interval = parseNumber<std::milli>(value, u"s"_s, 500);
        if (interval == 0) {
            errors.append(tr("Invalid interval value: %1").arg(value));
        } else {
            settings.updateInterval = interval;
        }
    }

    // Parse the timestamp option.
    settings.timestamp = (quint32)QDateTime::currentSecsSinceEpoch(); // Note, subject to Y2038 epochalypse.
    if (parser.isSet(u"timestamp"_s)) {
        const QString value = parser.value(u"timestamp"_s);
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
        connect(service, &DataLoggerService::settingsWritten, this, &LoggerStartCommand::settingsWritten);
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
    settings.range = (minRangeFunc == nullptr) ? 0 : minRangeFunc(*service->pokitProduct(), rangeOptionValue);
    const QString range = service->toString(settings.range, settings.mode);
    qCInfo(lc).noquote() << tr("Logging %1, with range %2, every %L3ms.").arg(DataLoggerService::toString(settings.mode),
        (range.isNull()) ? QString::fromLatin1("N/A") : range).arg(settings.updateInterval);
    service->setSettings(settings);
}

/*!
 * \var LoggerStartCommand::minRangeFunc
 *
 * Pointer to function for converting #rangeOptionValue to a Pokit device's range enumerator. This function pointer
 * is assigned during the command line parsing, but is not invoked until after the device's services are discovered,
 * because prior to that discovery, we don't know which product (Meter vs Pro vs Clamp, etc) we're talking to and thus
 * which enumerator list to be using.
 *
 * If the current mode does not support ranges (eg diode, and continuity modes), then this member will be \c nullptr.
 *
 * \see processOptions
 * \see serviceDetailsDiscovered
 */

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
        std::cout << qUtf8Printable(u"true\n"_s);
        break;
    case OutputFormat::Text:
        std::cout << qUtf8Printable(tr("Done.\n"));
        break;
    }
    if (device) disconnect(); // Will exit the application once disconnected.
}
