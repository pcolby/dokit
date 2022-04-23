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
 * Defines the DeviceInfoService and DeviceInfoServicePrivate classes.
 */

#include <qtpokit/deviceinfoservice.h>
#include "deviceinfoservice_p.h"
#include "logging_p.h"

#include <QtEndian>

/*!
 * \class DeviceInfoService
 *
 * The DeviceInfoService class accesses the `Device Info` service of Pokit devices.
 */

/// UUID of the "Device Info" service.
const QBluetoothUuid DeviceInfoService::
    serviceUuid(QBluetoothUuid::ServiceClassUuid::DeviceInformation);

/// \struct DeviceInfoService::CharacteristicUuids
/// \brief Characteristics available via the `Device Info` service.

/// UUID of the `Device Info` service's `xxx` characterstic.
const QBluetoothUuid DeviceInfoService::CharacteristicUuids::
    manufacturerName(QBluetoothUuid::CharacteristicType::ManufacturerNameString);

/// UUID of the `Device Info` service's `xxx` characterstic.
const QBluetoothUuid DeviceInfoService::CharacteristicUuids::
    modelNumber(QBluetoothUuid::CharacteristicType::ModelNumberString);

/// UUID of the `Device Info` service's `xxx` characterstic.
const QBluetoothUuid DeviceInfoService::CharacteristicUuids::
    hardwareRevision(QBluetoothUuid::CharacteristicType::HardwareRevisionString);

/// UUID of the `Device Info` service's `xxx` characterstic.
const QBluetoothUuid DeviceInfoService::CharacteristicUuids::
    firmwareRevision(QBluetoothUuid::CharacteristicType::FirmwareRevisionString);

/// UUID of the `Device Info` service's `xxx` characterstic.
const QBluetoothUuid DeviceInfoService::CharacteristicUuids::
    softwareRevision(QBluetoothUuid::CharacteristicType::SoftwareRevisionString);

/*!
 * Constructs a new Pokit service with \a parent.
 */
DeviceInfoService::DeviceInfoService(QLowEnergyController * const controller, QObject * parent)
    : AbstractPokitService(new DeviceInfoServicePrivate(controller, this), parent)
{

}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
DeviceInfoService::DeviceInfoService(
    DeviceInfoServicePrivate * const d, QObject * const parent)
    : AbstractPokitService(d, parent)
{

}
/// \endcond

/*!
 * Destroys this DeviceInfoService object.
 */
DeviceInfoService::~DeviceInfoService()
{
    delete d_ptr;
}

bool DeviceInfoService::readCharacteristics()
{
    const bool r1 = readFirmwareRevisionCharacteristic();
    const bool r2 = readHardwareRevisionCharacteristic();
    const bool r3 = readSoftwareRevisionCharacteristic();
    const bool r4 = readManufacturerCharacteristics();
    const bool r5 = readModelNumberCharacteristic();
    return (r1 && r2 && r3 && r4 && r5);
}

/*!
 * Read the `Device Info` service's `Firmware Revision` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits firmwareRevisionRead() if/when the characteristic has been read successfully.
 */
bool DeviceInfoService::readFirmwareRevisionCharacteristic()
{
    Q_D(DeviceInfoService);
    return d->readCharacteristic(CharacteristicUuids::firmwareRevision);
}

/*!
 * Read the `Device Info` service's `Hardware Revision` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits hardwareRevisionRead() if/when the characteristic has been read successfully.
 */
bool DeviceInfoService::readHardwareRevisionCharacteristic()
{
    Q_D(DeviceInfoService);
    return d->readCharacteristic(CharacteristicUuids::hardwareRevision);
}

/*!
 * Read the `Device Info` service's `Manufacturer Name` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits manufacturerNameRead() if/when the characteristic has been read successfully.
 */
bool DeviceInfoService::readManufacturerCharacteristics()
{
    Q_D(DeviceInfoService);
    return d->readCharacteristic(CharacteristicUuids::manufacturerName);
}

/*!
 * Read the `Device Info` service's `Model Number` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits modelNumberRead() if/when the characteristic has been read successfully.
 */
bool DeviceInfoService::readModelNumberCharacteristic()
{
    Q_D(DeviceInfoService);
    return d->readCharacteristic(CharacteristicUuids::modelNumber);
}

/*!
 * Read the `Device Info` service's `Software Revision` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits softwareRevisionRead() if/when the characteristic has been read successfully.
 */
bool DeviceInfoService::readSoftwareRevisionCharacteristic()
{
    Q_D(DeviceInfoService);
    return d->readCharacteristic(CharacteristicUuids::softwareRevision);
}

/*!
 * Returns the most recent value of the `Device Info` service's `Manufacturer Name` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then a
 * null QString is returned.
 */
QString DeviceInfoService::manufacturer() const
{
    Q_D(const DeviceInfoService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("No device characteristics without a service object.");
        return QString();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::manufacturerName);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Manufacturer Name characteristic not valid yet.");
        return QString();
    }

    return QString::fromUtf8(characteristic.value());
}

/*!
 * Returns the most recent value of the `Device Info` service's `Model Number` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then a
 * null QString is returned.
 */
QString DeviceInfoService::modelNumber() const
{
    Q_D(const DeviceInfoService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("No device characteristics without a service object.");
        return QString();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::modelNumber);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Model Number characteristic not valid yet.");
        return QString();
    }

    return QString::fromUtf8(characteristic.value());
}

/*!
 * Returns the most recent value of the `Device Info` service's `Hardware Revision` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then a
 * null QString is returned.
 */
QString DeviceInfoService::hardwareRevision() const
{
    Q_D(const DeviceInfoService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("No device characteristics without a service object.");
        return QString();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::hardwareRevision);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Hardware Revision characteristic not valid yet.");
        return QString();
    }

    return QString::fromUtf8(characteristic.value());
}

/*!
 * Returns the most recent value of the `Device Info` service's `Firmware Revision` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then a
 * null QString is returned.
 */
QString DeviceInfoService::firmwareRevision() const
{
    Q_D(const DeviceInfoService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("No device characteristics without a service object.");
        return QString();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::firmwareRevision);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Firmware Revision characteristic not valid yet.");
        return QString();
    }

    return QString::fromUtf8(characteristic.value());
}

/*!
 * Returns the most recent value of the `Device Info` service's `Software Revision` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then a
 * null QString is returned.
 */
QString DeviceInfoService::softwareRevision() const
{
    Q_D(const DeviceInfoService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("No device characteristics without a service object.");
        return QString();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::softwareRevision);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Software Revision characteristic not valid yet.");
        return QString();
    }

    return QString::fromUtf8(characteristic.value());
}

/*!
 * \fn DeviceInfoService::manufacturerRead
 *
 * This signal is emitted when the `Manufacturer Name` characteristic has been read successfully.
 *
 * \see readManufacturerCharacteristic
 * \see manufacturer
 */

/*!
 * \fn DeviceInfoService::modelNumberRead
 *
 * This signal is emitted when the `Model Number` characteristic has been read successfully.
 *
 * \see readModelNumberCharacteristic
 * \see modelNumber
 */

/*!
 * \fn DeviceInfoService::hardwareRevisionRead
 *
 * This signal is emitted when the `Hardware Revision` characteristic has been read successfully.
 *
 * \see readHardwareRevisionCharacteristic
 * \see hardwareRevision
 */

/*!
 * \fn DeviceInfoService::firmwareRevisionRead
 *
 * This signal is emitted when the `Firmware Revision` characteristic has been read successfully.
 *
 * \see readFirmwareRevisionCharacteristic
 * \see firmwareRevision
 */

/*!
 * \fn DeviceInfoService::softwareRevisionRead
 *
 * This signal is emitted when the `Software Revision` characteristic has been read successfully.
 *
 * \see readSoftwareRevisionCharacteristic
 * \see softwareRevision
 */

/*!
 * \cond internal
 * \class DeviceInfoServicePrivate
 *
 * The DeviceInfoServicePrivate class provides private implementation for DeviceInfoService.
 */

/*!
 * \internal
 * Constructs a new DeviceInfoServicePrivate object with public implementation \a q.
 */
DeviceInfoServicePrivate::DeviceInfoServicePrivate(
    QLowEnergyController * controller, DeviceInfoService * const q)
    : AbstractPokitServicePrivate(DeviceInfoService::serviceUuid, controller, q)
{

}

/*!
 * Implements AbstractPokitServicePrivate::characteristicRead to parse \a value, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void DeviceInfoServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    qCDebug(pokitService).noquote() << tr("Read  characteristic \"%1\" (%2) of size %3:")
        .arg(characteristic.name(), characteristic.uuid().toString()).arg(value.size()) << value;
    Q_Q(DeviceInfoService);

    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::manufacturerName) {
        const QString name = QString::fromUtf8(value);
        qCDebug(pokitService).noquote() << tr("Manufacturer name: \"%1\"").arg(name);
        emit q->manufacturerRead(name);
        return;
    }

    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::modelNumber) {
        const QString model = QString::fromUtf8(value);
        qCDebug(pokitService).noquote() << tr("Model number: \"%1\"").arg(model);
        emit q->modelNumberRead(model);
        return;
    }

    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::hardwareRevision) {
        const QString revision = QString::fromUtf8(value);
        qCDebug(pokitService).noquote() << tr("Hardware revision: \"%1\"").arg(revision);
        emit q->hardwareRevisionRead(revision);
        return;
    }

    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::firmwareRevision) {
        const QString revision = QString::fromUtf8(value);
        qCDebug(pokitService).noquote() << tr("Firmware revision: \"%1\"").arg(revision);
        emit q->firmwareRevisionRead(revision);
        return;
    }

    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::softwareRevision) {
        const QString revision = QString::fromUtf8(value);
        qCDebug(pokitService).noquote() << tr("Software revision: \"%1\"").arg(revision);
        emit q->softwareRevisionRead(revision);
        return;
    }

    qCWarning(pokitService).noquote() << tr("Unknown characteristic read for Device Info service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void DeviceInfoServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                 const QByteArray &newValue)
{
    Q_UNUSED(newValue);
    qCWarning(pokitService).noquote() << tr("Characteristic written event on read-only service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
