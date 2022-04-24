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
 * Declares the CalibrationService class.
 */

#ifndef QTPOKIT_CALIBRATIONSERVICE_H
#define QTPOKIT_CALIBRATIONSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class CalibrationServicePrivate;

class QTPOKIT_EXPORT CalibrationService : public AbstractPokitService
{
    Q_OBJECT

public:
    static const QBluetoothUuid serviceUuid;

    struct CharacteristicUuids {
        static const QBluetoothUuid temperature;
    };

    CalibrationService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~CalibrationService() override;

    bool readCharacteristics() override;

    // Temperature characteristic (BLE write only).
    bool calibrateTemperature(const float ambientTemperature);

signals:
    void temperatureCalibrated();

protected:
    /// \cond internal
    CalibrationService(CalibrationServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(CalibrationService)
    Q_DISABLE_COPY(CalibrationService)
    friend class TestCalibrationService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_CALIBRATIONSERVICE_H
