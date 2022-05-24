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
 * Declares the PokitDiscoveryAgent class.
 */

#ifndef QTPOKIT_POKITDISCOVERYAGENT_H
#define QTPOKIT_POKITDISCOVERYAGENT_H

#include "qtpokit_global.h"

#include <QBluetoothDeviceDiscoveryAgent>

QTPOKIT_BEGIN_NAMESPACE

class PokitDiscoveryAgentPrivate;

class QTPOKIT_EXPORT PokitDiscoveryAgent : public QBluetoothDeviceDiscoveryAgent
{
    Q_OBJECT

public:
    explicit PokitDiscoveryAgent(const QBluetoothAddress &deviceAdapter, QObject *parent=nullptr);
    PokitDiscoveryAgent(QObject * parent=nullptr);
    virtual ~PokitDiscoveryAgent();

    static bool isPokitDevice(const QBluetoothDeviceInfo &info);
    static bool isPokitMeter(const QBluetoothDeviceInfo &info);
    static bool isPokitPro(const QBluetoothDeviceInfo &info);

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
    PokitDiscoveryAgentPrivate * d_ptr; ///< Internal d-pointer.
    PokitDiscoveryAgent(PokitDiscoveryAgentPrivate * const d,
                              const QBluetoothAddress &deviceAdapter, QObject * const parent);
    PokitDiscoveryAgent(PokitDiscoveryAgentPrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(PokitDiscoveryAgent)
    Q_DISABLE_COPY(PokitDiscoveryAgent)
    friend class TestPokitDiscoveryAgent;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITDISCOVERYAGENT_H
