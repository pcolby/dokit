// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
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
    /// UUID of the `Calibration` service.
    static inline const QBluetoothUuid serviceUuid { QStringLiteral("6f53be2f-780b-49b8-a7c3-e8a052b3ae2c") };

    /// Characteristics available via the `Calibration` service.
    struct QTPOKIT_EXPORT CharacteristicUuids {
        /// UUID of the `Calibration` service's `Temperature` characterstic.
        static inline const QBluetoothUuid temperature { QStringLiteral("0cd0f713-f5aa-4572-9e23-f8049f6bcaaa") };

        /// \todo UUID of the `Calibration` service's (undocumented) `Get Parameter` characterstic.
        static inline const QBluetoothUuid getParam { QStringLiteral("b6728f91-409c-4d6c-864e-272a6a7a0204") };

        /// \todo UUID of the `Calibration` service's (undocumented) `Set Parameter` characterstic.
        static inline const QBluetoothUuid setParam { QStringLiteral("5588e47b-cb81-4f7b-acc4-6029a3f39f724") };
    };

    CalibrationService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~CalibrationService() override;

    bool readCharacteristics() override;

    // Temperature characteristic (BLE write only).
    bool calibrateTemperature(const float ambientTemperature);

Q_SIGNALS:
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
