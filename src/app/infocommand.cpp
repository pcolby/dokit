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

#include <qtpokit/pokitdevice.h>
#include "qtpokit/statusservice.h"
#include <qtpokit/utils.h>
#include <qtpokit/uuids.h>

#include <QCoreApplication>
#include <QDataStream>
#include <QLowEnergyController>
#include <QtEndian>

/*!
 * Construct a new InfoCommand object with \a parent.
 */
InfoCommand::InfoCommand(QObject * const parent) : DeviceCommand(parent)
{

}

QStringList InfoCommand::requiredOptions() const
{
    return DeviceCommand::requiredOptions() + QStringList{
        QLatin1String("device"),
    };
}

QStringList InfoCommand::supportedOptions() const
{
    return DeviceCommand::supportedOptions();
}

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
    Q_ASSERT(device != nullptr);
    StatusService * service = device->status();
    Q_ASSERT(service != nullptr);
    connect(service, &StatusService::serviceDetailsDiscovered,
            this, &InfoCommand::serviceDetailsDiscovered);

    /// \todo Cleanup, and move to InfoCommand::serviceDetailsDiscovered.
    connect(service, &StatusService::serviceDetailsDiscovered,[service]() {
        service->deviceCharacteristics(); // Should be immediately (cached) available.
        service->flashLed(); // Should be fun :)
        //service->setDeviceName(QLatin1String("PokitPro")); // Yes, this works :D
        service->readCharacteristics(); // Can also fetch new values (results via signal).
    });

    qCDebug(lc).noquote() << tr("Connecting to device...");
    device->controller()->connectToDevice();
    return true;
}
