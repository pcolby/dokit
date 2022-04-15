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
 * Declares the StatusService class.
 */

#ifndef QTPOKIT_STATUSSERVICE_H
#define QTPOKIT_STATUSSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothUuid>

QTPOKIT_BEGIN_NAMESPACE

class StatusServicePrivate;

QLowEnergyController * createPokitDeviceController(const QString &addressOrUuid);

class QTPOKIT_EXPORT StatusService : public AbstractPokitService
{
    Q_OBJECT

public:
    static const QBluetoothUuid serviceId;

    StatusService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    virtual ~StatusService();

public slots:

signals:

protected:
    /// \cond internal
    StatusService(StatusServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(StatusService)
    Q_DISABLE_COPY(StatusService)
    friend class TestStatusService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_StatusSERVICE_H
