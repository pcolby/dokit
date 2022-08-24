// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "infocommand.h"

#include <qtpokit/deviceinfoservice.h>
#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

/*!
 * \class InfoCommand
 *
 * The InfoCommand class implements the `info` CLI command.
 */

/*!
 * Construct a new InfoCommand object with \a parent.
 */
InfoCommand::InfoCommand(QObject * const parent) : DeviceCommand(parent), service(nullptr)
{

}

QStringList InfoCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
    };
}

QStringList InfoCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser);
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList InfoCommand::processOptions(const QCommandLineParser &parser)
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
 * This override returns a pointer to a DeviceInfoService object.
 */
AbstractPokitService * InfoCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->deviceInformation();
        Q_ASSERT(service);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override fetches the current device's information, and outputs it in the selected format.
 */
void InfoCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    const QString deviceName = device->controller()->remoteName();
    const QBluetoothAddress deviceAddress = device->controller()->remoteAddress();
    const QBluetoothUuid deviceUuid = device->controller()->remoteDeviceUuid();
    switch (format) {
    case OutputFormat::Csv:
        std::cout << qUtf8Printable(tr("device_name,device_address,device_uuid,manufacturer_name,model_number,"
                            "hardware_revision,firmware_revision,software_revision\n"));
        std::cout << qUtf8Printable(QString::fromLatin1("%1,%2,%3,%4,%5,%6,%7,%8\n").arg(
            escapeCsvField(deviceName),
            (deviceAddress.isNull()) ? QString() : deviceAddress.toString(),
            (deviceUuid.isNull()) ? QString() : deviceUuid.toString(),
            escapeCsvField(service->manufacturer()), escapeCsvField(service->modelNumber()),
            escapeCsvField(service->hardwareRevision()), escapeCsvField(service->firmwareRevision()),
            escapeCsvField(service->softwareRevision())));
        break;
    case OutputFormat::Json: {
        QJsonObject jsonObject{
            { QLatin1String("manufacturerName"), service->manufacturer() },
            { QLatin1String("modelNumber"),      service->modelNumber() },
            { QLatin1String("hardwareRevision"), service->hardwareRevision() },
            { QLatin1String("firmwareRevision"), service->firmwareRevision() },
            { QLatin1String("softwareRevision"), service->softwareRevision() },
        };
        if (!deviceName.isEmpty()) {
            jsonObject.insert(QLatin1String("deviceName"), deviceName);
        }
        if (!deviceAddress.isNull()) {
            jsonObject.insert(QLatin1String("deviceAddress"), deviceAddress.toString());
        }
        if (!deviceUuid.isNull()) {
            jsonObject.insert(QLatin1String("deviceUuid"), deviceUuid.toString());
        }
        std::cout << QJsonDocument(jsonObject).toJson().toStdString();
    }   break;
    case OutputFormat::Text:
        if (!deviceName.isEmpty()) {
            std::cout << qUtf8Printable(tr("Device name:       %1\n").arg(deviceName));
        }
        if (!deviceAddress.isNull()) {
            std::cout << qUtf8Printable(tr("Device addres:     %1\n").arg(deviceAddress.toString()));
        }
        if (!deviceUuid.isNull()) {
            std::cout << qUtf8Printable(tr("Device UUID:       %1\n").arg(deviceUuid.toString()));
        }
        std::cout << qUtf8Printable(tr("Manufacturer name: %1\n").arg(service->manufacturer()));
        std::cout << qUtf8Printable(tr("Model number:      %1\n").arg(service->modelNumber()));
        std::cout << qUtf8Printable(tr("Hardware revision: %1\n").arg(service->hardwareRevision()));
        std::cout << qUtf8Printable(tr("Firmware revision: %1\n").arg(service->firmwareRevision()));
        std::cout << qUtf8Printable(tr("Software revision: %1\n").arg(service->softwareRevision()));
        break;
    }
    if (device) disconnect(); // Will exit the application once disconnected.
}
