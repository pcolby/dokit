// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the CalibrationServicePrivate class.
 */

#ifndef QTPOKIT_CALIBRATIONSERVICE_P_H
#define QTPOKIT_CALIBRATIONSERVICE_P_H

#include <qtpokit/calibrationservice.h>

#include "abstractpokitservice_p.h"

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT CalibrationServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit CalibrationServicePrivate(QLowEnergyController * controller, CalibrationService * const q);

    static QByteArray encodeTemperature(const float value);

protected:
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;

private:
    Q_DECLARE_PUBLIC(CalibrationService)
    Q_DISABLE_COPY(CalibrationServicePrivate)
    friend class TestCalibrationService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_CALIBRATIONSERVICE_P_H
