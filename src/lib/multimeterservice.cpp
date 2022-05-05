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
 * Defines the MultimeterService and MultimeterServicePrivate classes.
 */

#include <qtpokit/multimeterservice.h>
#include "multimeterservice_p.h"

#include <QDataStream>
#include <QIODevice>
#include <QtEndian>

/*!
 * \class MultimeterService
 *
 * The MultimeterService class accesses the `Pokit Status` service of Pokit devices.
 */

/// UUID of the "Multimeter" service.
const QBluetoothUuid MultimeterService::
    serviceUuid(QLatin1String("e7481d2f-5781-442e-bb9a-fd4e3441dadc"));

/// \struct MultimeterService::CharacteristicUuids
/// \brief Characteristics available via the `Multimeter` service.

/// UUID of the `Multimeter` service's `Settings` characterstic.
const QBluetoothUuid MultimeterService::CharacteristicUuids::
    settings(QLatin1String("53dc9a7a-bc19-4280-b76b-002d0e23b078"));

/// UUID of the `Multimeter` service's `Reading` characterstic.
const QBluetoothUuid MultimeterService::CharacteristicUuids::
    reading(QLatin1String("047d3559-8bee-423a-b229-4417fa603b90"));

/// \struct MultimeterService::DeviceCharacteristics
/// \brief Attributes included in the `Device Characteristics` characterstic.

/// \enum MultimeterService::DeviceStatus
/// \brief Values support by the `Status` attribute of the `Status` characteristic.

/// \enum MultimeterService::BatteryStatus
/// \brief Values support by the `Battery Status` attribute of the `Status` characteristic.

/// \struct MultimeterService::Status
/// \brief Attributes included in the `Status` characterstic.

/*!
 * Constructs a new Pokit service with \a parent.
 */
MultimeterService::MultimeterService(QLowEnergyController * const controller, QObject * parent)
    : AbstractPokitService(new MultimeterServicePrivate(controller, this), parent)
{

}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
MultimeterService::MultimeterService(
    MultimeterServicePrivate * const d, QObject * const parent)
    : AbstractPokitService(d, parent)
{

}
/// \endcond

/*!
 * Destroys this MultimeterService object.
 */
MultimeterService::~MultimeterService()
{
    delete d_ptr;
}

bool MultimeterService::readCharacteristics()
{
    return readReadingCharacteristic();
}

/*!
 * Read the `Multimeter` service's `Reading` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits readingRead() if/when the characteristic has been read successfully.
 */
bool MultimeterService::readReadingCharacteristic()
{
    Q_D(MultimeterService);
    return d->readCharacteristic(CharacteristicUuids::reading);
}

/*!
 * Set's the Pokit device's name to \a name.
 *
 * Returns `true` if the write request was successfully queued, `false` otherwise.
 *
 * Emits deivceNameWritten() if/when the \a name has been set.
 */
bool MultimeterService::setSettings(const Settings &settings)
{
    Q_D(const MultimeterService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::settings);
    if (!characteristic.isValid()) {
        return false;
    }

    static_assert(sizeof(settings.mode) == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.range) == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.updateInterval) == 4, "Expected to be 4 bytes.");

    QByteArray value;
    QDataStream stream(&value, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << settings.mode << settings.range.voltageRange << settings.updateInterval;
    Q_ASSERT(value.size() == 6);
    d->service->writeCharacteristic(characteristic, value);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}

/*!
 * Returns the most recent value of the `Multimeter` service's `Reading` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then the
 * returned MultimeterService::Reading::value member will be a quiet NaN, which can be checked like:
 *
 * ```
 * const MultimeterService::Reading reading = multimeterService->reading();
 * if (qIsNaN(reading.value)) {
 *     // Handle failure.
 * }
 * ```
 */
MultimeterService::Reading MultimeterService::reading() const
{
    Q_D(const MultimeterService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::reading);
    return (characteristic.isValid()) ? MultimeterServicePrivate::parseReading(characteristic.value())
        : Reading{ ReadingStatusFlags(), std::numeric_limits<float>::quiet_NaN(),
                   MultimeterMode::Idle, { VoltageRange::AutoRange } };
}

/*!
 * \fn MultimeterService::readingRead
 *
 * This signal is emitted when the `Reading` characteristic has been read successfully.
 *
 * \see readReadingCharacteristic
 */

/*!
 * \fn MultimeterService::settingsWritten
 *
 * This signal is emitted when the `Settings` characteristic has been written successfully.
 *
 * \see setSettings
 */

/*!
 * \cond internal
 * \class MultimeterServicePrivate
 *
 * The MultimeterServicePrivate class provides private implementation for MultimeterService.
 */

/*!
 * \internal
 * Constructs a new MultimeterServicePrivate object with public implementation \a q.
 */
MultimeterServicePrivate::MultimeterServicePrivate(
    QLowEnergyController * controller, MultimeterService * const q)
    : AbstractPokitServicePrivate(MultimeterService::serviceUuid, controller, q)
{

}

/*!
 * Parses the `Reading` \a value into a MultimeterService::Reading struct.
 */
MultimeterService::Reading MultimeterServicePrivate::parseReading(const QByteArray &value)
{
    MultimeterService::Reading reading{
        MultimeterService::ReadingStatusFlags(),
        std::numeric_limits<float>::quiet_NaN(),
        MultimeterService::MultimeterMode::Idle,
        { MultimeterService::VoltageRange::AutoRange }
    };

    /// \todo Abstract out this style of min/max size checking.
    if (value.size() < 7) {
        qCWarning(lc).noquote() << tr("Invalid reading size %1 for value: 0x%2")
            .arg(value.size()).arg(QLatin1String(value.toHex()));
        return reading;
    }
    if (value.size() > 7) {
        qCWarning(lc).noquote() << tr("Reading has %1 extra bytes: 0x%2")
            .arg(value.size()-7).arg(QLatin1String(value.mid(7).toHex()));
    }

    reading.status = MultimeterService::ReadingStatusFlags(value.at(0));
    reading.value = qFromLittleEndian<float>(value.mid(1,4));
    reading.mode = static_cast<MultimeterService::MultimeterMode>(value.at(5));
    reading.range.voltageRange = static_cast<MultimeterService::VoltageRange>(value.at(6));
    return reading;
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicRead to parse \a value, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void MultimeterServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    qCDebug(lc).noquote() << tr("Read  characteristic \"%1\" (%2) of size %3: 0x%4")
        .arg(characteristic.name(), characteristic.uuid().toString()).arg(value.size())
        .arg(QLatin1String(value.toHex()));

    Q_Q(MultimeterService);
    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::reading) {
        emit q->readingRead(parseReading(value));
        return;
    }

    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::settings) {
        qCWarning(lc).noquote() << tr("Settings characteristic is write-only, but somehow read")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic read for Status Service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void MultimeterServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                 const QByteArray &newValue)
{
    qCDebug(lc).noquote() << tr("Characteristic \"%1\" (%2) written, with new value:")
        .arg(characteristic.name(), characteristic.uuid().toString()) << newValue;

    Q_Q(MultimeterService);
    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::settings) {
        emit q->settingsWritten();
        return;
    }

    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::reading) {
        qCWarning(lc).noquote() << tr("Reading characteristic is read-only, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic written for Status Service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
