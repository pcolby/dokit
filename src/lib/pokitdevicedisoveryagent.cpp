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
 * Defines the PokitDeviceDiscoveryAgent class.
 *
 * \todo Add a PokitDeviceDiscoveryAgentPrivate class?
 */

#include <qtpokit/pokitdevicedisoveryagent.h>
#include "pokitdevicedisoveryagent_p.h"

#include <qtpokit/uuids.h>

#include "logging_p.h"

#include <QBluetoothUuid>
#include <QDebug>

/*!
 * \class PokitDeviceDiscoveryAgent
 *
 * The PokitDeviceDiscoveryAgent class discovers nearby Pokit devices.
 *
 * After constructing a PokitDeviceDiscoveryAgent object, and subscribing to the relevant signals,
 * invoke start() to begin discovery.
 */

/*!
 * Constructs a new Pokit device discovery agent with \a parent, using \a deviceAdapter for the
 * search device.
 */
PokitDeviceDiscoveryAgent::PokitDeviceDiscoveryAgent(
    const QBluetoothAddress &deviceAdapter, QObject *parent)
    : QBluetoothDeviceDiscoveryAgent(deviceAdapter, parent),
      d_ptr(new PokitDeviceDiscoveryAgentPrivate(this))
{

}

/*!
 * Constructs a new Pokit device discovery agent with \a parent.
 */
PokitDeviceDiscoveryAgent::PokitDeviceDiscoveryAgent(QObject * parent)
    : QBluetoothDeviceDiscoveryAgent(parent),
      d_ptr(new PokitDeviceDiscoveryAgentPrivate(this))
{

}

/*!
 * \cond internal
 * Constructs a new Pokit device discovery agent with \a parent, using \a deviceAdapter for the
 * search device, and private implementation \a d.
 */
PokitDeviceDiscoveryAgent::PokitDeviceDiscoveryAgent(
    PokitDeviceDiscoveryAgentPrivate * const d, const QBluetoothAddress &deviceAdapter,
    QObject * const parent)
    : QBluetoothDeviceDiscoveryAgent(deviceAdapter, parent), d_ptr(d)
{

}

/*!
 * Constructs a new Pokit device discovery agent with \a parent, and private implementation \a d.
 */
PokitDeviceDiscoveryAgent::PokitDeviceDiscoveryAgent(
    PokitDeviceDiscoveryAgentPrivate * const d, QObject * const parent)
    : QBluetoothDeviceDiscoveryAgent(parent), d_ptr(d)
{

}
/// \endcond

/*!
 * Destroys this PokitDeviceDiscoveryAgent object.
 */
PokitDeviceDiscoveryAgent::~PokitDeviceDiscoveryAgent()
{
    delete d_ptr;
}

/*!
 * Returns \c true if \a info describes a Pokit device.
 *
 * Currently, this is based on whether or not \a info's service UUIDs includes a known Pokit
 * service, but this test criteria might be swapped for something more explicity sometime.
 */
bool PokitDeviceDiscoveryAgent::isPokitDevice(const QBluetoothDeviceInfo &info)
{
    return info.serviceUuids().contains(QBluetoothUuid(QStringLiteral(POKIT_SERVICE_STATUS)));
}

/*!
 * Starts Pokit device discovery.
 *
 * This override simply enforces that \a method must be \c LowEnergyMethod, as all Pokit devices
 * used Bluetooth Low Energy (BLE).
 */
void PokitDeviceDiscoveryAgent::start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethods methods)
{
    Q_ASSERT(methods == QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    QBluetoothDeviceDiscoveryAgent::start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

/*!
 * Starts Pokit device discovery.
 */
void PokitDeviceDiscoveryAgent::start()
{
    QBluetoothDeviceDiscoveryAgent::start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

/*!
 * \fn void PokitDeviceDiscoveryAgent::pokitDeviceDiscovered(const QBluetoothDeviceInfo &info)
 *
 * This signal is emitted when the Pokit device described by \a info is discovered.
 */

/*!
 * \fn void PokitDeviceDiscoveryAgent::pokitDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
 *
 * This signal is emitted when the Pokit device described by \a info is updated. The
 * \a updatedFields flags tell which information has been updated.
 */

/*!
 * \cond internal
 * \class PokitDeviceDiscoveryAgentPrivate
 *
 * The PokitDeviceDiscoveryAgentPrivate provides the private implementation for
 * PokitDeviceDiscoveryAgent.
 */

/*!
 * \internal
 * \brief PokitDeviceDiscoveryAgentPrivate::PokitDeviceDiscoveryAgentPrivate
 * \param q
 */
PokitDeviceDiscoveryAgentPrivate::PokitDeviceDiscoveryAgentPrivate(PokitDeviceDiscoveryAgent * const q)
    : q_ptr(q)
{
    connect(q, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &PokitDeviceDiscoveryAgentPrivate::deviceDiscovered);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Signal added in Qt 5.12.
    connect(q, &QBluetoothDeviceDiscoveryAgent::deviceUpdated,
            this, &PokitDeviceDiscoveryAgentPrivate::deviceUpdated);
    #endif
}

/*!
 * Handle deviceDiscovered signals.
 *
 * Here we simply check if \a info describes a Pokit device, and if so, emit pokitDeviceDiscovered().
 */
void PokitDeviceDiscoveryAgentPrivate::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    Q_Q(PokitDeviceDiscoveryAgent);
    if (!q->isPokitDevice(info)) return;
    qCDebug(pokitDiscovery) << "Discovered Pokit device" << info.name() << info.address();
    emit q->pokitDeviceDiscovered(info);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
/*!
 * Handle deviceUpdated signals.
 *
 * Here we simply check if \a info describes a Pokit device, and if so, emit pokitDeviceUpdated().
 *
 * \since Qt 5.12.0
 */
void PokitDeviceDiscoveryAgentPrivate::deviceUpdated(
    const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
{
    Q_Q(PokitDeviceDiscoveryAgent);
    if (!q->isPokitDevice(info)) return;
    qCDebug(pokitDiscovery) << "Updated Pokit device" << info.name() << info.address() << info.rssi();
    emit q->pokitDeviceUpdated(info, updatedFields);
}

/// \endcond

#endif
