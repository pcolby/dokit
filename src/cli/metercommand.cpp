// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "metercommand.h"
#include "../stringliterals_p.h"

#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

DOKIT_USE_STRINGLITERALS

/*!
 * \class MeterCommand
 *
 * The MeterCommand class implements the `meter` CLI command.
 */

/*!
 * Construct a new MeterCommand object with \a parent.
 */
MeterCommand::MeterCommand(QObject * const parent) : DeviceCommand(parent)
{

}

QStringList MeterCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
        u"mode"_s,
    };
}

QStringList MeterCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser) + QStringList{
        u"interval"_s,
        u"range"_s,
        u"samples"_s,
    };
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList MeterCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = DeviceCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    // Parse the (required) mode option.
    if (const QString mode = parser.value(u"mode"_s).trimmed().toLower();
        mode.startsWith(u"ac v"_s) || mode.startsWith(u"vac"_s)) {
        settings.mode = MultimeterService::Mode::AcVoltage;
        minRangeFunc = minVoltageRange;
    } else if (mode.startsWith(u"dc v"_s) || mode.startsWith(u"vdc"_s)) {
        settings.mode = MultimeterService::Mode::DcVoltage;
        minRangeFunc = minVoltageRange;
    } else if (mode.startsWith(u"ac c"_s) || mode.startsWith(u"aac"_s)) {
        settings.mode = MultimeterService::Mode::AcCurrent;
        minRangeFunc = minCurrentRange;
    } else if (mode.startsWith(u"dc c"_s) || mode.startsWith(u"adc"_s)) {
        settings.mode = MultimeterService::Mode::DcCurrent;
        minRangeFunc = minCurrentRange;
    } else if (mode.startsWith(u"res"_s)) {
        settings.mode = MultimeterService::Mode::Resistance;
        minRangeFunc = minResistanceRange;
    } else if (mode.startsWith(u"dio"_s)) {
        settings.mode = MultimeterService::Mode::Diode;
        minRangeFunc = nullptr;
    } else if (mode.startsWith(u"cont"_s)) {
        settings.mode = MultimeterService::Mode::Continuity;
        minRangeFunc = nullptr;
    } else if (mode.startsWith(u"temp"_s)) {
        settings.mode = MultimeterService::Mode::Temperature;
        minRangeFunc = nullptr;
    } else if (mode.startsWith(u"cap"_s)) {
        settings.mode = MultimeterService::Mode::Capacitance;
        minRangeFunc = minCapacitanceRange;
    } else {
        errors.append(tr("Unknown meter mode: %1").arg(parser.value(u"mode"_s)));
        return errors;
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

    // Parse the range option.
    rangeOptionValue = 0; // Default to auto.
    if (parser.isSet(u"range"_s)) {
        const QString value = parser.value(u"range"_s);
        if (value.trimmed().compare(u"auto"_s, Qt::CaseInsensitive) != 0) {
            switch (settings.mode) {
            case MultimeterService::Mode::DcVoltage:
            case MultimeterService::Mode::AcVoltage:
                rangeOptionValue =  parseNumber<std::milli>(value, u"V"_s, 50); // mV.
                break;
            case MultimeterService::Mode::DcCurrent:
            case MultimeterService::Mode::AcCurrent:
                rangeOptionValue = parseNumber<std::milli>(value, u"A"_s, 5); // mA.
                break;
            case MultimeterService::Mode::Resistance:
                rangeOptionValue = parseNumber<std::ratio<1>>(value, u"ohms"_s);
                break;
            case MultimeterService::Mode::Capacitance:
                rangeOptionValue = parseNumber<std::nano>(value, u"F"_s, 500); // pF.
                break;
            default:
                qCInfo(lc).noquote() << tr("Ignoring range value: %1").arg(value);
            }
            if ((minRangeFunc != nullptr) && (rangeOptionValue == 0)) {
                errors.append(tr("Invalid range value: %1").arg(value));
            }
        }
    }

    // Parse the samples option.
    if (parser.isSet(u"samples"_s)) {
        const QString value = parser.value(u"samples"_s);
        const quint32 samples = parseNumber<std::ratio<1>>(value, u"S"_s);
        if (samples == 0) {
            errors.append(tr("Invalid samples value: %1").arg(value));
        } else {
            samplesToGo = samples;
        }
    }
    return errors;
}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a MultimeterService object.
 */
AbstractPokitService * MeterCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->multimeter();
        Q_ASSERT(service);
        connect(service, &MultimeterService::settingsWritten,
                this, &MeterCommand::settingsWritten);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override fetches the current device's status, and outputs it in the selected format.
 */
void MeterCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    settings.range = (minRangeFunc == nullptr) ? 0 : minRangeFunc(*service->pokitProduct(), rangeOptionValue);
    const QString range = service->toString(settings.range, settings.mode);
    qCInfo(lc).noquote() << tr("Measuring %1, with range %2, every %L3ms.").arg(
        MultimeterService::toString(settings.mode),
        (range.isNull()) ? QString::fromLatin1("N/A") : range).arg(settings.updateInterval);
    service->setSettings(settings);
}

/*!
 * \var MeterCommand::minRangeFunc
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
 * Invoked when the multimeter settings have been written, to begin reading the meter values.
 */
void MeterCommand::settingsWritten()
{
    qCDebug(lc).noquote() << tr("Settings written; starting meter readings...");
    connect(service, &MultimeterService::readingRead,
            this, &MeterCommand::outputReading);
    service->enableReadingNotifications();
}

/*!
 * Outputs meter \a reading in the selected output format.
 */
void MeterCommand::outputReading(const MultimeterService::Reading &reading)
{
    QString status;
    if (reading.status == MultimeterService::MeterStatus::Error) {
        status = u"Error"_s;
    } else switch (reading.mode) {
    case MultimeterService::Mode::Idle:
        break;
    case MultimeterService::Mode::DcVoltage:
    case MultimeterService::Mode::AcVoltage:
    case MultimeterService::Mode::DcCurrent:
    case MultimeterService::Mode::AcCurrent:
    case MultimeterService::Mode::Resistance:
    case MultimeterService::Mode::Capacitance:
        status = (reading.status == MultimeterService::MeterStatus::AutoRangeOn)
            ? tr("Auto Range On") : tr("Auto Range Off");
        break;
    case MultimeterService::Mode::Continuity:
        status = (reading.status == MultimeterService::MeterStatus::Continuity)
            ? tr("Continuity") : tr("No continuity");
        break;
    case MultimeterService::Mode::Temperature:
    case MultimeterService::Mode::ExternalTemperature:
    case MultimeterService::Mode::Diode:
        status = tr("Ok");
        break;
    }

    QString unit;
    switch (reading.mode) {
    case MultimeterService::Mode::Idle:        break;
    case MultimeterService::Mode::DcVoltage:   unit = u"Vdc"_s; break;
    case MultimeterService::Mode::AcVoltage:   unit = u"Vac"_s; break;
    case MultimeterService::Mode::DcCurrent:   unit = u"Adc"_s; break;
    case MultimeterService::Mode::AcCurrent:   unit = u"Aac"_s; break;
    case MultimeterService::Mode::Resistance:  unit = QString::fromUtf8("Ω"); break;
    case MultimeterService::Mode::Diode:       break;
    case MultimeterService::Mode::Continuity:  break;
    case MultimeterService::Mode::Temperature: unit = QString::fromUtf8("°C"); break;
    case MultimeterService::Mode::Capacitance: unit = QString::fromUtf8("F");  break;
    case MultimeterService::Mode::ExternalTemperature: unit = QString::fromUtf8("°C"); break;
    }

    const QString range = service->toString(reading.range, reading.mode);

    switch (format) {
    case OutputFormat::Csv:
        for (; showCsvHeader; showCsvHeader = false) {
            std::cout << qUtf8Printable(tr("mode,value,unit,status,range\n"));
        }
        std::cout << qUtf8Printable(QString::fromLatin1("%1,%2,%3,%4,%5\n")
            .arg(escapeCsvField(MultimeterService::toString(reading.mode)))
            .arg(reading.value, 0, 'f').arg(unit, status, range)
            );
        break;
    case OutputFormat::Json: {
        QJsonObject object{
            { u"status"_s, status },
            { u"value"_s, qIsInf(reading.value) ?
                QJsonValue(tr("Infinity")) : QJsonValue(reading.value) },
            { u"mode"_s,   MultimeterService::toString(reading.mode) },
        };
        if (!unit.isNull()) {
            object.insert(u"unit"_s, unit);
        }
        if (!range.isNull()) {
            object.insert(u"range"_s, range);
        }
        std::cout << QJsonDocument(object).toJson().toStdString();
    }   break;
    case OutputFormat::Text:
        std::cout << qUtf8Printable(tr("Mode:   %1 (0x%2)\n").arg(MultimeterService::toString(reading.mode))
            .arg((quint8)reading.mode,2,16,'0'_L1));
        std::cout << qUtf8Printable(tr("Value:  %1 %2\n").arg(reading.value,0,'f').arg(unit));
        std::cout << qUtf8Printable(tr("Status: %1 (0x%2)\n").arg(status)
            .arg((quint8)reading.status,2,16,'0'_L1));
        std::cout << qUtf8Printable(tr("Range:  %1 (0x%2)\n").arg(range)
            .arg((quint8)reading.range,2,16,'0'_L1));
        break;
    }

    if ((samplesToGo > 0) && (--samplesToGo == 0)) {
        if (device) disconnect(); // Will exit the application once disconnected.
    }
}
