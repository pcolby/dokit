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
 * Declares the PokitDeviceDiscoveryAgent class.
 */

#ifndef QTPOKIT_POKITDEVICEDISCOVERYAGENT_H
#define QTPOKIT_POKITDEVICEDISCOVERYAGENT_H

#include "qtpokit_global.h"

#include <QBluetoothDeviceDiscoveryAgent>

QTPOKIT_BEGIN_NAMESPACE

class PokitDeviceDiscoveryAgentPrivate;

class QTPOKIT_EXPORT PokitDeviceDiscoveryAgent : public QBluetoothDeviceDiscoveryAgent
{
    Q_OBJECT

public:
    explicit PokitDeviceDiscoveryAgent(const QBluetoothAddress &deviceAdapter, QObject *parent=nullptr);
    PokitDeviceDiscoveryAgent(QObject * parent=nullptr);
    virtual ~PokitDeviceDiscoveryAgent();

    static bool isPokitDevice(const QBluetoothDeviceInfo &info);

public slots:
    void start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethods methods);
    void start();

signals:
    void pokitDeviceDiscovered(const QBluetoothDeviceInfo &info);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    void pokitDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    #endif

protected:
    /// \cond internal
    PokitDeviceDiscoveryAgentPrivate * d_ptr; ///< Internal d-pointer.
    PokitDeviceDiscoveryAgent(PokitDeviceDiscoveryAgentPrivate * const d,
                              const QBluetoothAddress &deviceAdapter, QObject * const parent);
    PokitDeviceDiscoveryAgent(PokitDeviceDiscoveryAgentPrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(PokitDeviceDiscoveryAgent)
    Q_DISABLE_COPY(PokitDeviceDiscoveryAgent)
    friend class TestPokitDeviceDiscoveryAgent;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITDEVICEDISCOVERYAGENT_H
