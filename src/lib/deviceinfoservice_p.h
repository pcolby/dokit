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
 * Declares the DeviceInfoServicePrivate class.
 */

#ifndef QTPOKIT_INFOSERVICE_P_H
#define QTPOKIT_INFOSERVICE_P_H

#include <qtpokit/deviceinfoservice.h>

#include "abstractpokitservice_p.h"

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT DeviceInfoServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit DeviceInfoServicePrivate(QLowEnergyController * controller, DeviceInfoService * const q);

protected:
    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;

private:
    Q_DECLARE_PUBLIC(DeviceInfoService)
    Q_DISABLE_COPY(DeviceInfoServicePrivate)
    friend class TestDeviceInfoService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_INFOSERVICE_P_H
