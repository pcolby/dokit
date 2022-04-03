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

#include "scanner.h"
#include "utils.h"
#include "uuids.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLowEnergyController>

#include "pokitdevicedisoveryagent.h"

Scanner::Scanner(QObject * const parent) : QObject(parent)
{
    discoveryAgent = new PokitDeviceDiscoveryAgent(this);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::pokitDeviceDiscovered,
            this, &Scanner::deviceDiscovered);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::pokitDeviceUpdated, this, [this](const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields) {
        qDebug() << "devid eupdatyed";
        QLowEnergyController * c = QLowEnergyController::createCentral(info);

        connect(c, &QLowEnergyController::connected, this, [c]() {
            qDebug() << "device connected";
            c->discoverServices();
        });

        fputs(QJsonDocument(toJsonObject(info)).toJson(), stdout);
    });

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::finished, this, []() {
        qDebug() << "device discovery finished";
        QCoreApplication::quit();
    });

    discoveryAgent->setLowEnergyDiscoveryTimeout(3000); ///< \todo Set this from CLI.
    discoveryAgent->start();
}

void Scanner::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    QLowEnergyController * c = QLowEnergyController::createCentral(info);
    qDebug() << c;
    qDebug() << c->services().size();

    connect(c, &QLowEnergyController::connected, this, [c]() {
        qDebug() << "device connected";
        c->discoverServices();
    });

    fputs(QJsonDocument(toJsonObject(info)).toJson(), stdout);

//    connect(c, &QLowEnergyController::serviceDiscovered, this, [c](const QBluetoothUuid &service) {
//        qDebug() << "service discovered" << service;
//        if (service == QBluetoothUuid(QStringLiteral(POKIT_SERVICE_STATUS))) {
//            qDebug() << "status";
//            QLowEnergyService * s = c->createServiceObject(service);
//            qDebug() << s;
//            if (s != nullptr) {
//                s->discoverDetails();
//            }
//        }
//    });

//    connect(c, &QLowEnergyController::discoveryFinished, this, [c]() {
//        qDebug() << "service discovery finished" << c->services().size();
//        QCoreApplication::quit();
//    });

//    c->connectToDevice();
}
