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
 * Defines the PokitDevice and PokitDevicePrivate classes.
 */

#include <qtpokit/pokitdevice.h>

#include <qtpokit/statusservice.h>

#include "pokitdevice_p.h"
#include "logging_p.h"

#include <QDebug>
#include <QMutexLocker>

/*!
 * \class PokitDevice
 *
 * The PokitDevice class discovers nearby Pokit devices.
 */

/*!
 * Constructs a new Pokit device controller wrapper for \a addressOrUuid, with \a parent.
 *
 * \a addressOrUuid must be either a UUID (as accepted by the QBluetoothUuid constructor), or a
 * device address (as accepted by the QBluetoothAddress contructur).  Specifically:
 *
 * ```
 *
 */
PokitDevice::PokitDevice(const QString &addressOrUuid, QObject *parent)
    : QObject(parent), d_ptr(new PokitDevicePrivate(this))
{
    Q_D(PokitDevice);
    const QBluetoothDeviceInfo info = (addressOrUuid.contains(QLatin1Char('-'))
        ? QBluetoothDeviceInfo(QBluetoothUuid(addressOrUuid), QString(), 0)
        : QBluetoothDeviceInfo(QBluetoothAddress(addressOrUuid), QString(), 0));
    qCDebug(pokitController) << "Pokit device" << info.deviceUuid() << info.address();
    d->setController(new QLowEnergyController(info, this));
}

/*!
 * Constructs a new Pokit device controller wrapper for \a controller, with \a parent.
 */
PokitDevice::PokitDevice(QLowEnergyController *controller, QObject *parent)
    : QObject(parent), d_ptr(new PokitDevicePrivate(this))
{
    Q_D(PokitDevice);
    d->setController(controller);
}

/*!
 * \cond internal
 * Constructs a new Pokit device discovery agent with \a parent, and private implementation \a d.
 *
 * Derived classes using this constructor should use PokitDevicePrivate::setController to assign
 * the BLE controller as some point.
 */
PokitDevice::PokitDevice(PokitDevicePrivate * const d, QObject * const parent)
    : QObject(parent), d_ptr(d)
{

}
/// \endcond

/*!
 * Destroys this PokitDevice object.
 */
PokitDevice::~PokitDevice()
{
    delete d_ptr;
}

QLowEnergyController * PokitDevice::controller()
{
    Q_D(PokitDevice);
    return d->controller;
}

#define POKIT_INTERNAL_GET_SERVICE(typeName, varName) \
    Q_D(PokitDevice);                                 \
    QMutexLocker(&d->varName##Mutex);                 \
    if (d->varName == nullptr) {                      \
        d->varName = new typeName(d->controller);     \
    }                                                 \
    return d->varName                                 \

DataLoggerService * PokitDevice::dataLogger()
{
    qCDebug(pokitController) << "Not imlemented" << __func__;
//    POKIT_INTERNAL_GET_SERVICE(DataLoggerService, dataLogger);
    return nullptr;
}

DeviceInformationService * PokitDevice::deviceInformation()
{
    qCDebug(pokitController) << "Not imlemented" << __func__;
//    POKIT_INTERNAL_GET_SERVICE(DeviceInformationService, deviceInformation);
    return nullptr;
}

DsoService * PokitDevice::dso()
{
    qCDebug(pokitController) << "Not imlemented" << __func__;
//    POKIT_INTERNAL_GET_SERVICE(DsoService, dso);
    return nullptr;
}

GenericAccessService * PokitDevice::genericAccess()
{
    qCDebug(pokitController) << "Not imlemented" << __func__;
//    POKIT_INTERNAL_GET_SERVICE(GenericAccessService, genericAccess);
    return nullptr;
}

MultimeterService * PokitDevice::multimeter()
{
    qCDebug(pokitController) << "Not imlemented" << __func__;
//    POKIT_INTERNAL_GET_SERVICE(MultimeterService, multimeter);
    return nullptr;
}

StatusService * PokitDevice::status()
{
    POKIT_INTERNAL_GET_SERVICE(StatusService, status);
}
#undef POKIT_INTERNAL_GET_SERVICE

/*!
 * \cond internal
 * \class PokitDevicePrivate
 *
 * The PokitDevicePrivate provides the private implementation for PokitDevice.
 */

/*!
 * Constructs a new PokitDevicePrivate object with public implementation \a q.
 */
PokitDevicePrivate::PokitDevicePrivate(PokitDevice * const q)
    : q_ptr(q)
{

}

void PokitDevicePrivate::setController(QLowEnergyController * newController)
{
    if (newController == this->controller) {
        qCDebug(pokitController) << "Controller is already" << newController;
        return;
    }

    if (this->controller) {
        qCDebug(pokitController) << "Disconnecting signals from previous controller" << controller;
        disconnect(this->controller, nullptr, this, nullptr);
    }

    this->controller = newController;

    connect(controller, &QLowEnergyController::connected,
            this, &PokitDevicePrivate::connected);

    connect(controller, &QLowEnergyController::connectionUpdated,
            this, &PokitDevicePrivate::connectionUpdated);

    connect(controller, &QLowEnergyController::disconnected,
            this, &PokitDevicePrivate::disconnected);

    connect(controller, &QLowEnergyController::discoveryFinished,
            this, &PokitDevicePrivate::discoveryFinished);

    connect(controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
            this, &PokitDevicePrivate::error);

    connect(controller, &QLowEnergyController::serviceDiscovered,
            this, &PokitDevicePrivate::serviceDiscovered);

    connect(controller, &QLowEnergyController::stateChanged,
            this, &PokitDevicePrivate::stateChanged);
}

/*!
 * Handle connected signals.
 */
void PokitDevicePrivate::connected()
{
    qCDebug(pokitController) << "Connected";
}

/*!
 * Handle connectionUpdated signals.
 */
void PokitDevicePrivate::connectionUpdated(const QLowEnergyConnectionParameters &newParameters)
{
    qCDebug(pokitController) << "Connection updated" << newParameters.latency()
        << newParameters.minimumInterval() << newParameters.maximumInterval()
        << newParameters.supervisionTimeout();
}

/*!
 * Handle disconnected signals.
 */
void PokitDevicePrivate::disconnected()
{
    qCDebug(pokitController) << "Disconnected";
}

/*!
 * Handle discoveryFinished signals.
 */
void PokitDevicePrivate::discoveryFinished()
{
    qCDebug(pokitDiscovery) << "Discovery finished";
}

/*!
 * Handle error signals.
 */
void PokitDevicePrivate::error(QLowEnergyController::Error newError)
{
    qCDebug(pokitController) << "Controller error" << newError;
}

/*!
 * Handle serviceDiscovered signals.
 */
void PokitDevicePrivate::serviceDiscovered(const QBluetoothUuid &newService)
{
    qCDebug(pokitController) << "Service discovered" << newService;
}

/*!
 * Handle stateChanged signals.
 */
void PokitDevicePrivate::stateChanged(QLowEnergyController::ControllerState state)
{
    qCDebug(pokitController) << "State changed" << state;
}

/// \endcond
