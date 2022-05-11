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

#include "dsocommand.h"

#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

/*!
 * \class DsoCommand
 *
 * The DsoCommand class implements the `dso` CLI command.
 */

/*!
 * Construct a new DsoCommand object with \a parent.
 */
DsoCommand::DsoCommand(QObject * const parent) : DeviceCommand(parent),
    service(nullptr), settings{
        DsoService::Command::FreeRunning, 0, DsoService::Mode::DcVoltage,
        { DsoService::VoltageRange::_30V_to_60V }, 1000000, 1000}
{

}

QStringList DsoCommand::requiredOptions(const QCommandLineParser &parser) const
{
    QStringList options = DeviceCommand::requiredOptions(parser) + QStringList{
        QLatin1String("mode"),
        QLatin1String("range"),
    };
    if ((parser.isSet(QLatin1String("trigger-level"))) ||
        (parser.isSet(QLatin1String("trigger-mode"))))
    {
        options.append({
            QLatin1String("trigger-level"),
            QLatin1String("trigger-mode"),
        });
    }
    return options;
}

QStringList DsoCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser) + QStringList{
        QLatin1String("interval"),
        QLatin1String("samples"),
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
    const QString mode = parser.value(QLatin1String("mode")).trimmed().toLower();
    if (mode.startsWith(QLatin1String("ac v")) || mode.startsWith(QLatin1String("vac"))) {
        settings.mode = DsoService::Mode::AcVoltage;
    } else if (mode.startsWith(QLatin1String("dc v")) || mode.startsWith(QLatin1String("vdc"))) {
        settings.mode = DsoService::Mode::DcVoltage;
    } else if (mode.startsWith(QLatin1String("ac c")) || mode.startsWith(QLatin1String("aac"))) {
        settings.mode = DsoService::Mode::AcCurrent;
    } else if (mode.startsWith(QLatin1String("dc c")) || mode.startsWith(QLatin1String("aac"))) {
        settings.mode = DsoService::Mode::DcCurrent;
    } else {
        errors.append(tr("Unknown DSO mode: %1").arg(parser.value(QLatin1String("mode"))));
        return errors;
    }

    // Parse the (required) range option.
    {
        const QString value = parser.value(QLatin1String("range"));
        QString unit; quint32 sensibleMinimum = 0;
        switch (settings.mode) {
        case DsoService::Mode::Idle:
            Q_ASSERT(false); // Not possible, since the mode parsing above never allows Idle.
            break;
        case DsoService::Mode::DcVoltage:
        case DsoService::Mode::AcVoltage:
            unit = QLatin1String("V");
            sensibleMinimum = 50; // mV.
            break;
        case DsoService::Mode::DcCurrent:
        case DsoService::Mode::AcCurrent:
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

    // Parse the trigger-level option.

    // Parse the trigger-mode option.

    // Parse the interval option.
//    if (parser.isSet(QLatin1String("interval"))) {
//        const QString value = parser.value(QLatin1String("interval"));
//        const quint32 interval = parseWholeValue(value, QLatin1String("s"));
//        if (interval == 0) {
//            errors.append(tr("Invalid interval value: %1").arg(value));
//        } else {
//            settings.updateInterval = interval;
//        }
//    }

    // Parse the samples option.
//    settings.timestamp = QDateTime::currentSecsSinceEpoch(); // Note, subject to Y2038 epochalypse.
//    if (parser.isSet(QLatin1String("timestamp"))) {
//        const QString value = parser.value(QLatin1String("timestamp"));
//        QLocale locale; bool ok;
//        static_assert(sizeof(uint) == sizeof(settings.timestamp));
//        const int timestamp = locale.toUInt(value, &ok);
//        if (!ok) {
//            errors.append(tr("Invalid timestamp value: %1").arg(value));
//        } else {
//            settings.timestamp = timestamp;
//        }
//    }
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
    const QString range = DsoService::toString(settings.range, settings.mode);
    qCInfo(lc).noquote() << tr("Sampling %1, with range %2, ...").arg(
        DsoService::toString(settings.mode), (range.isNull()) ? QString::fromLatin1("N/A") : range);
    service->setSettings(settings);
}

/*!
 * Returns the lowest \a mode range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
DsoService::Range DsoCommand::lowestRange(
    const DsoService::Mode mode, const quint32 desiredMax)
{
    DsoService::Range range{ DsoService::VoltageRange::_6V_to_12V };
    switch (mode) {
    case DsoService::Mode::Idle:
        qCWarning(lc).noquote() << tr("Idle has no defined ranges.");
        Q_ASSERT(false); // Should never have been called with this Idle mode.
        break;
    case DsoService::Mode::DcVoltage:
    case DsoService::Mode::AcVoltage:
        range.voltageRange = lowestVoltageRange(desiredMax);
        break;
    case DsoService::Mode::DcCurrent:
    case DsoService::Mode::AcCurrent:
        range.currentRange = lowestCurrentRange(desiredMax);
        break;
    default:
        qCWarning(lc).noquote() << tr("No defined ranges for mode %1.").arg((quint8)mode);
        Q_ASSERT(false); // Should never have been called with this invalid mode.
    }
    return range;
}

#define POKIT_APP_IF_LESS_THAN_RETURN(value, label) \
if (value <=  DsoService::maxValue(DsoService::label).toUInt()) { \
    return DsoService::label; \
}

/*!
 * Returns the lowest current range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
DsoService::CurrentRange DsoCommand::lowestCurrentRange(const quint32 desiredMax)
{
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_0_to_10mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_10mA_to_30mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_30mA_to_150mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_150mA_to_300mA)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, CurrentRange::_300mA_to_3A)
    return DsoService::CurrentRange::_300mA_to_3A; // Out of range, so go with the biggest.
}

/*!
 * Returns the lowest voltage range that can measure at least up to \a desired max, or AutoRange
 * if no such range is available.
 */
DsoService::VoltageRange DsoCommand::lowestVoltageRange(const quint32 desiredMax)
{
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_0_to_300mV)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_300mV_to_2V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_2V_to_6V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_6V_to_12V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_12V_to_30V)
    POKIT_APP_IF_LESS_THAN_RETURN(desiredMax, VoltageRange::_30V_to_60V)
    return DsoService::VoltageRange::_30V_to_60V; // Out of range, so go with the biggest.
}

#undef POKIT_APP_IF_LESS_THAN_RETURN

/*!
 * Invoked when the DSO settings have been written.
 */
void DsoCommand::settingsWritten()
{
    qCDebug(lc).noquote() << tr("Settings written; DSO has started.");
    /// \todo Output the settings used as CSV, JSON and Text.
    connect(service, &DsoService::metadataRead, this, &DsoCommand::metadataRead);
    connect(service, &DsoService::samplesRead, this, &DsoCommand::outputSamples);
//    service->beginMetadata();
//    service->beginSampling();
}

/*!
 * Invoked when \a metadata has been received from the DSO.
 *
 * \todo Presumably we need to at least record the metadata.scale here. Probably also need the
 * metadata.numberOfSamples. Of course, log and check for errors etc. too.
 */
void DsoCommand::metadataRead(const DsoService::Metadata &metadata)
{
    /// \todo Remember the metadata.
    Q_UNUSED(metadata);
    qCDebug(lc) << "status:" << (int)(metadata.status);
    qCDebug(lc) << "scale:" << metadata.scale;
    qCDebug(lc) << "mode:" << DsoService::toString(metadata.mode);
    qCDebug(lc) << "range:" << DsoService::toString(metadata.range.voltageRange);
    qCDebug(lc) << "samplingWindow:" << (int)metadata.samplingWindow;
    qCDebug(lc) << "numberOfSamples:" << metadata.numberOfSamples;
    qCDebug(lc) << "samplingRate:" << metadata.samplingRate;
}

/*!
 * Outputs DSO \a samples in the selected ouput format.
 */
void DsoCommand::outputSamples(const DsoService::Samples &samples)
{
    /// \todo Output the samples.
    Q_UNUSED(samples);
    qCDebug(lc) << "samplesRead";
}
