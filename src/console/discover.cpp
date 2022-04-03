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

#include "discover.h"
#include "uuids.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLowEnergyController>

QJsonArray toJsonArray(const QBluetoothDeviceInfo::ServiceClasses &classes)
{
    QJsonArray array;
    #define TEST_THEN_ADD(flag) if (classes.testFlag(QBluetoothDeviceInfo::flag)) array.append(QLatin1String(#flag))
    TEST_THEN_ADD(PositioningService);
    TEST_THEN_ADD(NetworkingService);
    TEST_THEN_ADD(RenderingService);
    TEST_THEN_ADD(CapturingService);
    TEST_THEN_ADD(ObjectTransferService);
    TEST_THEN_ADD(AudioService);
    TEST_THEN_ADD(TelephonyService);
    TEST_THEN_ADD(InformationService);
    #undef TEST_THEN_ADD
    return array;
}

QJsonArray toJsonArray(const QList<QBluetoothUuid> &uuids)
{
    QJsonArray array;
    for (const QBluetoothUuid &uuid: uuids) {
        array.append(uuid.toString());
    }
    return array;
}

QJsonArray toJsonArray(const QMultiHash<quint16, QByteArray> &data)
{
    QJsonArray array;
    for (auto iter = data.cbegin(); iter != data.cend(); ++iter) {
        array.append(QJsonObject{
            { QLatin1String("manufacturerId"), iter.key() },
            { QLatin1String("data"), QString::fromLatin1(iter.value().toBase64()) },
        });
    }
    return array;
}

QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
{
    #define TEST_THEN_RETURN(value) if (majorClass == QBluetoothDeviceInfo::value) return QLatin1String(#value)
    TEST_THEN_RETURN(MiscellaneousDevice);
    TEST_THEN_RETURN(ComputerDevice);
    TEST_THEN_RETURN(PhoneDevice);
    TEST_THEN_RETURN(NetworkDevice);
    TEST_THEN_RETURN(AudioVideoDevice);
    TEST_THEN_RETURN(PeripheralDevice);
    TEST_THEN_RETURN(ImagingDevice);
    TEST_THEN_RETURN(WearableDevice);
    TEST_THEN_RETURN(ToyDevice);
    TEST_THEN_RETURN(HealthDevice);
    TEST_THEN_RETURN(UncategorizedDevice);
    #undef TEST_THEN_RETURN
    return QString::number(majorClass);
}

QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass)
{
    switch (majorClass) {
    case QBluetoothDeviceInfo::MiscellaneousDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::ComputerDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::PhoneDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::NetworkDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::AudioVideoDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::PeripheralDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::ImagingDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::WearableDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::ToyDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::HealthDevice:
        /// \todo
        break;
    case QBluetoothDeviceInfo::UncategorizedDevice:
        // There are no minor classes defined (in Qt) for uncategorized devices.
        break;
    }
    // Fallback to just converting the unsigned integer to a string.
    return QString::number(minorClass);
}

Discover::Discover(QObject * const parent) : QObject(parent)
{
    discoveryAgent = new PokitDeviceDiscoveryAgent(this);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::pokitDeviceDiscovered, this, [this](const QBluetoothDeviceInfo &info) {
        QLowEnergyController * c = QLowEnergyController::createCentral(info);
        qDebug() << c;
        qDebug() << c->services().size();

        connect(c, &QLowEnergyController::connected, this, [c]() {
            qDebug() << "device connected";
            c->discoverServices();
        });

        QJsonObject json{
            { QLatin1String("address"), info.address().toString() },
            { QLatin1String("name"), info.name() },
            { QLatin1String("isCached"), info.isCached() },
            { QLatin1String("majorDeviceClass"), info.majorDeviceClass() },
            { QLatin1String("majorDeviceClass"), toString(info.majorDeviceClass()) },
            { QLatin1String("minorDeviceClass"), toString(info.majorDeviceClass(), info.minorDeviceClass()) },
            { QLatin1String("signalStrength"), info.rssi() },
            { QLatin1String("serviceUuids"), toJsonArray(info.serviceUuids()) },

        };
        if (!info.deviceUuid().isNull()) {
            json.insert(QLatin1String("deviceUuid"), info.deviceUuid().toString());
        }
        if (!info.manufacturerData().isEmpty()) {
            json.insert(QLatin1String("manufacturerData"), toJsonArray(info.manufacturerData()));
        }
        if (info.serviceClasses() != QBluetoothDeviceInfo::NoService) {
            json.insert(QLatin1String("serviceClasses"), toJsonArray(info.serviceClasses()));
        }
        fputs(QJsonDocument(json).toJson(), stdout);

        connect(c, &QLowEnergyController::serviceDiscovered, this, [c](const QBluetoothUuid &service) {
            qDebug() << "service discovered" << service;
            if (service == QBluetoothUuid(QStringLiteral(POKIT_SERVICE_STATUS))) {
                qDebug() << "status";
                QLowEnergyService * s = c->createServiceObject(service);
                qDebug() << s;
                if (s != nullptr) {
                    s->discoverDetails();
                }
            }
        });

        connect(c, &QLowEnergyController::discoveryFinished, this, [c]() {
            qDebug() << "service discovery finished" << c->services().size();
            QCoreApplication::quit();
        });

        c->connectToDevice();
    });

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::finished, this, []() {
        qDebug() << "device discovery finished";
//        QCoreApplication::quit();
    });

    discoveryAgent->setLowEnergyDiscoveryTimeout(3000);
    discoveryAgent->start();
}
