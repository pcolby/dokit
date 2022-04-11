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
 * Declares the AbstractPokitService class.
 */

#ifndef QTPOKIT_ABSTRACTPOKITSERVICE_H
#define QTPOKIT_ABSTRACTPOKITSERVICE_H

#include "qtpokit_global.h"

#include <QObject>

QTPOKIT_BEGIN_NAMESPACE

class AbstractPokitServicePrivate;
class QLowEnergyService;

class QTPOKIT_EXPORT AbstractPokitService : public QObject
{
    Q_OBJECT

public:
    AbstractPokitService(QObject * parent = nullptr);
    virtual ~AbstractPokitService();

public slots:

signals:

protected:
    /// \cond internal
    AbstractPokitServicePrivate * d_ptr; ///< Internal d-pointer.
    AbstractPokitService(AbstractPokitServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(AbstractPokitService)
    Q_DISABLE_COPY(AbstractPokitService)
    friend class TestAbstractPokitService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_ABSTRACTPOKITSERVICE_H
