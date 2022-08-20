// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the GenericAccessService class.
 */

#ifndef QTPOKIT_GENERICACCESSSERVICE_H
#define QTPOKIT_GENERICACCESSSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class GenericAccessServicePrivate;

class QTPOKIT_EXPORT GenericAccessService : public AbstractPokitService
{
    Q_OBJECT

public:
    static const QBluetoothUuid serviceUuid;

    struct QTPOKIT_EXPORT CharacteristicUuids {
        static const QBluetoothUuid deviceName;
        static const QBluetoothUuid appearance;
    };

    GenericAccessService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~GenericAccessService() override;

    bool readCharacteristics() override;
    bool readAppearanceCharacteristic();
    bool readDeviceNameCharacteristic();

    // Appearance characteristic (BLE read only).
    quint16 appearance() const;

    // Device Name characteristic (BLE read/write).
    QString deviceName() const;
    bool setDeviceName(const QString &name);

signals:
    void appearanceRead(const quint16 appearance);
    void deviceNameRead(const QString &deviceName);
    void deviceNameWritten();

protected:
    /// \cond internal
    GenericAccessService(GenericAccessServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(GenericAccessService)
    Q_DISABLE_COPY(GenericAccessService)
    friend class TestGenericAccessService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_GENERICACCESSSERVICE_H
