/*
    Copyright 2022 Paul Colby

    This file is part of Poqit.

    Poqit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Poqit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Poqit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "discover.h"

#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QCoreApplication>
#include <QLowEnergyController>
#include <QTimer>

Discover::Discover(QObject * const parent) : QObject(parent)
{
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    discoveryAgent->setLowEnergyDiscoveryTimeout(3000);

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, [](const QBluetoothDeviceInfo &info) {
        if (info.serviceUuids().contains(QBluetoothUuid(QStringLiteral("57d3a771-267c-4394-8872-78223e92aec5"))))
        {
            qDebug() << "discovered" << info.address() << info.name() << info.rssi()
                     << info.majorDeviceClass() << info.serviceClasses()
                     << info.serviceUuids() << info.manufacturerData() << info.manufacturerIds();

            QLowEnergyController * c = QLowEnergyController::createCentral(info);
            qDebug() << c;
        }
    });

//    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceUpdated, this, [](const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields) {
//        if (info.name() == QLatin1String("PokitPro"))
//        qDebug() << "updated" << info.address() << info.name() << info.rssi()
//                 << info.majorDeviceClass() << info.serviceClasses()
//                 << info.serviceUuids() << info.manufacturerData() << info.manufacturerIds();
//    });

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, []() {
        qDebug() << "discovery finished";
        QCoreApplication::quit();
    });

//    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
//    this, SLOT(addDevice(QBluetoothDeviceInfo)));
//    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));

//    connect(ui->list, SIGNAL(itemActivated(QListWidgetItem*)),
//    this, SLOT(itemActivated(QListWidgetItem*)));

    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}
