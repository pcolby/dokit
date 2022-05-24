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
 * Defines the PokitDiscoveryAgent and PokitDiscoveryAgentPrivate classes.
 */

#include <qtpokit/pokitdiscoveryagent.h>
#include "pokitdiscoveryagent_p.h"

#include <qtpokit/statusservice.h>

#include <QBluetoothUuid>

/*!
 * \class PokitDiscoveryAgent
 *
 * The PokitDiscoveryAgent class discovers nearby Pokit devices.
 *
 * After constructing a PokitDiscoveryAgent object, and subscribing to the relevant signals,
 * invoke start() to begin discovery.
 */

/*!
 * Constructs a new Pokit device discovery agent with \a parent, using \a deviceAdapter for the
 * search device.
 */
PokitDiscoveryAgent::PokitDiscoveryAgent(
    const QBluetoothAddress &deviceAdapter, QObject *parent)
    : QBluetoothDeviceDiscoveryAgent(deviceAdapter, parent),
      d_ptr(new PokitDiscoveryAgentPrivate(this))
{

}

/*!
 * Constructs a new Pokit device discovery agent with \a parent.
 */
PokitDiscoveryAgent::PokitDiscoveryAgent(QObject * parent)
    : QBluetoothDeviceDiscoveryAgent(parent),
      d_ptr(new PokitDiscoveryAgentPrivate(this))
{

}

/*!
 * \cond internal
 * Constructs a new Pokit device discovery agent with \a parent, using \a deviceAdapter for the
 * search device, and private implementation \a d.
 */
PokitDiscoveryAgent::PokitDiscoveryAgent(
    PokitDiscoveryAgentPrivate * const d, const QBluetoothAddress &deviceAdapter,
    QObject * const parent)
    : QBluetoothDeviceDiscoveryAgent(deviceAdapter, parent), d_ptr(d)
{

}

/*!
 * Constructs a new Pokit device discovery agent with \a parent, and private implementation \a d.
 */
PokitDiscoveryAgent::PokitDiscoveryAgent(
    PokitDiscoveryAgentPrivate * const d, QObject * const parent)
    : QBluetoothDeviceDiscoveryAgent(parent), d_ptr(d)
{

}
/// \endcond

/*!
 * Destroys this PokitDiscoveryAgent object.
 */
PokitDiscoveryAgent::~PokitDiscoveryAgent()
{
    delete d_ptr;
}

/*!
 * Returns \c true if \a info describes a Pokit device.
 *
 * Currently, this is based on whether or not \a info's service UUIDs includes a known Pokit
 * service, but this test criteria might be swapped for something else sometime.
 */
bool PokitDiscoveryAgent::isPokitDevice(const QBluetoothDeviceInfo &info)
{
    return (isPokitMeter(info) || isPokitPro(info));
}

/*!
 * Returns \c true if \a info describes a Pokit Meter device.
 *
 * Currently, this is based on whether or not \a info's service UUIDs includes a known Pokit
 * service, but this test criteria might be swapped for something else sometime.
 */
bool PokitDiscoveryAgent::isPokitMeter(const QBluetoothDeviceInfo &info)
{
    return info.serviceUuids().contains(StatusService::ServiceUuids::pokitMeter);
}

/*!
 * Returns \c true if \a info describes a Pokit Pro device.
 *
 * Currently, this is based on whether or not \a info's service UUIDs includes a known Pokit
 * service, but this test criteria might be swapped for something else sometime.
 */
bool PokitDiscoveryAgent::isPokitPro(const QBluetoothDeviceInfo &info)
{
    return info.serviceUuids().contains(StatusService::ServiceUuids::pokitPro);
}

/*!
 * Starts Pokit device discovery.
 *
 * This override simply enforces that \a method must be \c LowEnergyMethod, as all Pokit devices
 * used Bluetooth Low Energy (BLE).
 */
void PokitDiscoveryAgent::start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethods methods)
{
    Q_D(PokitDiscoveryAgent);
    Q_ASSERT(methods == QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    qCDebug(d->lc).noquote() << tr("Scanning for Bluetooth Low Energy devices.");
    QBluetoothDeviceDiscoveryAgent::start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

/*!
 * Starts Pokit device discovery.
 */
void PokitDiscoveryAgent::start()
{
    Q_D(PokitDiscoveryAgent);
    qCDebug(d->lc).noquote() << tr("Scanning for Bluetooth Low Energy devices.");
    QBluetoothDeviceDiscoveryAgent::start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

/*!
 * \fn void PokitDiscoveryAgent::pokitDeviceDiscovered(const QBluetoothDeviceInfo &info)
 *
 * This signal is emitted when the Pokit device described by \a info is discovered.
 */

/*!
 * \fn void PokitDiscoveryAgent::pokitDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
 *
 * This signal is emitted when the Pokit device described by \a info is updated. The
 * \a updatedFields flags tell which information has been updated.
 */

/*!
 * \cond internal
 * \class PokitDiscoveryAgentPrivate
 *
 * The PokitDiscoveryAgentPrivate class provides private implementation for
 * PokitDiscoveryAgent.
 */

/*!
 * \internal
 * Constructs a new PokitDiscoveryAgentPrivate object with public implementation \a q.
 */
PokitDiscoveryAgentPrivate::PokitDiscoveryAgentPrivate(PokitDiscoveryAgent * const q)
    : q_ptr(q)
{
    connect(q, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &PokitDiscoveryAgentPrivate::deviceDiscovered);

    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Signal added in Qt 5.12.
    connect(q, &QBluetoothDeviceDiscoveryAgent::deviceUpdated,
            this, &PokitDiscoveryAgentPrivate::deviceUpdated);
    #endif
}

/*!
 * Handle scan canceled signals, by simply logging the event for diagnostic purposes.
 */
void PokitDiscoveryAgentPrivate::canceled()
{
    qCDebug(lc).noquote() << tr("Pokit device scan cancelled.");
}

/*!
 * Handle deviceDiscovered signals.
 *
 * Here we simply check if \a info describes a Pokit device, and if so, emit pokitDeviceDiscovered().
 */
void PokitDiscoveryAgentPrivate::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    Q_Q(PokitDiscoveryAgent);
    if (!q->isPokitDevice(info)) return;
    qCDebug(lc).noquote() << tr("Discovered Pokit device \"%1\" at %2.")
        .arg(info.name(), info.address().toString());
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
void PokitDiscoveryAgentPrivate::deviceUpdated(
    const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
{
    Q_Q(PokitDiscoveryAgent);
    if (!q->isPokitDevice(info)) return;
    qCDebug(lc).noquote() << tr("Pokit device \"%1\" at %2 updated with RSSI %3.")
        .arg(info.name(), info.address().toString()).arg(info.rssi());
    emit q->pokitDeviceUpdated(info, updatedFields);
}
#endif

/*!
 * Handle scan errors, by simply logging \a error for diagnostic purposes.
 */
void PokitDiscoveryAgentPrivate::error(const QBluetoothDeviceDiscoveryAgent::Error error)
{
    qCWarning(lc).noquote() << tr("Pokit device scan error:") << error;
}

/*!
 * Handle scan finished signals, by simply logging the event for diagnostic purposes.
 */
void PokitDiscoveryAgentPrivate::finished()
{
    qCDebug(lc).noquote() << tr("Pokit device scan finished.");
}

/// \endcond
