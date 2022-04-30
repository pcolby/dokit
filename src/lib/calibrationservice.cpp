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
 * Defines the CalibrationService and CalibrationServicePrivate classes.
 */

#include <qtpokit/calibrationservice.h>
#include "calibrationservice_p.h"
#include "logging_p.h"

#include <QtEndian>

/*!
 * \class CalibrationService
 *
 * The CalibrationService class accesses the `Calibrartion` service of Pokit devices.
 */

/// UUID of the "Calibration" service.
const QBluetoothUuid CalibrationService::
    serviceUuid(QLatin1String("6f53be2f-780b-49b8-a7c3-e8a052b3ae2c"));

/// \struct CalibrationService::CharacteristicUuids
/// \brief Characteristics available via the `Calibration` service.

/// UUID of the `Calibration` service's `Temperature` characterstic.
const QBluetoothUuid CalibrationService::CharacteristicUuids::
    temperature(QLatin1String("0cd0f713-f5aa-4572-9e23-f8049f6bcaaa"));

/*!
 * Constructs a new Pokit service with \a parent.
 */
CalibrationService::CalibrationService(QLowEnergyController * const controller, QObject * parent)
    : AbstractPokitService(new CalibrationServicePrivate(controller, this), parent)
{

}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
CalibrationService::CalibrationService(
    CalibrationServicePrivate * const d, QObject * const parent)
    : AbstractPokitService(d, parent)
{

}
/// \endcond

/*!
 * Destroys this CalibrationService object.
 */
CalibrationService::~CalibrationService()
{
    delete d_ptr;
}

/*!
 * \copybrief AbstractPokitService::readCharacteristics
 *
 * This implementation always returns `true`, since the Calibration service provides no *readable*
 * characteristics (they're all write-only).
 */
bool CalibrationService::readCharacteristics()
{
    qCDebug(pokitService).noquote() << tr("Ignoring read request; the Calibration service is write-only.");
    return true;
}

/*!
 * Set's the Pokit device's name to \a name.
 *
 * Returns `true` if the write request was successfully queued, `false` otherwise.
 *
 * Emits deivceNameWritten() if/when the \a name has been set.
 */
bool CalibrationService::calibrateTemperature(const float ambientTemperature)
{
    static_assert(sizeof(float) == 4, "Pokit devices expect 32-bit floats");
    Q_D(const CalibrationService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::temperature);
    if (!characteristic.isValid()) {
        return false;
    }

    QByteArray newValue(sizeof(float), '\0');
    qToLittleEndian<float>(ambientTemperature, newValue.data());
    qCDebug(pokitService).noquote() << tr("Writing new temperature %1 (0x%2).")
        .arg(ambientTemperature).arg(QLatin1String(newValue.toHex()));
    d->service->writeCharacteristic(characteristic, newValue);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}

/*!
 * \fn CalibrationService::temperatureCalibrated
 *
 * This signal is emitted when the `Temperature` characteristic has been written succesfully.
 *
 * \see calibrateTemperature
 */

/*!
 * \cond internal
 * \class CalibrationServicePrivate
 *
 * The CalibrationServicePrivate class provides private implementation for CalibrationService.
 */

/*!
 * \internal
 * Constructs a new CalibrationServicePrivate object with public implementation \a q.
 */
CalibrationServicePrivate::CalibrationServicePrivate(
    QLowEnergyController * controller, CalibrationService * const q)
    : AbstractPokitServicePrivate(CalibrationService::serviceUuid, controller, q)
{

}

/*!
 * Implements AbstractPokitServicePrivate::characteristicRead to parse \a value, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void CalibrationServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    Q_UNUSED(value);
    qCWarning(pokitService).noquote() << tr("Characteristic read event on write-only service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void CalibrationServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                 const QByteArray &newValue)
{
    qCDebug(pokitService).noquote() << tr("Characteristic \"%1\" (%2) written, with new value:")
        .arg(characteristic.name(), characteristic.uuid().toString()) << newValue;

    Q_Q(CalibrationService);
    if (characteristic.uuid() == CalibrationService::CharacteristicUuids::temperature) {
        emit q->temperatureCalibrated();
        return;
    }

    qCWarning(pokitService).noquote() << tr("Unknown characteristic written for Calibration service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
