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
 * Declares the StatusServicePrivate class.
 */

#ifndef QTPOKIT_STATUSSERVICE_P_H
#define QTPOKIT_STATUSSERVICE_P_H

#include <qtpokit/statusservice.h>

#include "abstractpokitservice_p.h"

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT StatusServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit StatusServicePrivate(QLowEnergyController * controller, StatusService * const q);

    static StatusService::DeviceCharacteristics parseDeviceCharacteristics(const QByteArray &value);
    static StatusService::Status parseStatus(const QByteArray &value);

protected:
    void serviceDiscovered(const QBluetoothUuid &newService) override;

    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;

private:
    Q_DECLARE_PUBLIC(StatusService)
    Q_DISABLE_COPY(StatusServicePrivate)
    friend class TestStatusService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_STATUSSERVICE_P_H
