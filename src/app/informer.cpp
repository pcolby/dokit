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

#include <qtpokit/pokitdevice.h>
#include "qtpokit/statusservice.h"
#include <qtpokit/utils.h>
#include <qtpokit/uuids.h>

#include <QCoreApplication>
#include <QDataStream>
#include <QLoggingCategory>
#include <QLowEnergyController>
#include <QtEndian>

Q_LOGGING_CATEGORY(pokitInformer, "pokit.ui.informer", QtInfoMsg);

/// \todo Probably implement a sahred (library) PokitDevice class that wraps a QLowEnergyController
/// to implement all of the Pokit services?  Kinda depends on how Qt's BLE services classes work, so
/// we'll continue with this bespoke class to learn more for now...

Informer::Informer(QObject * const parent) : QObject(parent), controller(nullptr)
{
    PokitDevice * device = new PokitDevice(QLatin1String("5C:02:72:09:AA:25"), this);
    qDebug() << device;
    StatusService * service = device->status();
    qDebug() << service;

    connect(device->controller(), QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
            [](QLowEnergyController::Error error) {
        qDebug() << error;
        ::exit(EXIT_FAILURE); ///< \todo Exit gracefully.
    });

    device->controller()->connectToDevice();

//    QBluetoothDeviceInfo info(QBluetoothAddress(QLatin1String("5C:02:72:09:AA:25")), QString(), 0);
//    qDebug() << toJson(info);
//    controller = QLowEnergyController::createCentral(info);

//    connect(controller, &QLowEnergyController::serviceDiscovered, this, [this](const QBluetoothUuid &service) {
//        qDebug() << "service discovered" << service;
//        if (service == QBluetoothUuid(QStringLiteral(POKIT_SERVICE_STATUS))) {
//            qDebug() << "status";
//            QLowEnergyService * s = controller->createServiceObject(service);
//            qDebug() << s;
//            if (s != nullptr) {
//                s->discoverDetails();
//            }
//        }
//    });

//    QLowEnergyService * s = controller->createServiceObject(QBluetoothUuid(QLatin1String(POKIT_SERVICE_STATUS)), this);
//    qDebug() << "first service" << s;

//    connect(controller, &QLowEnergyController::connected, this, [this]() {
//        qDebug() << "connected" << controller->services().size();
//        controller->discoverServices();
//    });

//    connect(controller, &QLowEnergyController::disconnected, this, [this]() {
//        qDebug() << "disconnected" << controller->services().size();
//    });

//    connect(controller, &QLowEnergyController::discoveryFinished, this, [this]() {
//        qDebug() << "service discovery finished" << controller->services().size();
//        QLowEnergyService * s = controller->createServiceObject(QBluetoothUuid(QLatin1String(POKIT_SERVICE_STATUS)), this);
//        qDebug() << "second service" << s;
//        if (s) {
//            qDebug() << "second service" << s->serviceName() << s->serviceUuid();
//            #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
//            connect(s, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error),
//            #else
//            connect(s, &QLowEnergyService::errorOccurred,
//            #endif
//                    this, [](const QLowEnergyService::ServiceError error) {
//                qCritical() << "ServcieError:" << error;
//                QCoreApplication::quit();
//            });

//            connect(s, &QLowEnergyService::stateChanged, this, [s](const QLowEnergyService::ServiceState state) {
//                qDebug() << "s1 state chnaged" << state;
//                #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
//                if (state == QLowEnergyService::ServiceDiscovered) {
//                #else
//                if (state == QLowEnergyService::RemoteServiceDiscovered) {
//                #endif
//                    QLowEnergyCharacteristic c = s->characteristic(QUuid(POKIT_CHARACTERISTIC_STATUS_DEVICE));
//                    qDebug() << c.name() << c.value().length() << c.value() << c.isValid();
//                    if (c.isValid()) {
//                        Q_ASSERT(c.value().length() == 20); /// \todo Proper error handling.

//                        // Both of the following methods produce the same, correct results. Will
//                        // settle on one when refactoring into library classes.

//                        // The bytearray-indexing method.
//                        qDebug() << "version major" << qFromLittleEndian<quint8>(c.value().mid(0,1));
//                        qDebug() << "version minor" << qFromLittleEndian<quint8>(c.value().mid(1,1));
//                        qDebug() << "max voltage" << qFromLittleEndian<quint16>(c.value().mid(2,2));
//                        qDebug() << "max current" << qFromLittleEndian<quint16>(c.value().mid(4,2));
//                        qDebug() << "max resistance" << qFromLittleEndian<quint16>(c.value().mid(6,2));
//                        qDebug() << "max sample rate" << qFromLittleEndian<quint16>(c.value().mid(8,2));
//                        qDebug() << "max buff size" << qFromLittleEndian<quint16>(c.value().mid(10,2));
//                        qDebug() << "max cap mask" << qFromLittleEndian<quint16>(c.value().mid(12,2));
//                        qDebug() << "mac addr" << c.value().mid(14);

//                        // The datastream method.
//                        QDataStream str(c.value());
//                        str.setByteOrder(QDataStream::LittleEndian);
//                        quint8 v1,v2;
//                        quint16 maxV, maxC, maxR, maxS, buff, mask;
//                        str >> v1 >> v2 >> maxV >> maxC >> maxR >> maxS >> buff >> mask;
//                        qDebug() << "version major" << v1;
//                        qDebug() << "version minor" << v2;
//                        qDebug() << "max voltage" << maxV;
//                        qDebug() << "max current" << maxC;
//                        qDebug() << "max resistance" << maxR;
//                        qDebug() << "max sample rate" << maxS;
//                        qDebug() << "max buff size" << buff;
//                        qDebug() << "max cap mask" << mask;
//                        //qDebug() << "mac addr" << str;
//                    }
//                }
//            });
//            s->discoverDetails();
//        }

//        QLowEnergyService * s2 = controller->createServiceObject(POKIT_SERVICE_DEVICE_INFO, this);
//        qDebug() << "second service s2" << s2;
//        if (s2) {
//            qDebug() << "second service s2" << s2->serviceName() << s2->serviceUuid();
//            connect(s2, &QLowEnergyService::stateChanged, this, [s2](const QLowEnergyService::ServiceState state) {
//                qDebug() << "s2 state chnaged" << state;
//                #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
//                if (state == QLowEnergyService::ServiceDiscovered) {
//                #else
//                if (state == QLowEnergyService::RemoteServiceDiscovered) {
//                #endif
//                    QLowEnergyCharacteristic c = s2->characteristic(POKIT_CHARACTERISTIC_DEVICE_MANUFACTURER);
//                    qDebug() << c.name() << c.value() << c.isValid();
//                    c = s2->characteristic(POKIT_CHARACTERISTIC_DEVICE_MODEL_NUMBER);
//                    qDebug() << c.name() << c.value() << c.isValid();
//                    c = s2->characteristic(POKIT_CHARACTERISTIC_DEVICE_FIRMWARE_REV);
//                    qDebug() << c.name() << c.value() << c.isValid();
//                    c = s2->characteristic(POKIT_CHARACTERISTIC_DEVICE_SOFTWARE_REV);
//                    qDebug() << c.name() << c.value() << c.isValid();
//                    c = s2->characteristic(POKIT_CHARACTERISTIC_DEVICE_HARDWARE_REV);
//                    qDebug() << c.name() << c.value() << c.isValid();
//                }
//            });
//            s2->discoverDetails();
//        }
//    });

//    #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
//    connect(controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
//    #else
//    connect(controller, &QLowEnergyController::errorOccurred,
//    #endif
//            this, [](const QLowEnergyController::Error error) {
//        qCritical() << "Error:" << error;
//        QCoreApplication::quit();
//    });

//    qDebug() << "connecting";
//    controller->connectToDevice();
}
