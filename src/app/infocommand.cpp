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
InfoCommand::InfoCommand(QObject * const parent) : AbstractCommand(parent)
{
    PokitDevice * device = new PokitDevice(QLatin1String("5C:02:72:09:AA:25"), this);
    qDebug() << device;
    StatusService * service = device->status();
    qDebug() << service;

    connect(device->controller(),
        #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
        QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
        #else
        &QLowEnergyController::errorOccurred,
        #endif
    [](QLowEnergyController::Error error) {
        qDebug() << error;
        ::exit(EXIT_FAILURE); ///< \todo Exit gracefully.
    });

    connect(service, &StatusService::serviceDetailsDiscovered,[service]() {
        qDebug() << "All ready to go! :)";
        service->deviceCharacteristics(); // Should be immediately (cached) available.
        service->flashLed(); // Should be fun :)
        //service->setDeviceName(QLatin1String("PokitPro")); // Yes, this works :D
        service->readCharacteristics(); // Can also fetch new values (results via signal).
    });

    device->controller()->connectToDevice();
}

QStringList InfoCommand::requiredOptions() const
{
    return QStringList{
        QLatin1String("device"),
    };
}

QStringList InfoCommand::supportedOptions() const
{
    return AbstractCommand::supportedOptions();
}
