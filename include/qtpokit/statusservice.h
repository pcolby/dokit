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

#include <optional>

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

        /// UUID of the `Pokit Status` service's (undocumented) `Torch` characterstic.
        static inline const QBluetoothUuid torch { QStringLiteral("aaf3f6d5-43d4-4a83-9510-dff3d858d4cc") };
        ///  Torch - one byte, 0x00 off, 0x01 on.

        /// UUID of the `Pokit Status` service's (undocumented) `Button Press` characterstic.
        static inline const QBluetoothUuid buttonPress { QStringLiteral("8fe5b5a9-b5b4-4a7b-8ff2-87224b970f89") };
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
    static QString toString(const StatusService::BatteryStatus &status);

    /// Values supported by the (undocumented) `Switch Position` attribute of the `Status` characteristic.
    enum class SwitchPosition : quint8 {
        Voltage     = 0, ///< Device is switched to Voltage position.
        MultiMode   = 1, ///< Device is switched to Resistance / Low Current / Capacitance / Diode position.
        HighCurrent = 2, ///< Device is switched to High Current position.
    };
    static QString toString(const StatusService::SwitchPosition &position);

    /// Values supported by the (undocumented) `Charging Statue` attribute of the `Status` characteristic.
    enum class ChargingStatus : quint8 {
        Discharging = 0, ///< Battery is dischargin.
        Charging    = 1, ///< Battery is being charged.
        Charged     = 2, ///< Battery is fully charged.
    };
    static QString toString(const StatusService::ChargingStatus &status);

    /// Attributes included in the `Status` characterstic.
    struct Status {
        DeviceStatus deviceStatus;   ///< Current Pokit device status.
        float batteryVoltage;        ///< Current battery voltage level.
        BatteryStatus batteryStatus; ///< Logical interpretation the battery voltage level.
        std::optional<SwitchPosition> switchPosition; ///< Position of the Pokit device's physical mode switch.
        std::optional<ChargingStatus> chargingStatus; ///< Current charging status, if supported by the device.
    };

    /// Values supported by the single byte of the attribute of the (undocumented) `Torch` characteristic.
    enum class TorchStatus : quint8 {
        Off = 0, ///< Torch is off.
        On  = 1, ///< Torch is on.
    };
    static QString toString(const StatusService::TorchStatus &status);

    /// Values supported by the second byte of the attribute of the (undocumented) `Button Press` characteristic.
    enum class ButtonStatus : quint8 {
        Released = 0, ///< Button was released.
        Pressed  = 1, ///< Button was pressed.
        Held     = 2, ///< Button was held down (for typically 1,500ms).
    };
    static QString toString(const StatusService::ButtonStatus &status);

    StatusService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~StatusService() override;

    bool readCharacteristics() override;
    bool readDeviceCharacteristics();
    bool readStatusCharacteristic();
    bool readNameCharacteristic();
    bool readTorchCharacteristic();
    bool readButtonPressCharacteristic();

    // Device Characteristics characteristic (BLE read only).
    DeviceCharacteristics deviceCharacteristics() const;

    // Status characteristic (Read only).
    Status status() const;
    /// \todo bool enableStatusNotifications();
    /// \todo bool disableStatusNotifications();

    // Device Name characteristic (BLE read/write).
    QString deviceName() const;
    bool setDeviceName(const QString &name);

    // Flash LED characteristic (BLE write only).
    bool flashLed();

    // Undocumented Torch characteristic (BLE read, and possibly write?).
    TorchStatus torchStatus();
    bool setTorchStatus(const TorchStatus status); /// \todo Test write.
    /// \todo bool enableTorchNotifications();
    /// \todo bool disableTorchNotifications();

    // Undocumented Button Press characteristic (presumably BLE read only).
    std::pair<quint8, ButtonStatus> buttonPress();
    /// \todo bool enableButtonPressedNotifications();
    /// \todo bool disableButtonPressedNotifications();

signals:
    void deviceCharacteristicsRead(const StatusService::DeviceCharacteristics &characteristics);
    void deviceNameRead(const QString &deviceName);
    void deviceNameWritten();
    void deviceStatusRead(const StatusService::Status &status);
    void deviceLedFlashed();
    void torchStatusRead(const TorchStatus &status);
    void buttonPressRead(const quint8 &unknown, const ButtonStatus status); /// \todo What is the \c unknown byte here?

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
