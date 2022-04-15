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
 * Defines the StatusService and StatusServicePrivate classes.
 */

#include <qtpokit/statusservice.h>
#include <qtpokit/uuids.h>

#include "statusservice_p.h"
#include "logging_p.h"

#include <QDebug>

const QBluetoothUuid StatusService::serviceId(QString::fromLatin1(POKIT_SERVICE_STATUS));

/*!
 * \class StatusService
 *
 * Virtual base class for Pokit services.
 */

/*!
 * Constructs a new Pokit service with \a parent.
 */
StatusService::StatusService(QLowEnergyController * const controller, QObject * parent)
    : AbstractPokitService(new StatusServicePrivate(controller, this), parent)
{

}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
StatusService::StatusService(
    StatusServicePrivate * const d, QObject * const parent)
    : AbstractPokitService(d, parent)
{

}
/// \endcond

/*!
 * Destroys this StatusService object.
 */
StatusService::~StatusService()
{
    delete d_ptr;
}

/*!
 * \cond internal
 * \class StatusServicePrivate
 *
 * The StatusServicePrivate provides the private implementation for StatusService.
 */

/*!
 * \internal
 * Constructs a new StatusServicePrivate object with public implementation \a q.
 */
StatusServicePrivate::StatusServicePrivate(
    QLowEnergyController * controller, StatusService * const q)
    : AbstractPokitServicePrivate(controller, q)
{

}

/// \endcond
