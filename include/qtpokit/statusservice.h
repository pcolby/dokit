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
 * Declares the StatusService class.
 */

#ifndef QTPOKIT_STATUSSERVICE_H
#define QTPOKIT_STATUSSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QLowEnergyCharacteristic>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class StatusServicePrivate;

QLowEnergyController * createPokitDeviceController(const QString &addressOrUuid);

class QTPOKIT_EXPORT StatusService : public AbstractPokitService
{
    Q_OBJECT

public:
    static const QBluetoothUuid serviceUuid;

    struct CharacteristicUuids {
        static const QBluetoothUuid deviceCharacteristics;
        static const QBluetoothUuid status;
        static const QBluetoothUuid name;
        static const QBluetoothUuid flashLed;
    };

    struct DeviceCharacteristics {
        QVersionNumber firmwareVersion;
        quint16 maximumVoltage;
        quint16 maximumCurrent;
        quint16 maximumResistance;
        quint16 maximumSamplingRate;
        quint16 samplingBufferSize;
        quint16 capabilityMask; ///< Reserved.
        QBluetoothAddress macAddress;
    };

    enum class DeviceStatus {
        Idle = 0,
        MultimeterDcVoltage = 1,
        MultimeterAcVoltage = 2,
        MultimeterDcCurrent = 3,
        MultimeterAcCurrent = 4,
        MultimeterResistance = 5,
        MultimeterDiode = 6,
    };

    StatusService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~StatusService() override;

    bool readCharacteristics() override;
    bool readDeviceCharacteristics();
    bool readStatusCharacteristic();
    bool readNameCharacteristic();

    // Device Characteristics characteristic (BLE read only).
    int capabiltyMask() const;
    QVersionNumber firmwareVersion() const;
    int maximumCurrent() const;
    int maximumResistance() const;
    int maximumSamplingRate() const;
    int maximumVoltage() const;
    int samplingBufferSize() const;

    // Status characteristic (Read only).
    DeviceStatus deviceStatus() const;
    float batteryVoltage() const;

    // Device Name characteristic (BLE read/write).
    QString deviceName() const;
    void setDeviceName(const QString &name);

    // Flash LED characteristic (BLE write only).
    void flashLed();

signals:
    void deviceCharacteristicsRead(const StatusService::DeviceCharacteristics &characteristics);
    void deviceNameRead(const QString &deviceName);
    void deivceNameWritten();
    void deviceStatusRead(const StatusService::DeviceStatus status, const float batteryVoltage);
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

#endif // QTPOKIT_StatusSERVICE_H
