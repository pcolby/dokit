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

#include <QCoreApplication>
#include <QLowEnergyController>

Discover::Discover(QObject * const parent) : QObject(parent)
{
    discoveryAgent = new PokitDeviceDiscoveryAgent(this);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::pokitDeviceDiscovered, this, [](const QBluetoothDeviceInfo &info) {
        QLowEnergyController * c = QLowEnergyController::createCentral(info);
        qDebug() << c;
    });

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::finished, this, []() {
        qDebug() << "discovery finished";
        QCoreApplication::quit();
    });

    discoveryAgent->setLowEnergyDiscoveryTimeout(3000);
    discoveryAgent->start();
}
