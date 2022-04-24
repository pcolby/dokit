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
 * Defines the StatusService and StatusServicePrivate classes.
 */

#include <qtpokit/statusservice.h>
#include "statusservice_p.h"
#include "logging_p.h"

#include <QtEndian>

/*!
 * \class StatusService
 *
 * The StatusService class accesses the `Pokit Status` service of Pokit devices.
 */

/// UUID of the "Pokit Status" service.
const QBluetoothUuid StatusService::
    serviceUuid(QLatin1String("57d3a771-267c-4394-8872-78223e92aec5"));

/// \struct StatusService::CharacteristicUuids
/// \brief Characteristics available via the `Pokit Status` service.

/// UUID of the `Pokit Status` service's `Device Characteristics` characterstic.
const QBluetoothUuid StatusService::CharacteristicUuids::
    deviceCharacteristics(QLatin1String("6974f5e5-0e54-45c3-97dd-29e4b5fb0849"));

/// UUID of the `Pokit Status` service's `Status` characterstic.
const QBluetoothUuid StatusService::CharacteristicUuids::
    status(QLatin1String("3dba36e1-6120-4706-8dfd-ed9c16e569b6"));

/// UUID of the `Pokit Status` service's `Device Name` characterstic.
const QBluetoothUuid StatusService::CharacteristicUuids::
    name(QLatin1String("7f0375de-077e-4555-8f78-800494509cc3"));

/// UUID of the `Pokit Status` service's `Flash LED` characterstic.
const QBluetoothUuid StatusService::CharacteristicUuids::
    flashLed(QLatin1String("ec9bb1f3-05a9-4277-8dd0-60a7896f0d6e"));

/// \struct StatusService::DeviceCharacteristics
/// \brief Attributes included in the `Device Characteristics` characterstic.

/// \enum StatusService::DeviceStatus
/// \brief Values support by the `Status` attribute of the `Status` characteristic.

/// \enum StatusService::BatteryStatus
/// \brief Values support by the `Battery Status` attribute of the `Status` characteristic.

/// \struct StatusService::Status
/// \brief Attributes included in the `Status` characterstic.

/*!
 * Constructs a new Pokit service with \a parent.
 */
StatusService::StatusService(QLowEnergyController * const controller, QObject * parent)
    : AbstractPokitService(new StatusServicePrivate(controller, this), parent)
{

}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
StatusService::StatusService(
    StatusServicePrivate * const d, QObject * const parent)
    : AbstractPokitService(d, parent)
{

}
/// \endcond

/*!
 * Destroys this StatusService object.
 */
StatusService::~StatusService()
{
    delete d_ptr;
}

bool StatusService::readCharacteristics()
{
    const bool r1 = readDeviceCharacteristics();
    const bool r2 = readStatusCharacteristic();
    const bool r3 = readNameCharacteristic();
    return (r1 && r2 && r3);
}

/*!
 * Read the `Status` service's `Device Characteristics` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits deviceCharacteristicsRead() if/when the characteristic has been read successfully.
 */
bool StatusService::readDeviceCharacteristics()
{
    Q_D(StatusService);
    return d->readCharacteristic(CharacteristicUuids::deviceCharacteristics);
}

/*!
 * Read the `Status` service's `Status` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits deviceStatusRead() if/when the characteristic has been read successfully.
 */
bool StatusService::readStatusCharacteristic()
{
    Q_D(StatusService);
    return d->readCharacteristic(CharacteristicUuids::status);
}

/*!
 * Read the `Status` service's `Name` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits deviceNameRead() if/when the characteristic has been read successfully.
 */
bool StatusService::readNameCharacteristic()
{
    Q_D(StatusService);
    return d->readCharacteristic(CharacteristicUuids::name);
}

/*!
 * Returns the most recent value of the `Status` service's `Device Characteristics` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then a
 * null result is returned, which can be checked via the returned
 * DeviceCharacteristics::firmwareVersion, like:
 *
 * ```
 * const DeviceCharacteristics characteristics = service->deviceCharacteristics();
 * if (!characteristics.firmwareVersion.isNull()) {
 *     ...
 * }
 * ```
 */
StatusService::DeviceCharacteristics StatusService::deviceCharacteristics() const
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("No device characteristics without a service object.");
        return StatusService::DeviceCharacteristics();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::deviceCharacteristics);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Device characteristics not valid yet.");
        return StatusService::DeviceCharacteristics();
    }

    return StatusServicePrivate::parseDeviceCharacteristics(characteristic.value());
}

/*!
 * Returns the most recent value of the `Status` service's `Status` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then the
 * returned StatusService::Status::batteryLevel member will be a quiet NaN, which can be checked
 * like:
 *
 * ```
 * const StatusService::Status status = statusService->status();
 * if (qIsNaN(status.batteryLevel)) {
 *     // Handle failure.
 * }
 * ```
 */
StatusService::Status StatusService::status() const
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("No device status without a service object.");
        return StatusService::Status
            { DeviceStatus::Idle, std::numeric_limits<float>::quiet_NaN(), BatteryStatus::Low };
        return StatusService::Status
            { DeviceStatus::Idle, std::numeric_limits<float>::quiet_NaN(), BatteryStatus::Low };
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::status);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Status characteristic not valid yet.");
        return StatusService::Status
            { DeviceStatus::Idle, std::numeric_limits<float>::quiet_NaN(), BatteryStatus::Low };
    }

    return StatusServicePrivate::parseStatus(characteristic.value());
}

/*!
 * Returns a string version of the \a status enum label.
 */
QString StatusService::deviceStatusLabel(const StatusService::DeviceStatus &status)
{
    switch (status) {
    case DeviceStatus::Idle:                 return  QLatin1String("Idle");
    case DeviceStatus::MultimeterDcVoltage:  return  QLatin1String("MultimeterDcVoltage");
    case DeviceStatus::MultimeterAcVoltage:  return  QLatin1String("MultimeterAcVoltage");
    case DeviceStatus::MultimeterDcCurrent:  return  QLatin1String("MultimeterDcCurrent");
    case DeviceStatus::MultimeterAcCurrent:  return  QLatin1String("MultimeterAcCurrent");
    case DeviceStatus::MultimeterResistance: return  QLatin1String("MultimeterResistance");
    case DeviceStatus::MultimeterDiode:      return  QLatin1String("MultimeterDiode");
    case DeviceStatus::MultimeterContinuity: return  QLatin1String("MultimeterContinuity");
    case DeviceStatus::MultimeterTemperature:return  QLatin1String("MultimeterTemperature");
    case DeviceStatus::DsoModeSampling:      return  QLatin1String("DsoModeSampling");
    case DeviceStatus::LoggerModeSampling:   return  QLatin1String("LoggerModeSampling");
    }
    return QLatin1String("Invalid");
}

/*!
 * Returns a string version of the \a status enum label.
 */
QString StatusService::batteryStatusLabel(const StatusService::BatteryStatus &status)
{
    switch (status) {
    case BatteryStatus::Low:  return QLatin1String("Low");
    case BatteryStatus::Good: return QLatin1String("Good");
    }
    return QLatin1String("Invalid");
}

/*!
 * Returns the most recent value of the `Status` services's `Device Name` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then a
 * null QString is returned.
 */
QString StatusService::deviceName() const
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("No device name without a service object.");
        return QString();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::name);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Name characteristic not valid yet.");
        return QString();
    }

    return QString::fromUtf8(characteristic.value());
}

/*!
 * Set's the Pokit device's name to \a name.
 *
 * Returns `true` if the write request was successfully queued, `false` otherwise.
 *
 * Emits deivceNameWritten() if/when the \a name has been set.
 */
bool StatusService::setDeviceName(const QString &name)
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("Cannot set device name without a service object.");
        return false;
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::name);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Name characteristic not valid yet.");
        return false;
    }

    const QByteArray value = name.toUtf8();
    if (value.length() > 11) {
        qCWarning(pokitService).noquote() << tr("Device name \"%1\" is too long (%2 > 11 bytes): 0x3")
            .arg(name).arg(value.length()).arg(QLatin1String(value.toHex()));
        return false;
    }

    d->service->writeCharacteristic(characteristic, value);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}

/*!
 * Flash the Pokit device's LED.
 *
 * Returns `true` if the flash request was successfully queued, `false` otherwise.
 *
 * Emits deviceLedFlashed() if/when the LED has flashed successfully.
 *
 * \note This does not appear to work currently, for the one (Pokit Pro) device available for
 * testing. Instead, the underlying Bluetooth stack returns ATT error `0x80`.
 *
 * \todo Diagnose and/or follow up with Pokit Innovations.
 */
bool StatusService::flashLed()
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("Cannot flash LED without a service object.");
        return false;
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::flashLed);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Flash LED characteristic not valid yet.");
        return false;
    }

    // The Flash LED characeristic is write-only, and takes a single uint8 "LED" parameter, which
    // must always be 1. Presumably this is an index for which LED to flash, but the Pokit API docs
    // say that "any value other than 1 will be ignored", which makes sense given that all current
    // Pokit devices have only one LED.
    const QByteArray value(1, '\x01');
    d->service->writeCharacteristic(characteristic, value);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}

/*!
 * \fn StatusService::deviceCharacteristicsRead
 *
 * This signal is emitted when the `Device Characteristics` characteristic has been read
 * successfully.
 *
 * \see readDeviceCharacteristics
 */

/*!
 * \fn StatusService::deviceNameRead
 *
 * This signal is emitted when the `Device Name` characteristic has been read successfully.
 *
 * \see readDeviceName
 */

/*!
 * \fn StatusService::deivceNameWritten
 *
 * This signal is emitted when the `Device Name` characteristic has been written successfully.
 *
 * \see setDeviceName
 */

/*!
 * \fn StatusService::deviceStatusRead
 *
 * This signal is emitted when the `Status` characteristic has been read successfully.
 *
 * \see readDeviceStatus
 */

/*!
 * \fn StatusService::deviceLedFlashed
 *
 * This signal is emitted when device's LED has flashed in response to a write of the `Flash LED`
 * characteristic.
 */

/*!
 * \cond internal
 * \class StatusServicePrivate
 *
 * The StatusServicePrivate class provides private implementation for StatusService.
 */

/*!
 * \internal
 * Constructs a new StatusServicePrivate object with public implementation \a q.
 */
StatusServicePrivate::StatusServicePrivate(
    QLowEnergyController * controller, StatusService * const q)
    : AbstractPokitServicePrivate(StatusService::serviceUuid, controller, q)
{

}

/*!
 * Parses the `Device Characteristics` \a value into a DeviceCharacteristics struct.
 */
StatusService::DeviceCharacteristics StatusServicePrivate::parseDeviceCharacteristics(
    const QByteArray &value)
{
    StatusService::DeviceCharacteristics characteristics;
    Q_ASSERT(characteristics.firmwareVersion.isNull());  // How we indicate failure.

    if (value.size() < 20) {
        qCWarning(pokitService).noquote() << tr("Invalid characteristics size %1 for value: 0x%2")
            .arg(value.size()).arg(QLatin1String(value.toHex()));
        return characteristics;
    }
    if (value.size() > 20) {
        qCWarning(pokitService).noquote() << tr("Characterisitcs has %1 extra bytes: 0x%2")
            .arg(value.size()-20).arg(QLatin1String(value.mid(20).toHex()));
    }

    characteristics.firmwareVersion = QVersionNumber(
                                          qFromLittleEndian<quint8 >(value.mid(0,1)),
                                          qFromLittleEndian<quint8 >(value.mid(1,1)));
    characteristics.maximumVoltage      = qFromLittleEndian<quint16>(value.mid(2,2));
    characteristics.maximumCurrent      = qFromLittleEndian<quint16>(value.mid(4,2));
    characteristics.maximumResistance   = qFromLittleEndian<quint16>(value.mid(6,2));
    characteristics.maximumSamplingRate = qFromLittleEndian<quint16>(value.mid(8,2));
    characteristics.samplingBufferSize  = qFromLittleEndian<quint16>(value.mid(10,2));
    characteristics.capabilityMask      = qFromLittleEndian<quint16>(value.mid(12,2));
    characteristics.macAddress = QBluetoothAddress(qFromBigEndian<quint64>
                                                   (QByteArray(2, '\0') + value.mid(14,6)));

    qCDebug(pokitService).noquote() << tr("Firmware version:     ") << characteristics.firmwareVersion;
    qCDebug(pokitService).noquote() << tr("Maximum voltage:      ") << characteristics.maximumVoltage;
    qCDebug(pokitService).noquote() << tr("Maximum current:      ") << characteristics.maximumCurrent;
    qCDebug(pokitService).noquote() << tr("Maximum resistance:   ") << characteristics.maximumResistance;
    qCDebug(pokitService).noquote() << tr("Maximum sampling rate:") << characteristics.maximumSamplingRate;
    qCDebug(pokitService).noquote() << tr("Sampling buffer size: ") << characteristics.samplingBufferSize;
    qCDebug(pokitService).noquote() << tr("Capability mask:      ") << characteristics.capabilityMask;
    qCDebug(pokitService).noquote() << tr("MAC address:          ") << characteristics.macAddress;

    Q_ASSERT(!characteristics.firmwareVersion.isNull()); // How we indicate success.
    return characteristics;
}

/*!
 * Parses the `Status` \a value into a DeviceStatus and battery voltage.
 */
StatusService::Status StatusServicePrivate::parseStatus(const QByteArray &value)
{
    StatusService::Status status{
        static_cast<StatusService::DeviceStatus>
            (std::numeric_limits<StatusService::DeviceStatus>::max()),
        std::numeric_limits<float>::quiet_NaN(),
        static_cast<StatusService::BatteryStatus>
            (std::numeric_limits<StatusService::BatteryStatus>::max()),
    };

    if (value.size() < 5) {
        qCWarning(pokitService).noquote() << tr("Invalid status size %1 for value: 0x%2")
            .arg(value.size()).arg(QLatin1String(value.toHex()));
        return status;
    }
    if (value.size() > 6) {
        qCWarning(pokitService).noquote() << tr("Status has %1 extra bytes: 0x%2")
            .arg(value.size()-6).arg(QLatin1String(value.mid(6).toHex()));
    }

    status.deviceStatus = static_cast<StatusService::DeviceStatus>(value.at(0));
    status.batteryVoltage = qFromLittleEndian<float>(value.mid(1,4));
    if (value.size() >= 6) { // Battery Status added to Pokit API docs v1.00.
        status.batteryStatus = static_cast<StatusService::BatteryStatus>(value.at(5));
    }
    qCDebug(pokitService).noquote() << tr("Device status:   %1 (%2)")
        .arg((quint8)status.deviceStatus).arg(StatusService::deviceStatusLabel(status.deviceStatus));
    qCDebug(pokitService).noquote() << tr("Battery voltage: %1 volts").arg(status.batteryVoltage);
    qCDebug(pokitService).noquote() << tr("Battery status:  %1 (%2)")
        .arg((quint8)status.batteryStatus);
    return status;
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicRead to parse \a value, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void StatusServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    qCDebug(pokitService).noquote() << tr("Read  characteristic \"%1\" (%2) of size %3: 0x%4")
        .arg(characteristic.name(), characteristic.uuid().toString()).arg(value.size())
        .arg(QLatin1String(value.toHex()));

    Q_Q(StatusService);
    if (characteristic.uuid() == StatusService::CharacteristicUuids::deviceCharacteristics) {
        const StatusService::DeviceCharacteristics characteristics = parseDeviceCharacteristics(value);
        if (characteristics.firmwareVersion.isNull()) {
            qCWarning(pokitService).noquote() << tr("Failed to parse device characteristics: 0x%1")
                .arg(QLatin1String(value.toHex()));
            return;
        }
        emit q->deviceCharacteristicsRead(characteristics);
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::status) {
        const StatusService::Status status = parseStatus(value);
        if (qIsNaN(status.batteryVoltage)) {
            qCWarning(pokitService).noquote() << tr("Failed to parse status: 0x%1")
                .arg(QLatin1String(value.toHex()));
            return;
        }
        emit q->deviceStatusRead(status);
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::name) {
        const QString deviceName = QString::fromUtf8(value);
        qCDebug(pokitService).noquote() << tr("Device name: \"%1\"").arg(deviceName);
        emit q->deviceNameRead(deviceName);
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::flashLed) {
        qCWarning(pokitService).noquote() << tr("Flash LED characteristic is write-only, but somehow read")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(pokitService).noquote() << tr("Unknown characteristic read for Status Service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void StatusServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                 const QByteArray &newValue)
{
    qCDebug(pokitService).noquote() << tr("Characteristic \"%1\" (%2) written, with new value:")
        .arg(characteristic.name(), characteristic.uuid().toString()) << newValue;

    Q_Q(StatusService);
    if (characteristic.uuid() == StatusService::CharacteristicUuids::deviceCharacteristics) {
        qCWarning(pokitService).noquote() << tr("Device Characteristics is read-only, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::status) {
        qCWarning(pokitService).noquote() << tr("Status characteristic is read-only, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::name) {
        emit q->deivceNameWritten();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::flashLed) {
        emit q->deviceLedFlashed();
        return;
    }

    qCWarning(pokitService).noquote() << tr("Unknown characteristic written for Status Service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
