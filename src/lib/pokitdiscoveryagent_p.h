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
 * Declares the PokitDiscoveryAgentPrivate class.
 */

#ifndef QTPOKIT_POKITDISCOVERYAGENT_P_H
#define QTPOKIT_POKITDISCOVERYAGENT_P_H

#include <qtpokit/qtpokit_global.h>

QTPOKIT_BEGIN_NAMESPACE

class PokitDiscoveryAgent;

class QTPOKIT_EXPORT PokitDiscoveryAgentPrivate : public QObject
{
    Q_OBJECT

public:
    explicit PokitDiscoveryAgentPrivate(PokitDiscoveryAgent * const q);

public slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &info);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    void deviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    #endif

protected:
    PokitDiscoveryAgent * q_ptr; ///< Internal q-pointer.

private:
    Q_DECLARE_PUBLIC(PokitDiscoveryAgent)
    Q_DISABLE_COPY(PokitDiscoveryAgentPrivate)
    friend class TestPokitDiscoveryAgent;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITDISCOVERYAGENT_P_H
