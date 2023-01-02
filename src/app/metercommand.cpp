// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "metercommand.h"

#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

/*!
 * \class MeterCommand
 *
 * The MeterCommand class implements the `meter` CLI command.
 */

/*!
 * Construct a new MeterCommand object with \a parent.
 */
MeterCommand::MeterCommand(QObject * const parent) : DeviceCommand(parent),
    service(nullptr), settings{
        MultimeterService::Mode::DcVoltage,
        MultimeterService::VoltageRange::AutoRange,
        1000
    }, samplesToGo(-1), showCsvHeader(true)
{

}

QStringList MeterCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
        QLatin1String("mode"),
    };
}

QStringList MeterCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser) + QStringList{
        QLatin1String("interval"),
        QLatin1String("range"),
        QLatin1String("samples"),
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
    const QString mode = parser.value(QLatin1String("mode")).trimmed().toLower();
    if (mode.startsWith(QLatin1String("ac v")) || mode.startsWith(QLatin1String("vac"))) {
        settings.mode = MultimeterService::Mode::AcVoltage;
    } else if (mode.startsWith(QLatin1String("dc v")) || mode.startsWith(QLatin1String("vdc"))) {
        settings.mode = MultimeterService::Mode::DcVoltage;
    } else if (mode.startsWith(QLatin1String("ac c")) || mode.startsWith(QLatin1String("aac"))) {
        settings.mode = MultimeterService::Mode::AcCurrent;
    } else if (mode.startsWith(QLatin1String("dc c")) || mode.startsWith(QLatin1String("adc"))) {
        settings.mode = MultimeterService::Mode::DcCurrent;
    } else if (mode.startsWith(QLatin1String("res"))) {
        settings.mode = MultimeterService::Mode::Resistance;
    } else if (mode.startsWith(QLatin1String("dio"))) {
        settings.mode = MultimeterService::Mode::Diode;
    } else if (mode.startsWith(QLatin1String("cont"))) {
       settings.mode = MultimeterService::Mode::Continuity;
    } else if (mode.startsWith(QLatin1String("temp"))) {
       settings.mode = MultimeterService::Mode::Temperature;
    } else {
        errors.append(tr("Unknown meter mode: %1").arg(parser.value(QLatin1String("mode"))));
        return errors;
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

    // Parse the range option.
    if (parser.isSet(QLatin1String("range"))) {
        const QString value = parser.value(QLatin1String("range"));
        const bool isAuto = (value.trimmed().compare(QLatin1String("auto"), Qt::CaseInsensitive) == 0);
        QString unit; quint32 sensibleMinimum = 0;
        switch (settings.mode) {
        case MultimeterService::Mode::DcVoltage:
        case MultimeterService::Mode::AcVoltage:
            if (isAuto) {
                settings.range.voltageRange = MultimeterService::VoltageRange::AutoRange;
            }
            unit = QLatin1String("V");
            sensibleMinimum = 50; // mV.
            break;
        case MultimeterService::Mode::DcCurrent:
        case MultimeterService::Mode::AcCurrent:
            if (isAuto) {
                settings.range.currentRange = MultimeterService::CurrentRange::AutoRange;
            }
            unit = QLatin1String("A");
            sensibleMinimum = 5; // mA.
            break;
        case MultimeterService::Mode::Resistance:
            if (isAuto) {
                settings.range.resistanceRange = MultimeterService::ResistanceRange::AutoRange;
            }
            unit = QLatin1String("ohms");
            sensibleMinimum = 0; // Unused.
            break;
        default:
            qCInfo(lc).noquote() << tr("Ignoring range value: %1").arg(value);
        }
        if ((!unit.isEmpty()) && (!isAuto)) { // isEmpty indicates a mode that has no range option.
            const quint32 rangeMax = (sensibleMinimum == 0)
                ? parseWholeValue(value, unit) : parseMilliValue(value, unit, sensibleMinimum);
            if (rangeMax == 0) {
                errors.append(tr("Invalid range value: %1").arg(value));
            } else {
                settings.range = lowestRange(settings.mode, rangeMax);
            }
        }
    }

    // Parse the samples option.
    if (parser.isSet(QLatin1String("samples"))) {
        const QString value = parser.value(QLatin1String("samples"));
        const quint32 samples = parseWholeValue(value, QLatin1String("S"));
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
    const QString range = MultimeterService::toString(settings.range, settings.mode);
    qCInfo(lc).noquote() << tr("Measuring %1, with range %2, every %L3ms.").arg(
        MultimeterService::toString(settings.mode),
        (range.isNull()) ? QString::fromLatin1("N/A") : range).arg(settings.updateInterval);
    service->setSettings(settings);
}

/*!
 * Returns the lowest \a mode range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
MultimeterService::Range MeterCommand::lowestRange(
    const MultimeterService::Mode mode, const quint32 desiredMax)
{
    MultimeterService::Range range;
    switch (mode) {
    case MultimeterService::Mode::DcVoltage:
    case MultimeterService::Mode::AcVoltage:
        range.voltageRange = lowestVoltageRange(desiredMax);
        break;
    case MultimeterService::Mode::DcCurrent:
    case MultimeterService::Mode::AcCurrent:
        range.currentRange = lowestCurrentRange(desiredMax);
        break;
    case MultimeterService::Mode::Resistance:
        range.resistanceRange = lowestResistanceRange(desiredMax);
        break;
    default:
        qCWarning(lc).noquote() << tr("Mode does not support range.");
        range.voltageRange = MultimeterService::VoltageRange::AutoRange;
    }
    return range;
}

#define POKIT_APP_IF_LESS_THAN_RETURN(value, label) \
if (value <=  MultimeterService::maxValue(MultimeterService::label).toUInt()) { \
    return MultimeterService::label; \
}

/*!
 * Returns the lowest current range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
MultimeterService::CurrentRange MeterCommand::lowestCurrentRange(const quint32 desiredMax)
{
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_0_to_10mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_10mA_to_30mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_30mA_to_150mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_150mA_to_300mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_300mA_to_3A)
    return MultimeterService::CurrentRange::AutoRange;
}

/*!
 * Returns the lowest resistance range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
MultimeterService::ResistanceRange MeterCommand::lowestResistanceRange(const quint32 desiredMax)
{
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, ResistanceRange::_0_to_160)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, ResistanceRange::_160_to_330)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, ResistanceRange::_330_to_890)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, ResistanceRange::_890_to_1K5)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, ResistanceRange::_1K5_to_10K)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, ResistanceRange::_10K_to_100K)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, ResistanceRange::_100K_to_470K)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, ResistanceRange::_470K_to_1M)
    return MultimeterService::ResistanceRange::AutoRange;
}

/*!
 * Returns the lowest voltage range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
MultimeterService::VoltageRange MeterCommand::lowestVoltageRange(const quint32 desiredMax)
{
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_0_to_300mV)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_300mV_to_2V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_2V_to_6V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_6V_to_12V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_12V_to_30V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_30V_to_60V)
    return MultimeterService::VoltageRange::AutoRange;
}

#undef POKIT_APP_IF_LESS_THAN_RETURN

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
 * Outputs meter \a reading in the selected ouput format.
 */
void MeterCommand::outputReading(const MultimeterService::Reading &reading)
{
    QString status;
    if (reading.status == MultimeterService::MeterStatus::Error) {
        status = QLatin1String("Error");
    } else switch (reading.mode) {
    case MultimeterService::Mode::Idle:
        break;
    case MultimeterService::Mode::DcVoltage:
    case MultimeterService::Mode::AcVoltage:
    case MultimeterService::Mode::DcCurrent:
    case MultimeterService::Mode::AcCurrent:
    case MultimeterService::Mode::Resistance:
        status = (reading.status == MultimeterService::MeterStatus::AutoRangeOn)
            ? tr("Auto Range On") : tr("Auto Range Off");
        break;
    case MultimeterService::Mode::Continuity:
        status = (reading.status == MultimeterService::MeterStatus::Continuity)
            ? tr("Continuity") : tr("No continuity");
        break;
    case MultimeterService::Mode::Temperature:
    case MultimeterService::Mode::Diode:
        status = tr("Ok");
        break;
    }

    QString units;
    switch (reading.mode) {
    case MultimeterService::Mode::Idle:        break;
    case MultimeterService::Mode::DcVoltage:   units = QLatin1String("Vdc"); break;
    case MultimeterService::Mode::AcVoltage:   units = QLatin1String("Vac"); break;
    case MultimeterService::Mode::DcCurrent:   units = QLatin1String("Adc"); break;
    case MultimeterService::Mode::AcCurrent:   units = QLatin1String("Aac"); break;
    case MultimeterService::Mode::Resistance:  units = QString::fromUtf8("Ω"); break;
    case MultimeterService::Mode::Diode:       break;
    case MultimeterService::Mode::Continuity:  break;
    case MultimeterService::Mode::Temperature: units = QString::fromUtf8("°C"); break;
    }

    QString range;
    QVariant rangeMin, rangeMax;
    switch (reading.mode) {
    case MultimeterService::Mode::Idle:        break;
    case MultimeterService::Mode::DcVoltage:
    case MultimeterService::Mode::AcVoltage:
        range = MultimeterService::toString(reading.range.voltageRange);
        rangeMin = MultimeterService::minValue(reading.range.voltageRange);
        rangeMax = MultimeterService::maxValue(reading.range.voltageRange);
        break;
    case MultimeterService::Mode::DcCurrent:
    case MultimeterService::Mode::AcCurrent:
        range = MultimeterService::toString(reading.range.currentRange);
        rangeMin = MultimeterService::minValue(reading.range.currentRange);
        rangeMax = MultimeterService::maxValue(reading.range.currentRange);
        break;
    case MultimeterService::Mode::Resistance:
        range = MultimeterService::toString(reading.range.resistanceRange);
        rangeMin = MultimeterService::minValue(reading.range.resistanceRange);
        rangeMax = MultimeterService::maxValue(reading.range.resistanceRange);
        break;
    case MultimeterService::Mode::Diode:       break;
    case MultimeterService::Mode::Continuity:  break;
    case MultimeterService::Mode::Temperature: break;
    }

    switch (format) {
    case OutputFormat::Csv:
        for (; showCsvHeader; showCsvHeader = false) {
            std::cout << qUtf8Printable(tr("mode,value,units,status,range_min_milli,range_max_milli\n"));
        }
        std::cout << qUtf8Printable(QString::fromLatin1("%1,%2,%3,%4,%5,%6\n")
            .arg(escapeCsvField(MultimeterService::toString(reading.mode)))
            .arg(reading.value, 0, 'f').arg(units, status, rangeMin.toString(), rangeMax.toString())
            );
        break;
    case OutputFormat::Json: {
        QJsonObject jsonObject{
            { QLatin1String("status"), status },
            { QLatin1String("value"), qIsInf(reading.value) ?
                QJsonValue(tr("Infinity")) : QJsonValue(reading.value) },
            { QLatin1String("mode"),   MultimeterService::toString(reading.mode) },
        };
        if ((!rangeMin.isNull()) || (!rangeMax.isNull())) {
            jsonObject.insert(QLatin1String("range"), QJsonObject{
                { QLatin1String("min"),
                    #if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
                    (rangeMin.typeId() == QMetaType::Int)
                    #else
                    (rangeMin.type() == QVariant::Int)
                    #endif
                    ? QJsonValue(rangeMin.toInt()/1000.0) : rangeMin.toJsonValue() },
                { QLatin1String("max"),
                    #if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
                    (rangeMax.typeId() == QMetaType::Int) ?
                    #else
                    (rangeMax.type() == QVariant::Int) ?
                    #endif
                    QJsonValue(rangeMax.toInt()/1000.0) : rangeMax.toJsonValue() },
            });
        }
        std::cout << QJsonDocument(jsonObject).toJson().toStdString();
    }   break;
    case OutputFormat::Text:
        std::cout << qUtf8Printable(tr("Mode:   %1 (0x%2)\n").arg(MultimeterService::toString(reading.mode))
            .arg((quint8)reading.mode,2,16,QLatin1Char('0')));
        std::cout << qUtf8Printable(tr("Value:  %1 %2\n").arg(reading.value,0,'f').arg(units));
        std::cout << qUtf8Printable(tr("Status: %1 (0x%2)\n").arg(status)
            .arg((quint8)reading.status,2,16,QLatin1Char('0')));
        std::cout << qUtf8Printable(tr("Range:  %1 (0x%2)\n").arg(range)
            .arg((quint8)reading.range.voltageRange,2,16,QLatin1Char('0')));
        break;
    }

    if ((samplesToGo > 0) && (--samplesToGo == 0)) {
        if (device) disconnect(); // Will exit the application once disconnected.
    }
}
