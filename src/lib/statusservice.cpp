// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
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
 * \cond internal
 * \struct StatusService::ServiceUuids
 * \pokitApi Pokit API 1.00 (and 0.02) states the Status Service UUID as
 * `57d3a771-267c-4394-8872-78223e92aec4` which is correct for the Pokit Meter, but Pokit Pro uses
 * `57d3a771-267c-4394-8872-78223e92aec5` instead, that is the last digit is a `5` not `4`.
 * \endcond
 */

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
 * \cond internal
 * \enum StatusService::SwitchPosition
 * \pokitApi These enum values are undocumented, but easily testable with a physical Pokit Pro device.
 * Internally, Pokit's Android app calls these: `SWITCH_MODE_VOLTAGE`, `SWITCH_MODE_ALL` and `SWITCH_MODE_CURRENT`.
 * \endcond
 */

/*!
 * Returns a string version of the \a position enum label.
 */
QString StatusService::toString(const StatusService::SwitchPosition &position)
{
    switch (position) {
    case SwitchPosition::Voltage:     return QLatin1String("Voltage");
    case SwitchPosition::MultiMode:   return QLatin1String("MultiMode");
    case SwitchPosition::HighCurrent: return QLatin1String("HighCurrent");
    }
    return QString();
}

/*!
 * Returns a string version of the \a status enum label.
 */
QString StatusService::toString(const StatusService::ChargingStatus &status)
{
    switch (status) {
    case ChargingStatus::Discharging: return QLatin1String("Discharging");
    case ChargingStatus::Charging:    return QLatin1String("Charging");
    case ChargingStatus::Charged:     return QLatin1String("Charged");
    }
    return QString();
}

/*!
 * Returns a string version of the \a status enum label.
 */
QString StatusService::toString(const StatusService::TorchStatus &status)
{
    switch (status) {
    case TorchStatus::Off: return QLatin1String("Off");
    case TorchStatus::On:  return QLatin1String("On");
    }
    return QString();
}

/*!
 * Returns a string version of the \a status enum label.
 */
QString StatusService::toString(const StatusService::ButtonStatus &status)
{
    switch (status) {
    case ButtonStatus::Released: return QLatin1String("Released");
    case ButtonStatus::Pressed:  return QLatin1String("Pressed");
    case ButtonStatus::Held:     return QLatin1String("Held");
    }
    return QString();
}

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
    const bool r4 = ((service() != nullptr) && (service()->characteristic(CharacteristicUuids::torch).isValid()))
        ? readTorchCharacteristic() : true;
    const bool r5 = ((service() != nullptr) && (service()->characteristic(CharacteristicUuids::buttonPress).isValid()))
        ? readButtonPressCharacteristic() : true;
    return (r1 && r2 && r3 && r4 && r5);
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
 * Read the `Status` service's (undocumented) `Torch` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits torchStatusRead() if/when the characteristic has been read successfully.
 */
bool StatusService::readTorchCharacteristic()
{
    Q_D(StatusService);
    return d->readCharacteristic(CharacteristicUuids::torch);
}

/*!
 * Read the `Status` service's (undocumented) `Button Press` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits buttonPressRead() if/when the characteristic has been read successfully.
 */
bool StatusService::readButtonPressCharacteristic()
{
    Q_D(StatusService);
    return d->readCharacteristic(CharacteristicUuids::buttonPress);
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
 *
 * Not all Pokit devices support the Status::batteryStatus member, in which case the member will be
 * initilialised to the maximum value supported by the underlying type (ie `255`) to indicate "not set"
 */
StatusService::Status StatusService::status() const
{
    Q_D(const StatusService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::status);
    return (characteristic.isValid()) ? StatusServicePrivate::parseStatus(characteristic.value())
        : StatusService::Status{ DeviceStatus::Idle, std::numeric_limits<float>::quiet_NaN(),
                                 BatteryStatus::Low, std::nullopt, std::nullopt };
}

/*!
 * Enables client-side notifications of device status changes.
 *
 * This is an alternative to manually requesting individual reads via readStatusCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 *
 * Successfully read values (if any) will be emitted via the deviceStatusRead() signal.
 */
bool StatusService::enableStatusNotifications()
{
    Q_D(StatusService);
    return d->enableCharacteristicNotificatons(CharacteristicUuids::status);
}

/*!
 * Disables client-side notifications of device status changes.
 *
 * Instantaneous status can still be fetched by readStatusCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool StatusService::disableStatusNotifications()
{
    Q_D(StatusService);
    return d->disableCharacteristicNotificatons(CharacteristicUuids::status);
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
        qCWarning(d->lc).noquote() << tr(R"(Device name "%1" is too long (%2 > 11 bytes): 0x3)")
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
 * Returns the most recent value of the `Status` services's `Torch` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (eg if the device does not support the Torch characteristic), then `nullopt`
 * is returned.
 */
std::optional<StatusService::TorchStatus> StatusService::torchStatus() const
{
    Q_D(const StatusService);
    const QLowEnergyCharacteristic characteristic = d->getCharacteristic(CharacteristicUuids::torch);
    return (characteristic.isValid()) ? StatusServicePrivate::parseTorchStatus(characteristic.value()) : std::nullopt;
}

/*!
 * Set the Pokit device's torch to \a status.
 *
 * Returns `true` if the request was successfully queued, `false` otherwise.
 *
 * Emits torchStatusWritten() if/when the LED has flashed successfully.
 *
 * \note This operation is only supported by Pokit Pro devices, and not Pokit Meter devices.
 */
bool StatusService::setTorchStatus(const StatusService::TorchStatus status)
{
    Q_D(const StatusService);
    const QLowEnergyCharacteristic characteristic = d->getCharacteristic(CharacteristicUuids::torch);
    if (!characteristic.isValid()) {
        return false;
    }

    const QByteArray value(1, static_cast<char>(status));
    d->service->writeCharacteristic(characteristic, value);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}

/*!
 * Enables client-side notifications of torch status changes.
 *
 * This is an alternative to manually requesting individual reads via readTorchCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 *
 * Successfully read values (if any) will be emitted via the torchStatusRead() signal.
 */
bool StatusService::enableTorchStatusNotifications()
{
    Q_D(StatusService);
    return d->enableCharacteristicNotificatons(CharacteristicUuids::torch);
}

/*!
 * Disables client-side notifications of torch status changes.
 *
 * Instantaneous torch status can still be fetched by readTorchCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool StatusService::disableTorchStatusNotifications()
{
    Q_D(StatusService);
    return d->disableCharacteristicNotificatons(CharacteristicUuids::torch);
}

/*!
 * Enables client-side notifications of button presses.
 *
 * This is an alternative to manually requesting individual reads via readButtonPressCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 *
 * Successfully read values (if any) will be emitted via the torchStatusRead() signal.
 */
bool StatusService::enableButtonPressedNotifications()
{
    Q_D(StatusService);
    return d->enableCharacteristicNotificatons(CharacteristicUuids::buttonPress);
}

/*!
 * Disables client-side notifications of button presses.
 *
 * Instantaneous button press statussed can still be fetched by readButtonPressCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool StatusService::disableButtonPressedNotifications()
{
    Q_D(StatusService);
    return d->disableCharacteristicNotificatons(CharacteristicUuids::buttonPress);
}

/*!
 * Returns the most recent value of the `Status` services's `Button Press` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (eg if the device does not support the Torch characteristic), then `nullopt`
 * is returned.
 */
std::optional<StatusService::ButtonStatus> StatusService::buttonPress() const
{
    Q_D(const StatusService);
    const QLowEnergyCharacteristic characteristic = d->getCharacteristic(CharacteristicUuids::buttonPress);
    return (characteristic.isValid()) ? StatusServicePrivate::parseButtonPress(characteristic.value()) : std::nullopt;
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
 * \fn StatusService::torchStatusRead
 *
 * This signal is emitted when the `Torch` characteristic has been read successfully.
 *
 * \see setTorchStatus
 */

/*!
 * \fn StatusService::torchStatusWritten
 *
 * This signal is emitted when the `Torch` characteristic has been written successfully.
 *
 * \see readTorchCharacteristic
 */

/*!
 * \fn StatusService::buttonPressRead
 *
 * This signal is emitted when the `Button Press` characteristic has been read successfully.
 *
 * \see readButtonPressCharacteristic
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
                                          qFromLittleEndian<quint8 >(value.mid(0,1).constData()),
                                          qFromLittleEndian<quint8 >(value.mid(1,1).constData()));
    characteristics.maximumVoltage      = qFromLittleEndian<quint16>(value.mid(2,2).constData());
    characteristics.maximumCurrent      = qFromLittleEndian<quint16>(value.mid(4,2).constData());
    characteristics.maximumResistance   = qFromLittleEndian<quint16>(value.mid(6,2).constData());
    characteristics.maximumSamplingRate = qFromLittleEndian<quint16>(value.mid(8,2).constData());
    characteristics.samplingBufferSize  = qFromLittleEndian<quint16>(value.mid(10,2).constData());
    characteristics.capabilityMask      = qFromLittleEndian<quint16>(value.mid(12,2).constData());
    characteristics.macAddress = QBluetoothAddress(qFromBigEndian<quint64>
                                                   ((QByteArray(2, '\0') + value.mid(14,6)).constData()));

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
 * Parses the `Status` \a value into a Status struct. Note, not all Pokit devices support all members
 * in Status. Specifically, the batteryStatus member is not usually set by Pokit Meter devices, so
 * will be an invlalid BatteryStatus enum value (`255`) in that case.
 */
StatusService::Status StatusServicePrivate::parseStatus(const QByteArray &value)
{
    StatusService::Status status{
        static_cast<StatusService::DeviceStatus>
            (std::numeric_limits<std::underlying_type_t<StatusService::DeviceStatus>>::max()),
        std::numeric_limits<float>::quiet_NaN(),
        static_cast<StatusService::BatteryStatus>
            (std::numeric_limits<std::underlying_type_t<StatusService::BatteryStatus>>::max()),
        std::nullopt, std::nullopt,
    };

    /*!
     * \pokitApi Pokit API 0.02 says the `Status` characteristic is 5 bytes. API 1.00 then added an
     * additional byte for `Battery Status`, for 6 bytes in total. However, Pokit Pro devices return
     * 8 bytes here. It appears that the first of those 2 extra bytes (ie the 7th byte) is used to
     * indicate the physical switch position, while the other extra byte (ie the 8th byte) indicates
     * the device's current charging status.
     */

    if (!checkSize(QLatin1String("Status"), value, 5, 8)) {
        return status;
    }

    status.deviceStatus = static_cast<StatusService::DeviceStatus>(value.at(0));
    status.batteryVoltage = qFromLittleEndian<float>(value.mid(1,4).constData());
    if (value.size() >= 6) { // Battery Status added to Pokit API docs v1.00.
        status.batteryStatus = static_cast<StatusService::BatteryStatus>(value.at(5));
    }
    if (value.size() >= 7) { // Switch Position - as yet, undocumented by Pokit Innovations.
        status.switchPosition = static_cast<StatusService::SwitchPosition>(value.at(6));
    }
    if (value.size() >= 8) { // Charging Status - as yet, undocumented by Pokit Innovations.
        status.chargingStatus = static_cast<StatusService::ChargingStatus>(value.at(7));
    }
    qCDebug(lc).noquote() << tr("Device status:   %1 (%2)")
        .arg((quint8)status.deviceStatus).arg(StatusService::toString(status.deviceStatus));
    qCDebug(lc).noquote() << tr("Battery voltage: %1 volts").arg(status.batteryVoltage);
    qCDebug(lc).noquote() << tr("Battery status:  %1 (%2)")
        .arg((quint8)status.batteryStatus).arg(StatusService::toString(status.batteryStatus));
    if (status.switchPosition) {
        qCDebug(lc).noquote() << tr("Switch position: %1 (%2)")
            .arg((quint8)*status.switchPosition).arg(StatusService::toString(*status.switchPosition));
    }
    if (status.chargingStatus) {
        qCDebug(lc).noquote() << tr("Charging status: %1 (%2)")
        .arg((quint8)*status.chargingStatus).arg(StatusService::toString(*status.chargingStatus));
    }
    return status;
}

/*!
 * Parses the torch status \a value, and returns the corresponding TorchStatus.
 */
std::optional<StatusService::TorchStatus> StatusServicePrivate::parseTorchStatus(const QByteArray &value)
{
    if (!checkSize(QLatin1String("Torch"), value, 1, 1)) {
        return std::nullopt;
    }

    const StatusService::TorchStatus status = static_cast<StatusService::TorchStatus>(value.at(0));
    qCDebug(lc).noquote() << tr("Torch status: %1 (%2)").arg((quint8)status).arg(StatusService::toString(status));
    return status;
}

/*!
 * Parses the button press \a value, and returns the corresponding ButtonStatus.
 */
std::optional<StatusService::ButtonStatus> StatusServicePrivate::parseButtonPress(const QByteArray &value)
{
    if (!checkSize(QLatin1String("Torch"), value, 2, 2)) {
        return std::nullopt;
    }

    /*!
     * \pokitApi The button event is the second byte, but no idea what the first byte is. In all examples
     * I've see it's always `0x02`. It appears that the Pokit Android app only ever looks at `bytes[1]`.
     *
     * \pokitApi Note, we can actually write to the Button Press characteristic too. If we do, then whatever
     * we set as the first byte persists, and (unsurprisingly) the second byte reverts to the current
     * button state. So still no idea what that first byte is for.
     */

    const StatusService::ButtonStatus status = static_cast<StatusService::ButtonStatus>(value.at(1));
    qCDebug(lc).noquote() << tr("Button: %1 (%2)").arg((quint8)status).arg(StatusService::toString(status));
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
        Q_EMIT q->deviceCharacteristicsRead(parseDeviceCharacteristics(value));
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::status) {
        Q_EMIT q->deviceStatusRead(parseStatus(value));
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::name) {
        const QString deviceName = QString::fromUtf8(value);
        qCDebug(lc).noquote() << tr(R"(Device name: "%1")").arg(deviceName);
        Q_EMIT q->deviceNameRead(deviceName);
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::flashLed) {
        qCWarning(lc).noquote() << tr("Flash LED characteristic is write-only, but somehow read")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::torch) {
        if (!checkSize(QLatin1String("Torch"), value, 1, 1)) {
            return;
        }
        const StatusService::TorchStatus status = static_cast<StatusService::TorchStatus>(value.at(0));
        qCDebug(lc).noquote() << tr("Torch status:  %1 (%2)").arg((quint8)status).arg(StatusService::toString(status));
        Q_EMIT q->torchStatusRead(status);
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::buttonPress) {
        if (!checkSize(QLatin1String("Torch"), value, 2, 2)) {
            return;
        }
        const StatusService::ButtonStatus status = static_cast<StatusService::ButtonStatus>(value.at(1));
        qCDebug(lc).noquote() << tr("Button status:  %1 (%2)").arg((quint8)status).arg(StatusService::toString(status));
        Q_EMIT q->buttonPressRead(value.at(0), status);
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
        Q_EMIT q->deviceNameWritten();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::flashLed) {
        Q_EMIT q->deviceLedFlashed();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::torch) {
        Q_EMIT q->torchStatusWritten();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic written for Status service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
