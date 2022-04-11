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
 * Defines the AbstractPokitService and AbstractPokitServicePrivate classes.
 */

#include <qtpokit/abstractpokitservice.h>
#include "abstractpokitservice_p.h"

#include "logging_p.h"

#include <QDebug>

/*!
 * \class AbstractPokitService
 *
 * Virtual base class for Pokit services.
 */

/*!
 * Constructs a new Pokit device discovery agent with \a parent.
 */
AbstractPokitService::AbstractPokitService(QObject * parent)
    : QObject(parent),
      d_ptr(new AbstractPokitServicePrivate(this))
{

}

/*!
 * \cond internal
 * Constructs a new Pokit device discovery agent with \a parent, and private implementation \a d.
 */
AbstractPokitService::AbstractPokitService(
    AbstractPokitServicePrivate * const d, QObject * const parent)
    : QObject(parent), d_ptr(d)
{

}
/// \endcond

/*!
 * Destroys this AbstractPokitService object.
 */
AbstractPokitService::~AbstractPokitService()
{
    delete d_ptr;
}

/*!
 * \cond internal
 * \class AbstractPokitServicePrivate
 *
 * The AbstractPokitServicePrivate provides the private implementation for AbstractPokitService.
 */

/*!
 * \internal
 * Constructs a new AbstractPokitServicePrivate object with public implementation \a q.
 */
AbstractPokitServicePrivate::AbstractPokitServicePrivate(AbstractPokitService * const q)
    : q_ptr(q)
{

}

/// \endcond
