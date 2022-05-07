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

#include "multimetercommand.h"

#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

/*!
 * \class MultimeterCommand
 *
 * The MultimeterCommand class implements the `meter` CLI command.
 */

/*!
 * Construct a new MultimeterCommand object with \a parent.
 */
MultimeterCommand::MultimeterCommand(QObject * const parent) : DeviceCommand(parent),
    service(nullptr), settings{
        MultimeterService::Mode::DcVoltage,
        { MultimeterService::VoltageRange::AutoRange },
        1000
    }, numberOfSamplesToRead(-1)
{

}

QStringList MultimeterCommand::requiredOptions() const
{
    return DeviceCommand::requiredOptions() + QStringList{
        QLatin1String("mode"),
    };
}

QStringList MultimeterCommand::supportedOptions() const
{
    return DeviceCommand::supportedOptions() + QStringList{
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
QStringList MultimeterCommand::processOptions(const QCommandLineParser &parser)
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
    } else if (mode.startsWith(QLatin1String("dc c")) || mode.startsWith(QLatin1String("aac"))) {
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
                settings.range.resitanceRange = MultimeterService::ResistanceRange::AutoRange;
            }
            unit = QLatin1String("ohms");
            sensibleMinimum = 0; // Unused.
            break;
        default:
            qCInfo(lc).noquote() << tr("Ignoring option: %1").arg(value);
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
        QLocale locale; bool ok;
        const int samples = locale.toInt(value, &ok);
        if (!ok) {
            errors.append(tr("Invalid number of samples: %1").arg(value));
        } else {
            numberOfSamplesToRead = samples;
        }
    }

    return errors;
}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a MultimeterService object.
 */
AbstractPokitService * MultimeterCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->multimeter();
        Q_ASSERT(service);
        connect(service, &MultimeterService::settingsWritten,
                this, &MultimeterCommand::settingsWritten);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override fetches the current device's status, and outputs it in the selected format.
 */
void MultimeterCommand::serviceDetailsDiscovered()
{
    QString range = tr("N/A");
    switch (settings.mode) {
    case MultimeterService::Mode::Idle:        break;
    case MultimeterService::Mode::DcVoltage:
    case MultimeterService::Mode::AcVoltage:
        range = MultimeterService::toString(settings.range.voltageRange);
        break;
    case MultimeterService::Mode::DcCurrent:
    case MultimeterService::Mode::AcCurrent:
        range = MultimeterService::toString(settings.range.currentRange);
        break;
    case MultimeterService::Mode::Resistance:
        range = MultimeterService::toString(settings.range.resitanceRange);
        break;
    case MultimeterService::Mode::Diode:       break;
    case MultimeterService::Mode::Continuity:  break;
    case MultimeterService::Mode::Temperature: break;
    }

    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    qCInfo(lc).noquote() << tr("Measuring %1, with range %2, every %L3ms.")
        .arg(MultimeterService::toString(settings.mode), range).arg(settings.updateInterval);
    service->setSettings(settings);
}

/*!
 * Returns the lowest \a mode range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
MultimeterService::Range MultimeterCommand::lowestRange(
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
        range.resitanceRange = lowestResistanceRange(desiredMax);
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
MultimeterService::CurrentRange MultimeterCommand::lowestCurrentRange(const quint32 desiredMax)
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
MultimeterService::ResistanceRange MultimeterCommand::lowestResistanceRange(const quint32 desiredMax)
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
MultimeterService::VoltageRange MultimeterCommand::lowestVoltageRange(const quint32 desiredMax)
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
void MultimeterCommand::settingsWritten()
{
    qCDebug(lc).noquote() << tr("Settings written; starting meter readings...");
    connect(service, &MultimeterService::readingRead,
            this, &MultimeterCommand::outputReading);
    service->beginClientReadings();
}

/*!
 * Outputs meter \a reading in the selected ouput format.
 */
void MultimeterCommand::outputReading(const MultimeterService::Reading &reading)
{
    QString status;
    if (reading.status.testFlag(MultimeterService::ReadingStatusFlag::Error)) {
        status = QLatin1String("Error");
    } else switch (reading.mode) {
    case MultimeterService::Mode::Idle:
        break;
    case MultimeterService::Mode::DcVoltage:
    case MultimeterService::Mode::AcVoltage:
    case MultimeterService::Mode::DcCurrent:
    case MultimeterService::Mode::AcCurrent:
    case MultimeterService::Mode::Resistance:
        status = (reading.status.testFlag(MultimeterService::ReadingStatusFlag::AutoRange)
            ? tr("Auto Range On") : tr("Auto Range Off"));
        break;
    case MultimeterService::Mode::Continuity:
        status = (reading.status.testFlag(MultimeterService::ReadingStatusFlag::AutoRange)
            ? tr("Continuity") : tr("No continuity"));
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
        range = MultimeterService::toString(reading.range.resitanceRange);
        rangeMin = MultimeterService::minValue(reading.range.resitanceRange);
        rangeMax = MultimeterService::maxValue(reading.range.resitanceRange);
        break;
    case MultimeterService::Mode::Diode:       break;
    case MultimeterService::Mode::Continuity:  break;
    case MultimeterService::Mode::Temperature: break;
    }

    switch (format) {
    case OutputFormat::Csv:
        for (static bool firstTime = true; firstTime; firstTime = false) {
            fputs(qPrintable(tr("mode,value,units,status,range_min_milli,range_max_milli\n")), stdout);
        }
        fputs(qPrintable(QString::fromLatin1("%1,%2,%3,%4,%5,%6\n")
            .arg(escapeCsvField(MultimeterService::toString(reading.mode)))
            .arg(reading.value, 0, 'f').arg(units, status, rangeMin.toString(), rangeMax.toString())
            ), stdout);
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
        fputs(QJsonDocument(jsonObject).toJson(), stdout);
    }   break;
    case OutputFormat::Text:
        fputs(qPrintable(tr("Mode:   %1 (0x%2)\n").arg(MultimeterService::toString(reading.mode))
            .arg((quint8)reading.mode,2,16,QLatin1Char('0'))), stdout);
        fputs(qPrintable(tr("Value:  %1 %2\n").arg(reading.value,0,'f').arg(units)), stdout);
        fputs(qPrintable(tr("Status: %1 (0x%2)\n").arg(status)
            .arg(reading.status,2,16,QLatin1Char('0'))), stdout);
        fputs(qPrintable(tr("Range:  %1 (0x%2)\n").arg(range)
            .arg((quint8)reading.range.voltageRange,2,16,QLatin1Char('0'))), stdout);
        break;
    }

    if (numberOfSamplesToRead > 0) {
        if (--numberOfSamplesToRead == 0) {
            service->stopClientReadings();
            QCoreApplication::exit(EXIT_SUCCESS);
        }
    }
}
