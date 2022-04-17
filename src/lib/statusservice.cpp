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

#include <QDebug>
#include <QtEndian>

/// UUID of the "Pokit Status" service.
const QBluetoothUuid StatusService::
    serviceUuid(QLatin1String("57d3a771-267c-4394-8872-78223e92aec5"));

/// UUID of the "Pokit Status" service's "Device Characteristics" charactersic.
const QBluetoothUuid StatusService::CharacteristicUuids::
    deviceCharacteristics(QLatin1String("6974f5e5-0e54-45c3-97dd-29e4b5fb0849"));

/// UUID of the "Pokit Status" service's "Status" charactersic.
const QBluetoothUuid StatusService::CharacteristicUuids::
    status(QLatin1String("3dba36e1-6120-4706-8dfd-ed9c16e569b6"));

/// UUID of the "Pokit Status" service's "Device Name" charactersic.
const QBluetoothUuid StatusService::CharacteristicUuids::
    name(QLatin1String("7f0375de-077e-4555-8f78-800494509cc3"));

/// UUID of the "Pokit Status" service's "Flash LED" charactersic.
const QBluetoothUuid StatusService::CharacteristicUuids::
    flashLed(QLatin1String("ec9bb1f3-05a9-4277-8dd0-60a7896f0d6e"));

/*!
 * \class StatusService
 *
 * Virtual base class for Pokit services.
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
    delete d_ptr;
}

bool StatusService::readCharacteristics()
{
    const bool r1 = readDeviceCharacteristics();
    const bool r2 = readStatusCharacteristic();
    const bool r3 = readNameCharacteristic();
    return (r1 && r2 && r3);
}

bool StatusService::readDeviceCharacteristics()
{
    Q_D(StatusService);
    return d->readCharacteristic(CharacteristicUuids::deviceCharacteristics);
}

bool StatusService::readStatusCharacteristic()
{
    Q_D(StatusService);
    return d->readCharacteristic(CharacteristicUuids::status);
}

bool StatusService::readNameCharacteristic()
{
    Q_D(StatusService);
    return d->readCharacteristic(CharacteristicUuids::name);
}

StatusService::DeviceCharacteristics StatusService::deviceCharacteristics() const
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService) << "No device characteristics without a service object";
        return StatusService::DeviceCharacteristics();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::deviceCharacteristics);
    if (!characteristic.isValid()) {
        qCDebug(pokitService) << "Device characteristics not valid yet";
        return StatusService::DeviceCharacteristics();
    }

    return StatusServicePrivate::parseDeviceCharacteristics(characteristic.value());
}

StatusService::DeviceStatus StatusService::deviceStatus() const
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService) << "No device status without a service object";
        return static_cast<StatusService::DeviceStatus>(0xFF);
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::status);
    if (!characteristic.isValid()) {
        qCDebug(pokitService) << "Status characteristic not valid yet";
        return static_cast<StatusService::DeviceStatus>(0xFF);
    }

    return StatusServicePrivate::parseStatus(characteristic.value()).first;
}

float StatusService::batteryVoltage() const
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService) << "No device status without a service object";
        return std::numeric_limits<float>::quiet_NaN();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::status);
    if (!characteristic.isValid()) {
        qCDebug(pokitService) << "Status characteristic not valid yet";
        return std::numeric_limits<float>::quiet_NaN();
    }

    return StatusServicePrivate::parseStatus(characteristic.value()).second;
}

QString StatusService::deviceName() const
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService) << "No device name without a service object";
        return QString();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::status);
    if (!characteristic.isValid()) {
        qCDebug(pokitService) << "Name characteristic not valid yet";
        return QString();
    }

    return QString::fromUtf8(characteristic.value());
}

bool StatusService::setDeviceName(const QString &name)
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService) << "Cannot set device name without a service object";
        return false;
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::name);
    if (!characteristic.isValid()) {
        qCDebug(pokitService) << "Name characteristic not valid yet";
        return false;
    }

    const QByteArray value = name.toUtf8();
    if (value.size() > 11) {
        qCWarning(pokitService) << "New device name cannot exceed 11 bytes" << name << value;
        return false;
    }

    d->service->writeCharacteristic(characteristic, value);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}

/// \todo Currently doesn't work (ATT error 0x80). Follow up with Pokit Innovations?
bool StatusService::flashLed()
{
    Q_D(const StatusService);
    if (!d->service) {
        qCDebug(pokitService) << "Cannot flash LED without a service object";
        return false;
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::flashLed);
    if (!characteristic.isValid()) {
        qCDebug(pokitService) << "Flash LED characteristic not valid yet";
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
 * \cond internal
 * \class StatusServicePrivate
 *
 * The StatusServicePrivate provides the private implementation for StatusService.
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

StatusService::DeviceCharacteristics StatusServicePrivate::parseDeviceCharacteristics(
    const QByteArray &value)
{
    StatusService::DeviceCharacteristics characteristics;
    Q_ASSERT(characteristics.firmwareVersion.isNull());  // How we indicate failure.

    if (value.size() < 20) {
        qCWarning(pokitService) << "Invalid characteristics size" << value.size() << value;
        return characteristics;
    }
    if (value.size() > 20) {
        qCWarning(pokitService) << "Characterisitcs has" << (value.size()-20) << "extra bytes" << value;
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

    qCDebug(pokitService) << "Firmware version:     " << characteristics.firmwareVersion;
    qCDebug(pokitService) << "Maximum voltage:      " << characteristics.maximumVoltage;
    qCDebug(pokitService) << "Maximum current:      " << characteristics.maximumCurrent;
    qCDebug(pokitService) << "Maximum resistance:   " << characteristics.maximumResistance;
    qCDebug(pokitService) << "Maximum sampling rate:" << characteristics.maximumSamplingRate;
    qCDebug(pokitService) << "Sampling buffer size: " << characteristics.samplingBufferSize;
    qCDebug(pokitService) << "Capability mask:      " << characteristics.capabilityMask;
    qCDebug(pokitService) << "MAC address:          " << characteristics.macAddress;

    Q_ASSERT(!characteristics.firmwareVersion.isNull()); // How we indicate success.
    return characteristics;
}

QPair<StatusService::DeviceStatus, float> StatusServicePrivate::parseStatus(const QByteArray &value)
{
    if (value.size() < 5) {
        qCWarning(pokitService) << "Invalid status size" << value.size() << value;
        return QPair(StatusService::DeviceStatus::Idle, std::numeric_limits<float>::quiet_NaN());
    }
    if (value.size() > 5) {
        qCWarning(pokitService) << "Status has" << (value.size()-5) << "extra bytes" << value;
    }
    const StatusService::DeviceStatus status = static_cast<StatusService::DeviceStatus>(value.at(0));
    const float batteryVoltage = qFromLittleEndian<float>(value.mid(1,4));
    qCDebug(pokitService) << "Status:" << (quint8)status << "Battery:" << batteryVoltage << "volts";
    return QPair(status, batteryVoltage);
}

void StatusServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    qCDebug(pokitService) << "Characteristic read" << characteristic.name()
        << characteristic.uuid() << value.size() << value;

    Q_Q(StatusService);
    if (characteristic.uuid() == StatusService::CharacteristicUuids::deviceCharacteristics) {
        const StatusService::DeviceCharacteristics characteristics = parseDeviceCharacteristics(value);
        if (characteristics.firmwareVersion.isNull()) {
            qCWarning(pokitService) << "Failed to parse device characteristics" << value.size() << value;
            return;
        }
        emit q->deviceCharacteristicsRead(characteristics);
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::status) {
        const auto pair = parseStatus(value);
        if (qIsNaN(pair.second)) {
            qCWarning(pokitService) << "Failed to parse status characteristic" << value.size() << value;
            return;
        }
        emit q->deviceStatusRead(pair.first, pair.second);
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::name) {
        const QString deviceName = QString::fromUtf8(value);
        qCDebug(pokitService) << "Device name" << deviceName;
        emit q->deviceNameRead(deviceName);
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::flashLed) {
        qCWarning(pokitService) << "Flash LED characteristic is write-only, but somehow read"
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(pokitService) << "Unknown characteristic read for Status Service"
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

void StatusServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                 const QByteArray &newValue)
{
    qCDebug(pokitService) << "Characteristic written" << characteristic.name()
        << characteristic.uuid() << newValue;

    Q_Q(StatusService);
    if (characteristic.uuid() == StatusService::CharacteristicUuids::deviceCharacteristics) {
        qCWarning(pokitService) << "Device Characteristics is read-only, but somehow written"
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == StatusService::CharacteristicUuids::status) {
        qCWarning(pokitService) << "Status characteristic is read-only, but somehow written"
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

    qCWarning(pokitService) << "Unknown characteristic written for Status Service"
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
