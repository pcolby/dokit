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
 * Defines the GenericAccessService and GenericAccessServicePrivate classes.
 */

#include <qtpokit/genericaccessservice.h>
#include "genericaccessservice_p.h"
#include "logging_p.h"

#include <QtEndian>

/*!
 * \class GenericAccessService
 *
 * The GenericAccessService class accesses the `Generic Access` service of Pokit devices.
 */

/// UUID of the "Generic Access" service.
const QBluetoothUuid GenericAccessService::
    serviceUuid(QBluetoothUuid::ServiceClassUuid::GenericAccess);

/// \struct GenericAccessService::CharacteristicUuids
/// \brief Characteristics available via the `Generic Access` service.

/// UUID of the `Generic Access` service's `Device Name` characterstic.
const QBluetoothUuid GenericAccessService::CharacteristicUuids::
    name(QBluetoothUuid::CharacteristicType::DeviceName);

/// UUID of the `Generic Access` service's `Appearance` characterstic.
const QBluetoothUuid GenericAccessService::CharacteristicUuids::
    appearance(QBluetoothUuid::CharacteristicType::Appearance);

/*!
 * Constructs a new Pokit service with \a parent.
 */
GenericAccessService::GenericAccessService(QLowEnergyController * const controller, QObject * parent)
    : AbstractPokitService(new GenericAccessServicePrivate(controller, this), parent)
{

}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
GenericAccessService::GenericAccessService(
    GenericAccessServicePrivate * const d, QObject * const parent)
    : AbstractPokitService(d, parent)
{

}
/// \endcond

/*!
 * Destroys this GenericAccessService object.
 */
GenericAccessService::~GenericAccessService()
{
    delete d_ptr;
}

bool GenericAccessService::readCharacteristics()
{
    const bool r1 = readNameCharacteristic();
    const bool r2 = readAppearanceCharacteristic();
    return (r1 && r2);
}

/*!
 * Read the `Generic Access` service's `Appearance` characteristic.
 *
 * Returns `true` if the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits appearanceRead() if/when the characteristic has been read successfully.
 */
bool GenericAccessService::readAppearanceCharacteristic()
{
    Q_D(GenericAccessService);
    return d->readCharacteristic(CharacteristicUuids::appearance);
}

/*!
 * Read the `Generic Access` service's `Device Name` characteristic.
 *
 * Returns `true` if the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits deviceNameRead() if/when the characteristic has been read successfully.
 */
bool GenericAccessService::readNameCharacteristic()
{
    Q_D(GenericAccessService);
    return d->readCharacteristic(CharacteristicUuids::name);
}

/*!
 * Returns the most recent value of the `Generic Access` services's `Appearance` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), `0xFFFF`
 * is returned.
 *
 * \note Pokit's Bluetooth API suggests the device's `Appearance` will always be 0 aka "Unknown
 * Appearance", so this is probably not a very useful characteristic if you already know you are
 * dealing with a Pokit device.
 */
quint16 GenericAccessService::appearance() const
{
    Q_D(const GenericAccessService);
    if (!d->service) {
        qCDebug(pokitService).noquote() << tr("No device name without a service object.");
        return std::numeric_limits<quint16>::max();
    }

    const QLowEnergyCharacteristic characteristic =
        d->service->characteristic(CharacteristicUuids::appearance);
    if (!characteristic.isValid()) {
        qCDebug(pokitService).noquote() << tr("Appearance characteristic not valid yet.");
        return std::numeric_limits<quint16>::max();
    }

    return d->parseAppearance(characteristic.value());
}

/*!
 * Returns the most recent value of the `Generic Access` services's `Device Name` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then a
 * null QString is returned.
 */
QString GenericAccessService::deviceName() const
{
    Q_D(const GenericAccessService);
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
bool GenericAccessService::setDeviceName(const QString &name)
{
    Q_D(const GenericAccessService);
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
        qCWarning(pokitService).noquote() << tr("Device name \"%1\" is too long (%2 > 11 bytes):")
            .arg(name).arg(value.length()) << value;
        return false;
    }

    d->service->writeCharacteristic(characteristic, value);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}


/*!
 * \fn GenericAccessService::appearanceRead
 *
 * This signal is emitted when the `Appearance` characteristic has been read successfully.
 *
 * \see readAppearanceCharacteristic
 * \see appearance
 */

/*!
 * \fn GenericAccessService::deviceNameRead
 *
 * This signal is emitted when the `Device Name` characteristic has been read successfully.
 *
 * \see readDeviceName
 */

/*!
 * \fn GenericAccessService::deivceNameWritten
 *
 * This signal is emitted when the `Device Name` characteristic has been written successfully.
 *
 * \see setDeviceName
 */

/*!
 * \cond internal
 * \class GenericAccessServicePrivate
 *
 * The GenericAccessServicePrivate class provides private implementation for GenericAccessService.
 */

/*!
 * \internal
 * Constructs a new GenericAccessServicePrivate object with public implementation \a q.
 */
GenericAccessServicePrivate::GenericAccessServicePrivate(
    QLowEnergyController * controller, GenericAccessService * const q)
    : AbstractPokitServicePrivate(GenericAccessService::serviceUuid, controller, q)
{

}

/*!
 * Parses the `Appearance` \a value. Returns `0xFFFF` if not valid.
 */
quint16 GenericAccessServicePrivate::parseAppearance(const QByteArray &value)
{
    if (value.size() < 2) {
        qCWarning(pokitService).noquote() << tr("Invalid appearance size %1 for value:")
            .arg(value.size()) << value;
        return std::numeric_limits<quint16>::max();
    }
    if (value.size() > 2) {
        qCWarning(pokitService).noquote() << tr("Appearance has %1 extra bytes:")
            .arg(value.size()-2) << value.mid(2);
    }
    const quint16 appearance = qFromLittleEndian<quint16>(value);
    qCDebug(pokitService).noquote() << tr("Appearance: %1.").arg(appearance);
    return appearance;
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicRead to parse \a value, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void GenericAccessServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    qCDebug(pokitService).noquote() << tr("Read  characteristic \"%1\" (%2) of size %3:")
        .arg(characteristic.name(), characteristic.uuid().toString()).arg(value.size()) << value;

    Q_Q(GenericAccessService);
    if (characteristic.uuid() == GenericAccessService::CharacteristicUuids::appearance) {
        emit q->appearanceRead(parseAppearance(value));
        return;
    }

    if (characteristic.uuid() == GenericAccessService::CharacteristicUuids::name) {
        const QString deviceName = QString::fromUtf8(value);
        qCDebug(pokitService).noquote() << tr("Device name: \"%1\"").arg(deviceName);
        emit q->deviceNameRead(deviceName);
        return;
    }

    qCWarning(pokitService).noquote() << tr("Unknown characteristic read for Generic Access Service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void GenericAccessServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                 const QByteArray &newValue)
{
    qCDebug(pokitService).noquote() << tr("Characteristic \"%1\" (%2) written, with new value:")
        .arg(characteristic.name(), characteristic.uuid().toString()) << newValue;

    Q_Q(GenericAccessService);
    if (characteristic.uuid() == GenericAccessService::CharacteristicUuids::appearance) {
        qCWarning(pokitService).noquote() << tr("Appearance haracteristic is read-only, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == GenericAccessService::CharacteristicUuids::name) {
        emit q->deivceNameWritten();
        return;
    }

    qCWarning(pokitService).noquote() << tr("Unknown characteristic written for Generic Access service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
