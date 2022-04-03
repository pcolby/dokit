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
#include <QJsonDocument>
#include <QLowEnergyController> ///< \todo Remove this when related experimental code is removed.

#include "pokitdevicedisoveryagent.h"

Q_LOGGING_CATEGORY(pokitScanner, "pokit.ui.scanner", QtInfoMsg);

Scanner::Scanner(QObject * const parent) : QObject(parent)
{
    discoveryAgent = new PokitDeviceDiscoveryAgent(this);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::pokitDeviceDiscovered,
            this, &Scanner::deviceDiscovered);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::pokitDeviceUpdated, this, [this](const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields) {
        qCDebug(pokitScanner) << "devid eupdatyed";
        QLowEnergyController * c = QLowEnergyController::createCentral(info);

        connect(c, &QLowEnergyController::connected, this, [c]() {
            qCDebug(pokitScanner) << "device connected";
            c->discoverServices();
        });

        fputs(QJsonDocument(toJsonObject(info)).toJson(), stdout);
    });

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::finished, this, []() {
        qCDebug(pokitScanner) << "Finished scanning for Pokit devices.";
        QCoreApplication::quit();
    });
}

void Scanner::start(const int timeout)
{
    Q_ASSERT(discoveryAgent);
    qCDebug(pokitScanner).noquote().nospace() << "Scanning for Pokit devices (with "
        << (timeout ? QLocale().toString(timeout)+QLatin1String("ms") : QLatin1String("no"))
        << " timeout).";
    discoveryAgent->setLowEnergyDiscoveryTimeout(timeout);
    discoveryAgent->start();
}

void Scanner::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    QLowEnergyController * c = QLowEnergyController::createCentral(info);

    connect(c, &QLowEnergyController::connected, this, [c]() {
        qCDebug(pokitScanner) << "device connected";
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
