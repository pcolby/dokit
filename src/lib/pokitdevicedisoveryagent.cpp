/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pokitdevicedisoveryagent.h"

#include "logging.h"
#include "uuids.h"

#include <QBluetoothUuid>
#include <QDebug>

PokitDeviceDiscoveryAgent::PokitDeviceDiscoveryAgent(
    const QBluetoothAddress &deviceAdapter, QObject *parent)
    : QBluetoothDeviceDiscoveryAgent(deviceAdapter, parent)
{
    connect(this, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &PokitDeviceDiscoveryAgent::onDeviceDiscovered);

    connect(this, &QBluetoothDeviceDiscoveryAgent::deviceUpdated,
            this, &PokitDeviceDiscoveryAgent::onDeviceUpdated);
}

PokitDeviceDiscoveryAgent::PokitDeviceDiscoveryAgent(QObject * parent)
    : QBluetoothDeviceDiscoveryAgent(parent)
{
    connect(this, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &PokitDeviceDiscoveryAgent::onDeviceDiscovered);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Signal added in Qt 5.12.
    connect(this, &QBluetoothDeviceDiscoveryAgent::deviceUpdated,
            this, &PokitDeviceDiscoveryAgent::onDeviceUpdated);
    #endif
}

bool PokitDeviceDiscoveryAgent::isPokitDevice(const QBluetoothDeviceInfo &info)
{
    return info.serviceUuids().contains(QBluetoothUuid(QStringLiteral(POKIT_SERVICE_STATUS)));
}

void PokitDeviceDiscoveryAgent::start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethods methods)
{
    Q_ASSERT(methods == QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    QBluetoothDeviceDiscoveryAgent::start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void PokitDeviceDiscoveryAgent::start()
{
    QBluetoothDeviceDiscoveryAgent::start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void PokitDeviceDiscoveryAgent::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    if (!isPokitDevice(info)) return;
    qCDebug(pokitDiscovery) << "Discovered Pokit device" << info.name() << info.address();
    emit pokitDeviceDiscovered(info);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
void PokitDeviceDiscoveryAgent::onDeviceUpdated(const QBluetoothDeviceInfo &info,
                                                QBluetoothDeviceInfo::Fields updatedFields)
{
    if (!isPokitDevice(info)) return;
    qCDebug(pokitDiscovery) << "Updated Pokit device" << info.name() << info.address() << info.rssi();
    emit pokitDeviceUpdated(info, updatedFields);
}
#endif
