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

protected:
    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;

private:
    Q_DECLARE_PUBLIC(CalibrationService)
    Q_DISABLE_COPY(CalibrationServicePrivate)
    friend class TestCalibrationService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_CALIBRATIONSERVICE_P_H
