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

#include "informer.h"
#include "utils.h"
#include "uuids.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QLowEnergyController>

#include "pokitdevicedisoveryagent.h"

Q_LOGGING_CATEGORY(pokitInformer, "pokit.ui.informer", QtInfoMsg);

/// \todo Probably implement a sahred (library) PokitDevice class that wraps a QLowEnergyController
/// to implement all of the Pokit services?  Kinda depends on how Qt's BLE services classes work, so
/// we'll continue with this bespoke class to learn more for now...

Informer::Informer(QObject * const parent) : QObject(parent), controller(nullptr)
{
    QBluetoothDeviceInfo info(QBluetoothAddress(QLatin1String("...")), QString(), 0);
    qDebug() << toJsonObject(info);
    controller = QLowEnergyController::createCentral(info);

    connect(controller, &QLowEnergyController::serviceDiscovered, this, [this](const QBluetoothUuid &service) {
        qDebug() << "service discovered" << service;
        if (service == QBluetoothUuid(QStringLiteral(POKIT_SERVICE_STATUS))) {
            qDebug() << "status";
            QLowEnergyService * s = controller->createServiceObject(service);
            qDebug() << s;
            if (s != nullptr) {
                s->discoverDetails();
            }
        }
    });

    connect(controller, &QLowEnergyController::connected, this, [this]() {
        qDebug() << "connected" << controller->services().size();
        controller->discoverServices();
    });

    connect(controller, &QLowEnergyController::disconnected, this, [this]() {
        qDebug() << "disconnected" << controller->services().size();
    });

    connect(controller, &QLowEnergyController::discoveryFinished, this, [this]() {
        qDebug() << "service discovery finished" << controller->services().size();
    });

    connect(controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
            this, [](const QLowEnergyController::Error error) {
        qCritical() << "Error:" << error;
        QCoreApplication::quit();
    });

    qDebug() << "connecting";
    controller->connectToDevice();
}

//void Scanner::deviceDiscovered(const QBluetoothDeviceInfo &info)
//{

//    connect(c, &QLowEnergyController::connected, this, [c]() {
//        qCDebug(pokitScanner) << "device connected";
//        c->discoverServices();
//    });

//    fputs(QJsonDocument(toJsonObject(info)).toJson(), stdout);

//}
