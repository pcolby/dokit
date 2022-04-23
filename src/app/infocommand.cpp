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

#include "infocommand.h"

#include <qtpokit/deviceinfoservice.h>
#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

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

QStringList InfoCommand::requiredOptions() const
{
    return DeviceCommand::requiredOptions() + QStringList{
    };
}

QStringList InfoCommand::supportedOptions() const
{
    return DeviceCommand::supportedOptions();
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
 * Begins scanning for Pokit devices.
 */
bool InfoCommand::start()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->deviceInformation();
        Q_ASSERT(service);
        connect(service, &DeviceInfoService::serviceDetailsDiscovered,
                this, &InfoCommand::serviceDetailsDiscovered);
        connect(service, &DeviceInfoService::serviceErrorOccurred,
                this, &InfoCommand::serviceError);
    }
    qCInfo(lc).noquote() << tr("Connecting to device...");
    device->controller()->connectToDevice();
    return true;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override fetches the current device's information, and outputs it in the selected format.
 */
void InfoCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    switch (format) {
    case OutputFormat::Csv:
        fputs(qPrintable(tr("manufacturer_name,model_number,hardware_revision,firmware_revision,"
                            "software_revision\n")), stdout);
        fputs(qPrintable(QString::fromLatin1("%1,%2,%3,%4,%5\n").arg(
            escapeCsvField(service->manufacturer()), escapeCsvField(service->modelNumber()),
            escapeCsvField(service->hardwareRevision()), escapeCsvField(service->firmwareRevision()),
            escapeCsvField(service->softwareRevision()))), stdout);
        break;
    case OutputFormat::Json:
        fputs(QJsonDocument(QJsonObject{
            { QLatin1String("manufacturerName"), service->manufacturer() },
            { QLatin1String("modelNumber"),      service->modelNumber() },
            { QLatin1String("hardwareRevision"), service->hardwareRevision() },
            { QLatin1String("firmwareRevision"), service->firmwareRevision() },
            { QLatin1String("softwareRevision"), service->softwareRevision() },
            }).toJson(), stdout);
        break;
    case OutputFormat::Text:
        fputs(qPrintable(tr("Manufacturer name: %1\n").arg(service->manufacturer())), stdout);
        fputs(qPrintable(tr("Model number:      %1\n").arg(service->modelNumber())), stdout);
        fputs(qPrintable(tr("Hardware revision: %1\n").arg(service->hardwareRevision())), stdout);
        fputs(qPrintable(tr("Firmware revision: %1\n").arg(service->firmwareRevision())), stdout);
        fputs(qPrintable(tr("Software revision: %1\n").arg(service->softwareRevision())), stdout);
        break;
    }
    QCoreApplication::quit(); // We're all done :)
}
