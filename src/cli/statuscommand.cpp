// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "statuscommand.h"
#include "../stringliterals_p.h"

#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

DOKIT_USE_STRINGLITERALS

/*!
 * \class StatusCommand
 *
 * The StatusCommand class implements the `status` CLI command.
 */

/*!
 * Construct a new StatusCommand object with \a parent.
 */
StatusCommand::StatusCommand(QObject * const parent) : DeviceCommand(parent)
{

}

QStringList StatusCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
    };
}

QStringList StatusCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser);
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList StatusCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = DeviceCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    return errors;
}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a StatusService object.
 */
AbstractPokitService * StatusCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->status();
        Q_ASSERT(service);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override fetches the current device's status, and outputs it in the selected format.
 */
void StatusCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    const StatusService::DeviceCharacteristics chrs = service->deviceCharacteristics();
    if (chrs.firmwareVersion.isNull()) {
        qCWarning(lc).noquote() << tr("Failed to parse device information");
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    }
    outputDeviceStatus(chrs);
}

/*!
 * Outputs the Pokit device's details, including \a chrs, in the selected format.
 */
void StatusCommand::outputDeviceStatus(const StatusService::DeviceCharacteristics &chrs)
{
    const QString deviceName = service->deviceName();
    const StatusService::Status status = service->status();
    const std::optional<StatusService::TorchStatus> torchStatus = service->torchStatus();
    const std::optional<StatusService::ButtonStatus> buttonStatus = service->buttonPress();
    const QString statusLabel = StatusService::toString(status.deviceStatus);
    const QString batteryLabel = StatusService::toString(status.batteryStatus);
    const QString switchLabel = status.switchPosition ? StatusService::toString(*status.switchPosition) : QString();
    const QString chargingLabel = status.chargingStatus ? StatusService::toString(*status.chargingStatus) : QString();
    const QString torchLabel = (torchStatus) ? StatusService::toString(*torchStatus) : QString();
    const QString buttonLabel = (buttonStatus) ? StatusService::toString(*buttonStatus) : QString();

    switch (format) {
    case OutputFormat::Csv:
        std::cout << qUtf8Printable(tr("device_name,device_status,firmware_version,maximum_voltage,"
                            "maximum_current,maximum_resistance,maximum_sampling_rate,"
                            "sampling_buffer_size,capability_mask,mac_address,battery_voltage,"
                            "battery_status,torch_status,button_status,switch_position,charging_status\n"));
        std::cout << qUtf8Printable(QString::fromLatin1("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16\n")
            .arg(escapeCsvField(deviceName),statusLabel.toLower(),chrs.firmwareVersion.toString())
            .arg(chrs.maximumVoltage).arg(chrs.maximumCurrent).arg(chrs.maximumResistance)
            .arg(chrs.maximumSamplingRate).arg(chrs.samplingBufferSize).arg(chrs.capabilityMask)
            .arg(chrs.macAddress.toString()).arg(status.batteryVoltage)
            .arg(batteryLabel.toLower(), torchLabel.toLower(), buttonLabel.toLower(), switchLabel.toLower(),
                 chargingLabel.toLower()));
        break;
    case OutputFormat::Json: {
        QJsonObject battery{
            { u"level"_s,  status.batteryVoltage },
        };
        if (!batteryLabel.isNull()) {
            battery.insert(u"status"_s, batteryLabel);
        }
        QJsonObject object{
                { u"deviceName"_s,   deviceName },
                { u"firmwareVersion"_s, QJsonObject{
                      { u"major"_s, chrs.firmwareVersion.majorVersion() },
                      { u"minor"_s, chrs.firmwareVersion.minorVersion() },
                }},
                { u"maximumVoltage"_s,      chrs.maximumVoltage },
                { u"maximumCurrent"_s,      chrs.maximumCurrent },
                { u"maximumResistance"_s,   chrs.maximumResistance },
                { u"maximumSamplingRate"_s, chrs.maximumSamplingRate },
                { u"samplingBufferSize"_s,  chrs.samplingBufferSize },
                { u"capabilityMask"_s,      chrs.capabilityMask },
                { u"macAddress"_s,          chrs.macAddress.toString() },
                { u"deviceStatus"_s, QJsonObject{
                      { u"code"_s, (quint8)status.deviceStatus },
                      { u"label"_s, statusLabel },
                }},
                { u"battery"_s, battery },
            };
        if (torchStatus) {
            object.insert(u"torchStatus"_s, QJsonObject{
                { u"code"_s, (quint8)*torchStatus },
                { u"label"_s, torchLabel },
            });
        }
        if (buttonStatus) {
            object.insert(u"buttonStatus"_s, QJsonObject{
                { u"code"_s, (quint8)*buttonStatus },
                { u"label"_s, buttonLabel },
            });
        }
        if (status.switchPosition) {
            object.insert(u"switchStatus"_s, QJsonObject{
                { u"code"_s, (quint8)*status.switchPosition },
                { u"label"_s, switchLabel },
            });
        }
        if (status.chargingStatus) {
            object.insert(u"chargingStatus"_s, QJsonObject{
                { u"code"_s, (quint8)*status.chargingStatus },
                { u"label"_s, chargingLabel },
            });
        }
        std::cout << QJsonDocument(object).toJson().toStdString();
    }   break;
    case OutputFormat::Text:
        std::cout << qUtf8Printable(tr("Device name:           %1\n").arg(deviceName));
        std::cout << qUtf8Printable(tr("Firmware version:      %1\n").arg(chrs.firmwareVersion.toString()));
        std::cout << qUtf8Printable(tr("Maximum voltage:       %1\n").arg(chrs.maximumVoltage));
        std::cout << qUtf8Printable(tr("Maximum current:       %1\n").arg(chrs.maximumCurrent));
        std::cout << qUtf8Printable(tr("Maximum resistance:    %1\n").arg(chrs.maximumResistance));
        std::cout << qUtf8Printable(tr("Maximum sampling rate: %1\n").arg(chrs.maximumSamplingRate));
        std::cout << qUtf8Printable(tr("Sampling buffer size:  %1\n").arg(chrs.samplingBufferSize));
        std::cout << qUtf8Printable(tr("Capability mask:       %1\n").arg(chrs.capabilityMask));
        std::cout << qUtf8Printable(tr("MAC address:           %1\n").arg(chrs.macAddress.toString()));
        std::cout << qUtf8Printable(tr("Device status:         %1 (%2)\n").arg(statusLabel)
            .arg((quint8)status.deviceStatus));
        std::cout << qUtf8Printable(tr("Battery voltage:       %1\n").arg(status.batteryVoltage));
        std::cout << qUtf8Printable(tr("Battery status:        %1 (%2)\n")
            .arg(batteryLabel.isNull() ? QString::fromLatin1("N/A") : batteryLabel)
            .arg((quint8)status.batteryStatus));
        if (status.switchPosition) {
            std::cout << qUtf8Printable(tr("Switch position:       %1 (%2)\n")
                .arg(switchLabel).arg((quint8)*status.switchPosition));
        }
        if (status.chargingStatus) {
            std::cout << qUtf8Printable(tr("Charging status:       %1 (%2)\n")
                .arg(chargingLabel).arg((quint8)*status.chargingStatus));
        }
        break;
    }
    if (device) disconnect(); // Will exit the application once disconnected.
}
