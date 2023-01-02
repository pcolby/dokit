// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the StatusService and StatusServicePrivate classes.
 */

#include <qtpokit/statusservice.h>
#include "statusservice_p.h"

#include <QtEndian>

/*!
 * \class StatusService
 *
 * The StatusService class accesses the `Pokit Status` service of Pokit devices.
 */

/*!
 * \struct StatusService::ServiceUuids
 *
 * UUIDs of the "Pokit Status" service.
 *
 * \cond internal
 * \pokitApi Pokit API 1.00 (and 0.02) states the Status Service UUID as
 * `57d3a771-267c-4394-8872-78223e92aec4` which is correct for the Pokit Meter, but Pokit Pro uses
 * `57d3a771-267c-4394-8872-78223e92aec5` instead, that is the last digit is a `5` not `4`.
 * \endcond
 */

/// UUID of the Pokit Meter's `Pokit Status` service.
const QBluetoothUuid StatusService::ServiceUuids::
    pokitMeter(QLatin1String("57d3a771-267c-4394-8872-78223e92aec4"));

/// UUID of the Pokit Pro's `Pokit Status` service.
const QBluetoothUuid StatusService::ServiceUuids::
    pokitPro(QLatin1String("57d3a771-267c-4394-8872-78223e92aec5"));

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
/// \brief Values supported by the `Status` attribute of the `Status` characteristic.

/*!
 * Returns a string version of the \a status enum label.
 */
QString StatusService::toString(const StatusService::DeviceStatus &status)
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
    return QString();
}

/// \enum StatusService::BatteryStatus
/// \brief Values supported by the `Battery Status` attribute of the `Status` characteristic.

/*!
 * Returns a string version of the \a status enum label.
 */
QString StatusService::toString(const StatusService::BatteryStatus &status)
{
    switch (status) {
    case BatteryStatus::Low:  return QLatin1String("Low");
    case BatteryStatus::Good: return QLatin1String("Good");
    }
    return QString();
}

/*!
 * \struct StatusService::Status
 * \brief Attributes included in the `Status` characterstic.
 *
 * \note Not all Pokit devices support the batteryStatus member, in which case the member will be
 * initilialised to the maximum value supported by the underlying type (ie `255`) to indicate "not
 * set".
 */

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
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::deviceCharacteristics);
    return (characteristic.isValid())
        ? StatusServicePrivate::parseDeviceCharacteristics(characteristic.value())
        : StatusService::DeviceCharacteristics();
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
 * if (qIsNaN(status.batteryVoltage)) {
 *     // Handle failure.
 * }
 * ```
 */
StatusService::Status StatusService::status() const
{
    Q_D(const StatusService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::status);
    return (characteristic.isValid()) ? StatusServicePrivate::parseStatus(characteristic.value())
        : StatusService::Status{ DeviceStatus::Idle, std::numeric_limits<float>::quiet_NaN(),
                                 BatteryStatus::Low };
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
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::name);
    return (characteristic.isValid()) ? QString::fromUtf8(characteristic.value()) : QString();
}

/*!
 * Set's the Pokit device's name to \a name.
 *
 * Returns `true` if the write request was successfully queued, `false` otherwise.
 *
 * Emits deviceNameWritten() if/when the \a name has been set.
 */
bool StatusService::setDeviceName(const QString &name)
{
    Q_D(const StatusService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::name);
    if (!characteristic.isValid()) {
        return false;
    }

    const QByteArray value = name.toUtf8();
    if (value.length() > 11) {
        qCWarning(d->lc).noquote() << tr("Device name \"%1\" is too long (%2 > 11 bytes): 0x3")
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
 * \note This operation is only supported by Pokit Meter devices. Pokit Pro devices will report an
 * Bluetooth ATT error `0x80`.
 *
 * \cond internal
 * \pokitApi The Android app can turn Pokit Pro LEDs on/off. Perhaps that is handled by an
 * undocumented use of this characteristic. Or perhaps its via some other service.
 * \endcond
 */
bool StatusService::flashLed()
{
    Q_D(const StatusService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::flashLed);
    if (!characteristic.isValid()) {
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
 * \fn StatusService::deviceNameWritten
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
    : AbstractPokitServicePrivate(QBluetoothUuid(), controller, q)
{

}

/*!
 * Parses the `Device Characteristics` \a value into a DeviceCharacteristics struct.
 */
StatusService::DeviceCharacteristics StatusServicePrivate::parseDeviceCharacteristics(
    const QByteArray &value)
{
    StatusService::DeviceCharacteristics characteristics{
        QVersionNumber(), 0, 0, 0, 0, 0, 0, QBluetoothAddress()
    };
    Q_ASSERT(characteristics.firmwareVersion.isNull());  // How we indicate failure.

    if (!checkSize(QLatin1String("Device Characterisitcs"), value, 20, 20)) {
        return characteristics;
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

    qCDebug(lc).noquote() << tr("Firmware version:     ") << characteristics.firmwareVersion;
    qCDebug(lc).noquote() << tr("Maximum voltage:      ") << characteristics.maximumVoltage;
    qCDebug(lc).noquote() << tr("Maximum current:      ") << characteristics.maximumCurrent;
    qCDebug(lc).noquote() << tr("Maximum resistance:   ") << characteristics.maximumResistance;
    qCDebug(lc).noquote() << tr("Maximum sampling rate:") << characteristics.maximumSamplingRate;
    qCDebug(lc).noquote() << tr("Sampling buffer size: ") << characteristics.samplingBufferSize;
    qCDebug(lc).noquote() << tr("Capability mask:      ") << characteristics.capabilityMask;
    qCDebug(lc).noquote() << tr("MAC address:          ") << characteristics.macAddress;

    Q_ASSERT(!characteristics.firmwareVersion.isNull()); // How we indicate success.
    return characteristics;
}

/*!
 * Parses the `Status` \a value into Statu struct. Note, not all Pokit devices support all members
 * in Status. Specifically, the batteryStatus member is not usually set by Pokit Meter devices, so
 * will be an invlalid BatteryStatus enum value (`255`) in that case.
 */
StatusService::Status StatusServicePrivate::parseStatus(const QByteArray &value)
{
    StatusService::Status status{
        static_cast<StatusService::DeviceStatus>
            (std::numeric_limits<std::underlying_type<StatusService::DeviceStatus>::type>::max()),
        std::numeric_limits<float>::quiet_NaN(),
        static_cast<StatusService::BatteryStatus>
            (std::numeric_limits<std::underlying_type<StatusService::BatteryStatus>::type>::max()),
    };

    /*!
     * \pokitApi Pokit API 0.02 says the `Status` characteristic is 5 bytes. API 1.00 then added an
     * additional byte for `Battery Status`, for 6 bytes in total. However, Pokit Pro devices return
     * 8 bytes here. The purpose of those last 2 bytes are not currently known. Note also, Pokit
     * Meter only uses the first 5 bytes - ie `Battery Status` is not present.
     */

    if (!checkSize(QLatin1String("Status"), value, 5, 6)) {
        return status;
    }

    status.deviceStatus = static_cast<StatusService::DeviceStatus>(value.at(0));
    status.batteryVoltage = qFromLittleEndian<float>(value.mid(1,4));
    if (value.size() >= 6) { // Battery Status added to Pokit API docs v1.00.
        status.batteryStatus = static_cast<StatusService::BatteryStatus>(value.at(5));
    }
    qCDebug(lc).noquote() << tr("Device status:   %1 (%2)")
        .arg((quint8)status.deviceStatus).arg(StatusService::toString(status.deviceStatus));
    qCDebug(lc).noquote() << tr("Battery voltage: %1 volts").arg(status.batteryVoltage);
    qCDebug(lc).noquote() << tr("Battery status:  %1 (%2)")
        .arg((quint8)status.batteryStatus).arg(StatusService::toString(status.batteryStatus));
    return status;
}

/*!
 * Handles `QLowEnergyController::serviceDiscovered` events.
 *
 * Here we override the base implementation to detect if we're looking at a Pokit Meter, or Pokit
 * Pro device, as the two devices have very slightly different Status Service UUIDs.
 */
void StatusServicePrivate::serviceDiscovered(const QBluetoothUuid &newService)
{
    if (newService == StatusService::ServiceUuids::pokitMeter) {
        qCDebug(lc).noquote() << tr("Found Status Service for a Pokit Meter device.");
        serviceUuid = StatusService::ServiceUuids::pokitMeter;
    } else if (newService == StatusService::ServiceUuids::pokitPro) {
        qCDebug(lc).noquote() << tr("Found Status Service for a Pokit Pro device.");
        serviceUuid = StatusService::ServiceUuids::pokitPro;
    }
    AbstractPokitServicePrivate::serviceDiscovered(newService);
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicRead to parse \a value, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void StatusServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    AbstractPokitServicePrivate::characteristicRead(characteristic, value);

    Q_Q(StatusService);
    if (characteristic.uuid() == StatusService::CharacteristicUuids::deviceCharacteristics) {
        emit q->deviceCharacteristicsRead(parseDeviceCharacteristics(value));
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::status) {
        emit q->deviceStatusRead(parseStatus(value));
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::name) {
        const QString deviceName = QString::fromUtf8(value);
        qCDebug(lc).noquote() << tr("Device name: \"%1\"").arg(deviceName);
        emit q->deviceNameRead(deviceName);
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::flashLed) {
        qCWarning(lc).noquote() << tr("Flash LED characteristic is write-only, but somehow read")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic read for Status service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void StatusServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                 const QByteArray &newValue)
{
    AbstractPokitServicePrivate::characteristicWritten(characteristic, newValue);

    Q_Q(StatusService);
    if (characteristic.uuid() == StatusService::CharacteristicUuids::deviceCharacteristics) {
        qCWarning(lc).noquote() << tr("Device Characteristics is read-only, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::status) {
        qCWarning(lc).noquote() << tr("Status characteristic is read-only, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::name) {
        emit q->deviceNameWritten();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::flashLed) {
        emit q->deviceLedFlashed();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic written for Status service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
