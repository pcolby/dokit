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
 * Declares the AbstractPokitServicePrivate class.
 */

#ifndef QTPOKIT_ABSTRACTPOKITSERVICE_P_H
#define QTPOKIT_ABSTRACTPOKITSERVICE_P_H

#include <qtpokit/qtpokit_global.h>

#include <QBluetoothUuid>
#include <QObject>

class QLowEnergyController;
class QLowEnergyService;

QTPOKIT_BEGIN_NAMESPACE

class AbstractPokitService;

class QTPOKIT_EXPORT AbstractPokitServicePrivate : public QObject
{
    Q_OBJECT

public:
    QLowEnergyController * controller;
    QLowEnergyService * service;

    explicit AbstractPokitServicePrivate(QLowEnergyController * controller,
                                         AbstractPokitService * const q);

    bool readCharacteristic(const QBluetoothUuid &uuid);

protected:
    AbstractPokitService * q_ptr; ///< Internal q-pointer.

private:
    Q_DECLARE_PUBLIC(AbstractPokitService)
    Q_DISABLE_COPY(AbstractPokitServicePrivate)
    friend class TestAbstractPokitService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_ABSTRACTPOKITSERVICE_P_H
