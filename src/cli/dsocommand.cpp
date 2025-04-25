// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "dsocommand.h"
#include "../stringliterals_p.h"

#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

DOKIT_USE_STRINGLITERALS

/*!
 * \class DsoCommand
 *
 * The DsoCommand class implements the `dso` CLI command.
 */

/*!
 * Construct a new DsoCommand object with \a parent.
 */
DsoCommand::DsoCommand(QObject * const parent) : DeviceCommand(parent)
{

}

QStringList DsoCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
        u"mode"_s,
        u"range"_s,
    };
}

QStringList DsoCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser) + QStringList{
        u"interval"_s,
        u"samples"_s,
        u"trigger-level"_s,
        u"trigger-mode"_s,
    };
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList DsoCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = DeviceCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    // Parse the (required) mode option.
    if (const QString mode = parser.value(u"mode"_s).trimmed().toLower();
        mode.startsWith(u"ac v"_s) || mode.startsWith(u"vac"_s)) {
        settings.mode = DsoService::Mode::AcVoltage;
    } else if (mode.startsWith(u"dc v"_s) || mode.startsWith(u"vdc"_s)) {
        settings.mode = DsoService::Mode::DcVoltage;
    } else if (mode.startsWith(u"ac c"_s) || mode.startsWith(u"aac"_s)) {
        settings.mode = DsoService::Mode::AcCurrent;
    } else if (mode.startsWith(u"dc c"_s) || mode.startsWith(u"adc"_s)) {
        settings.mode = DsoService::Mode::DcCurrent;
    } else {
        errors.append(tr("Unknown DSO mode: %1").arg(parser.value(u"mode"_s)));
        return errors;
    }

    // Parse the (required) range option.
    QString unit;
    {
        const QString value = parser.value(u"range"_s);
        quint32 sensibleMinimum = 0;
        switch (settings.mode) {
        case DsoService::Mode::Idle:
            Q_ASSERT(false); // Not possible, since the mode parsing above never allows Idle.
            break;
        case DsoService::Mode::DcVoltage:
        case DsoService::Mode::AcVoltage:
            minRangeFunc = minVoltageRange;
            unit = u"V"_s;
            sensibleMinimum = 50; // mV.
            break;
        case DsoService::Mode::DcCurrent:
        case DsoService::Mode::AcCurrent:
            minRangeFunc = minCurrentRange;
            unit = u"A"_s;
            sensibleMinimum = 5; // mA.
            break;
        }
        Q_ASSERT(!unit.isEmpty());
        rangeOptionValue = parseNumber<std::milli>(value, unit, sensibleMinimum);
        if (rangeOptionValue == 0) {
            errors.append(tr("Invalid range value: %1").arg(value));
        }
    }

    // Parse the trigger-level option.
    if (parser.isSet(u"trigger-level"_s)) {
        const QString value = parser.value(u"trigger-level"_s);
        const quint32 level = parseNumber<std::micro>(value, unit);
        if (level == 0) {
            errors.append(tr("Invalid trigger-level value: %1").arg(value));
        } else {
            settings.triggerLevel = (float)(level/1'000'000.0);
        }
    }

    // Parse the trigger-mode option.
    if (parser.isSet(u"trigger-mode"_s)) {
        const QString triggerMode = parser.value(u"trigger-mode"_s).trimmed().toLower();
        if (triggerMode.startsWith(u"free"_s)) {
            settings.command = DsoService::Command::FreeRunning;
        } else if (triggerMode.startsWith(u"ris"_s)) {
           settings.command = DsoService::Command::RisingEdgeTrigger;
        } else if (triggerMode.startsWith(u"fall"_s)) {
            settings.command = DsoService::Command::FallingEdgeTrigger;
        } else {
            errors.append(tr("Unknown trigger mode: %1").arg(parser.value(u"trigger-mode"_s)));
        }
    }

    // Ensure that if either trigger option is present, then both are.
    if (parser.isSet(u"trigger-level"_s) != parser.isSet(u"trigger-mode"_s)) {
        errors.append(tr("If either option is provided, then both must be: trigger-level, trigger-mode"));
    }

    // Parse the interval option.
    if (parser.isSet(u"interval"_s)) {
        const QString value = parser.value(u"interval"_s);
        const quint32 interval = parseNumber<std::micro>(value, u"s"_s, 500'000);
        if (interval == 0) {
            errors.append(tr("Invalid interval value: %1").arg(value));
        } else {
            settings.samplingWindow = interval;
        }
    }

    // Parse the samples option.
    if (parser.isSet(u"samples"_s)) {
        const QString value = parser.value(u"samples"_s);
        const quint32 samples = parseNumber<std::ratio<1>>(value, u"S"_s);
        if (samples == 0) {
            errors.append(tr("Invalid samples value: %1").arg(value));
        } else if (samples > std::numeric_limits<quint16>::max()) {
            errors.append(tr("Samples value (%1) must be no greater than %2")
                .arg(value).arg(std::numeric_limits<quint16>::max()));
        } else {
            if (samples > 8192) {
                qCWarning(lc).noquote() << tr("Pokit devices do not officially support great than 8192 samples");
            }
            settings.numberOfSamples = (quint16)samples;
        }
    }
    return errors;
}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a DsoService object.
 */
AbstractPokitService * DsoCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->dso();
        Q_ASSERT(service);
        connect(service, &DsoService::settingsWritten,
                this, &DsoCommand::settingsWritten);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override fetches the current device's status, and outputs it in the selected format.
 */
void DsoCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    settings.range = (minRangeFunc == nullptr) ? 0 : minRangeFunc(*service->pokitProduct(), rangeOptionValue);
    const QString range = service->toString(settings.range, settings.mode);
    qCInfo(lc).noquote() << tr("Sampling %1, with range %2, %Ln sample/s over %L3us", nullptr, settings.numberOfSamples)
        .arg(DsoService::toString(settings.mode), (range.isNull()) ? QString::fromLatin1("N/A") : range)
        .arg(settings.samplingWindow);
    service->setSettings(settings);
}

/*!
 * \var DsoCommand::minRangeFunc
 *
 * Pointer to function for converting #rangeOptionValue to a Pokit device's range enumerator. This function pointer
 * is assigned during the command line parsing, but is not invoked until after the device's services are discovere,
 * because prior to that discovery, we don't know which product (Meter vs Pro vs Clamp, etc) we're talking to and thus
 * which enumerator list to be using.
 *
 * If the current mode does not support ranges (eg diode, and continuity modes), then this member will be \c nullptr.
 *
 * \see processOptions
 * \see serviceDetailsDiscovered
 */

/*!
 * Invoked when the DSO settings have been written.
 */
void DsoCommand::settingsWritten()
{
    Q_ASSERT(service);
    qCDebug(lc).noquote() << tr("Settings written; DSO has started.");
    connect(service, &DsoService::metadataRead, this, &DsoCommand::metadataRead);
    connect(service, &DsoService::samplesRead, this, &DsoCommand::outputSamples);
    service->enableMetadataNotifications();
    service->enableReadingNotifications();
}

/*!
 * Invoked when \a metadata has been received from the DSO.
 */
void DsoCommand::metadataRead(const DsoService::Metadata &data)
{
    qCDebug(lc) << "status:" << (int)(data.status);
    qCDebug(lc) << "scale:" << data.scale;
    qCDebug(lc) << "mode:" << DsoService::toString(data.mode);
    qCDebug(lc) << "range:" << service->toString(data.range, data.mode);
    qCDebug(lc) << "samplingWindow:" << (int)data.samplingWindow;
    qCDebug(lc) << "numberOfSamples:" << data.numberOfSamples;
    qCDebug(lc) << "samplingRate:" << data.samplingRate << "Hz";
    this->metadata = data;
    this->samplesToGo = data.numberOfSamples;
}

/*!
 * Outputs DSO \a samples in the selected ouput format.
 */
void DsoCommand::outputSamples(const DsoService::Samples &samples)
{
    QString unit;
    switch (metadata.mode) {
    case DsoService::Mode::DcVoltage: unit = u"Vdc"_s; break;
    case DsoService::Mode::AcVoltage: unit = u"Vac"_s; break;
    case DsoService::Mode::DcCurrent: unit = u"Adc"_s; break;
    case DsoService::Mode::AcCurrent: unit = u"Aac"_s; break;
    default:
        qCDebug(lc).noquote() << tr(R"(No known unit for mode %1 "%2".)").arg((int)metadata.mode)
            .arg(DsoService::toString(metadata.mode));
    }
    const QString range = service->toString(metadata.range, metadata.mode);

    for (const qint16 &sample: samples) {
        static int sampleNumber = 0; ++sampleNumber;
        const float value = sample * metadata.scale;
        switch (format) {
        case OutputFormat::Csv:
            for (; showCsvHeader; showCsvHeader = false) {
                std::cout << qUtf8Printable(tr("sample_number,value,unit,range\n"));
            }
            std::cout << qUtf8Printable(QString::fromLatin1("%1,%2,%3,%4\n")
                .arg(sampleNumber).arg(value).arg(unit, range));
            break;
        case OutputFormat::Json:
            std::cout << QJsonDocument(QJsonObject{
                    { u"value"_s,  value },
                    { u"unit"_s,   unit },
                    { u"range"_s,  range },
                    { u"mode"_s,   DsoService::toString(metadata.mode) },
                }).toJson().toStdString();
            break;
        case OutputFormat::Text:
            std::cout << qUtf8Printable(tr("%1 %2 %3\n").arg(sampleNumber).arg(value).arg(unit));
            break;
        }
        --samplesToGo;
    }
    if (samplesToGo <= 0) {
        qCInfo(lc).noquote() << tr("Finished fetching %Ln sample/s (with %L2 to remaining).",
            nullptr, metadata.numberOfSamples).arg(samplesToGo);
        if (device) disconnect(); // Will exit the application once disconnected.
    }
}
