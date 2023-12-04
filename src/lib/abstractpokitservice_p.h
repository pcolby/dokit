// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the AbstractPokitServicePrivate class.
 */

#ifndef QTPOKIT_ABSTRACTPOKITSERVICE_P_H
#define QTPOKIT_ABSTRACTPOKITSERVICE_P_H

#include <qtpokit/qtpokit_global.h>
#include <qtpokit/pokitproducts.h>

#include <QLoggingCategory>
#include <QLowEnergyService>
#include <QObject>

#include <optional>

class QLowEnergyController;

QTPOKIT_BEGIN_NAMESPACE

class AbstractPokitService;

class QTPOKIT_EXPORT AbstractPokitServicePrivate : public QObject
{
    Q_OBJECT

public:
    static Q_LOGGING_CATEGORY(lc, "pokit.ble.service", QtInfoMsg); ///< Logging category.

    bool autoDiscover { true };                    ///< Whether autodiscovery is enabled or not.
    QLowEnergyController * controller { nullptr }; ///< BLE controller to fetch the service from.
    std::optional<PokitProduct> pokitProduct;      ///< The Pokit product #controller is connected to.
    QLowEnergyService * service { nullptr };       ///< BLE service to read/write characteristics.
    QBluetoothUuid serviceUuid;                    ///< UUIDs for #service.

    AbstractPokitServicePrivate(const QBluetoothUuid &serviceUuid,
        QLowEnergyController * controller, AbstractPokitService * const q);

    bool createServiceObject();
    QLowEnergyCharacteristic getCharacteristic(const QBluetoothUuid &uuid) const;
    bool readCharacteristic(const QBluetoothUuid &uuid);

    bool enableCharacteristicNotificatons(const QBluetoothUuid &uuid);
    bool disableCharacteristicNotificatons(const QBluetoothUuid &uuid);

    static bool checkSize(const QString &label, const QByteArray &data, const int minSize,
                          const int maxSize=-1, const bool failOnMax=false);
    static QString toHexString(const QByteArray &data, const int maxSize=20);

protected:
    AbstractPokitService * q_ptr; ///< Internal q-pointer.

protected Q_SLOTS:
    void connected();
    void discoveryFinished();
    void errorOccurred(const QLowEnergyService::ServiceError newError);
    virtual void serviceDiscovered(const QBluetoothUuid &newService);
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
