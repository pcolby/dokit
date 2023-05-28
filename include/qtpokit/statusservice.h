// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the StatusService class.
 */

#ifndef QTPOKIT_STATUSSERVICE_H
#define QTPOKIT_STATUSSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class StatusServicePrivate;

class QTPOKIT_EXPORT StatusService : public AbstractPokitService
{
    Q_OBJECT

public:
    /// UUIDs of the `Pokit Status` service.
    struct QTPOKIT_EXPORT ServiceUuids {
        /// UUID of the Pokit Meter's `Pokit Status` service.
        static inline const QBluetoothUuid pokitMeter { QStringLiteral("57d3a771-267c-4394-8872-78223e92aec4") };

        /// UUID of the Pokit Pro's `Pokit Status` service.
        static inline const QBluetoothUuid pokitPro   { QStringLiteral("57d3a771-267c-4394-8872-78223e92aec5") };
    };

    /// Characteristics available via the `Pokit Status` service.
    struct QTPOKIT_EXPORT CharacteristicUuids {
        /// UUID of the `Pokit Status` service's `Device Characteristics` characterstic.
        static inline const QBluetoothUuid deviceCharacteristics  { QStringLiteral("6974f5e5-0e54-45c3-97dd-29e4b5fb0849") };

        /// UUID of the `Pokit Status` service's `Status` characterstic.
        static inline const QBluetoothUuid status { QStringLiteral("3dba36e1-6120-4706-8dfd-ed9c16e569b6") };

        /// UUID of the `Pokit Status` service's `Device Name` characterstic.
        static inline const QBluetoothUuid name { QStringLiteral("7f0375de-077e-4555-8f78-800494509cc3") };

        /// UUID of the `Pokit Status` service's `Flash LED` characterstic.
        static inline const QBluetoothUuid flashLed { QStringLiteral("ec9bb1f3-05a9-4277-8dd0-60a7896f0d6e") };
    };

    /// Attributes included in the `Device Characteristics` characterstic.
    struct DeviceCharacteristics {
        QVersionNumber firmwareVersion; ///< Device's major and minor firmware version.
        quint16 maximumVoltage;         ///< Device's maximum input voltage.
        quint16 maximumCurrent;         ///< Device's maximum input current.
        quint16 maximumResistance;      ///< Device's maximum input resistance.
        quint16 maximumSamplingRate;    ///< Device's maximum sampling rate.
        quint16 samplingBufferSize;     ///< Device's sampling buffer size.
        quint16 capabilityMask;         ///< Reserved.
        QBluetoothAddress macAddress;   ///< Device's MAC address.
    };

    /// Values supported by the `Status` attribute of the `Status` characteristic.
    enum class DeviceStatus : quint8 {
        Idle = 0,                 ///< Device is idle.
        MultimeterDcVoltage = 1,  ///< Multimeter is measuring DC voltage.
        MultimeterAcVoltage = 2,  ///< Multimeter is measuring AC voltage.
        MultimeterDcCurrent = 3,  ///< Multimeter is measuring DC current.
        MultimeterAcCurrent = 4,  ///< Multimeter is measuring AC current.
        MultimeterResistance = 5, ///< Multimeter is measuring resistance.
        MultimeterDiode = 6,      ///< Multimeter is measuring diode.
        MultimeterContinuity = 7, ///< Multimeter is measuring continuity.
        MultimeterTemperature = 8,///< Multimeter is measuring temperature.
        DsoModeSampling = 9,      ///< DSO is sampling.
        LoggerModeSampling = 10,  ///< Data Logger is sampling.
    };
    static QString toString(const StatusService::DeviceStatus &status);

    /// Values supported by the `Battery Status` attribute of the `Status` characteristic.
    enum class BatteryStatus : quint8 {
        Low = 0,  ///< Low (replace battery).
        Good = 1, ///< Good.
    };
    static QString toString(const StatusService::BatteryStatus &label);

    struct Status {
        DeviceStatus deviceStatus;   ///< Current Pokit device status.
        float batteryVoltage;        ///< Current battery voltage level.
        BatteryStatus batteryStatus; ///< Logical interpretation the battery voltage level.

    };

    StatusService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~StatusService() override;

    bool readCharacteristics() override;
    bool readDeviceCharacteristics();
    bool readStatusCharacteristic();
    bool readNameCharacteristic();

    // Device Characteristics characteristic (BLE read only).
    DeviceCharacteristics deviceCharacteristics() const;

    // Status characteristic (Read only).
    Status status() const;

    // Device Name characteristic (BLE read/write).
    QString deviceName() const;
    bool setDeviceName(const QString &name);

    // Flash LED characteristic (BLE write only).
    bool flashLed();

signals:
    void deviceCharacteristicsRead(const StatusService::DeviceCharacteristics &characteristics);
    void deviceNameRead(const QString &deviceName);
    void deviceNameWritten();
    void deviceStatusRead(const StatusService::Status &status);
    void deviceLedFlashed();

protected:
    /// \cond internal
    StatusService(StatusServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(StatusService)
    Q_DISABLE_COPY(StatusService)
    friend class TestStatusService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_STATUSSERVICE_H
