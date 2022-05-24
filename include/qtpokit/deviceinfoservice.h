// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the DeviceInfoService class.
 */

#ifndef QTPOKIT_INFOSERVICE_H
#define QTPOKIT_INFOSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class DeviceInfoServicePrivate;

class QTPOKIT_EXPORT DeviceInfoService : public AbstractPokitService
{
    Q_OBJECT

public:
    static const QBluetoothUuid serviceUuid;

    struct CharacteristicUuids {
        static const QBluetoothUuid manufacturerName;
        static const QBluetoothUuid modelNumber;
        static const QBluetoothUuid hardwareRevision;
        static const QBluetoothUuid firmwareRevision;
        static const QBluetoothUuid softwareRevision;
    };

    DeviceInfoService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~DeviceInfoService() override;

    bool readCharacteristics() override;
    bool readFirmwareRevisionCharacteristic();
    bool readHardwareRevisionCharacteristic();
    bool readManufacturerCharacteristics();
    bool readModelNumberCharacteristic();
    bool readSoftwareRevisionCharacteristic();

    // All Device Info characteristics are read-only, single values.
    QString manufacturer() const;
    QString modelNumber() const;
    QString hardwareRevision() const;
    QString firmwareRevision() const;
    QString softwareRevision() const;

signals:
    void manufacturerRead(const QString &name);
    void modelNumberRead(const QString &model);
    void hardwareRevisionRead(const QString &revision);
    void firmwareRevisionRead(const QString &revision);
    void softwareRevisionRead(const QString &revision);

protected:
    /// \cond internal
    DeviceInfoService(DeviceInfoServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(DeviceInfoService)
    Q_DISABLE_COPY(DeviceInfoService)
    friend class TestDeviceInfoService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_INFOSERVICE_H
