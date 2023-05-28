// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the DeviceInfoService and DeviceInfoServicePrivate classes.
 */

#include <qtpokit/deviceinfoservice.h>
#include "deviceinfoservice_p.h"

#include <QtEndian>

/*!
 * \class DeviceInfoService
 *
 * The DeviceInfoService class accesses the `Device Info` service of Pokit devices.
 */

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
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::manufacturerName);
    return (characteristic.isValid()) ? QString::fromUtf8(characteristic.value()) : QString();
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
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::modelNumber);
    return (characteristic.isValid()) ? QString::fromUtf8(characteristic.value()) : QString();
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
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::hardwareRevision);
    return (characteristic.isValid()) ? QString::fromUtf8(characteristic.value()) : QString();
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
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::firmwareRevision);
    return (characteristic.isValid()) ? QString::fromUtf8(characteristic.value()) : QString();
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
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::softwareRevision);
    return (characteristic.isValid()) ? QString::fromUtf8(characteristic.value()) : QString();
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
    AbstractPokitServicePrivate::characteristicRead(characteristic, value);

    Q_Q(DeviceInfoService);
    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::manufacturerName) {
        const QString name = QString::fromUtf8(value);
        qCDebug(lc).noquote() << tr(R"(Manufacturer name: "%1")").arg(name);
        emit q->manufacturerRead(name);
        return;
    }

    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::modelNumber) {
        const QString model = QString::fromUtf8(value);
        qCDebug(lc).noquote() << tr(R"(Model number: "%1")").arg(model);
        emit q->modelNumberRead(model);
        return;
    }

    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::hardwareRevision) {
        const QString revision = QString::fromUtf8(value);
        qCDebug(lc).noquote() << tr(R"(Hardware revision: "%1")").arg(revision);
        emit q->hardwareRevisionRead(revision);
        return;
    }

    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::firmwareRevision) {
        const QString revision = QString::fromUtf8(value);
        qCDebug(lc).noquote() << tr(R"(Firmware revision: "%1")").arg(revision);
        emit q->firmwareRevisionRead(revision);
        return;
    }

    if (characteristic.uuid() == DeviceInfoService::CharacteristicUuids::softwareRevision) {
        const QString revision = QString::fromUtf8(value);
        qCDebug(lc).noquote() << tr(R"(Software revision: "%1")").arg(revision);
        emit q->softwareRevisionRead(revision);
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic read for Device Info service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
