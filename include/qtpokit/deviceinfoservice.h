// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
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
    /// UUID of the "Device Info" service.
    static inline const QBluetoothUuid serviceUuid
        { QBluetoothUuid::ServiceClassUuid::DeviceInformation };

    /// Characteristics available via the `Device Info` service.
    struct QTPOKIT_EXPORT CharacteristicUuids {
        /// UUID of the `Device Info` service's `Manufacturer Name String` characterstic.
        static inline const QBluetoothUuid manufacturerName
            { QBluetoothUuid::CharacteristicType::ManufacturerNameString };

        /// UUID of the `Device Info` service's `Model Number String` characterstic.
        static inline const QBluetoothUuid modelNumber
            { QBluetoothUuid::CharacteristicType::ModelNumberString };

        /// UUID of the `Device Info` service's `Firmware Revision String` characterstic.
        static inline const QBluetoothUuid firmwareRevision
            { QBluetoothUuid::CharacteristicType::FirmwareRevisionString };

        /// UUID of the `Device Info` service's `Hardware Revision String` characterstic.
        static inline const QBluetoothUuid hardwareRevision
            { QBluetoothUuid::CharacteristicType::HardwareRevisionString };

        /// UUID of the `Device Info` service's `Software Revision String` characterstic.
        static inline const QBluetoothUuid softwareRevision
            { QBluetoothUuid::CharacteristicType::SoftwareRevisionString };
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
