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

/*!
 * \file
 * Declares the AbstractPokitServicePrivate class.
 */

#ifndef QTPOKIT_ABSTRACTPOKITSERVICE_P_H
#define QTPOKIT_ABSTRACTPOKITSERVICE_P_H

#include <qtpokit/qtpokit_global.h>

#include <QLoggingCategory>
#include <QLowEnergyService>
#include <QObject>

class QLowEnergyController;

QTPOKIT_BEGIN_NAMESPACE

class AbstractPokitService;

class QTPOKIT_EXPORT AbstractPokitServicePrivate : public QObject
{
    Q_OBJECT

public:
    static Q_LOGGING_CATEGORY(lc, "pokit.ble.service", QtInfoMsg); ///< Logging category.

    bool autoDiscover;                 ///< Whether autodiscovery is enabled or not.
    QLowEnergyController * controller; ///< BLE controller to fetch the service from.
    QLowEnergyService * service;       ///< BLE service to read/write characteristics.
    const QBluetoothUuid serviceUuid;  ///< UUID for #service.

    AbstractPokitServicePrivate(const QBluetoothUuid &serviceUuid,
        QLowEnergyController * controller, AbstractPokitService * const q);

    bool createServiceObject();
    QLowEnergyCharacteristic getCharacteristic(const QBluetoothUuid &uuid) const;
    bool readCharacteristic(const QBluetoothUuid &uuid);

    bool enableCharacteristicNotificatons(const QBluetoothUuid &uuid);
    bool disableCharacteristicNotificatons(const QBluetoothUuid &uuid);

protected:
    AbstractPokitService * q_ptr; ///< Internal q-pointer.

protected slots:
    void connected();
    void discoveryFinished();
    void errorOccurred(const QLowEnergyService::ServiceError newError);
    void serviceDiscovered(const QBluetoothUuid &newService);
    void stateChanged(QLowEnergyService::ServiceState newState);

    virtual void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                    const QByteArray &value);
    virtual void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                       const QByteArray &newValue);
    virtual void characteristicChanged(const QLowEnergyCharacteristic &characteristic,
                                       const QByteArray &newValue);

private:
    Q_DECLARE_PUBLIC(AbstractPokitService)
    Q_DISABLE_COPY(AbstractPokitServicePrivate)
    friend class TestAbstractPokitService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_ABSTRACTPOKITSERVICE_P_H
