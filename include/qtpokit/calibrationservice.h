// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
