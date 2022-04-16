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
#include <QLowEnergyController>

/*!
 * \class AbstractPokitService
 *
 * Virtual base class for Pokit services.
 */

/*!
 * Constructs a new Pokit service with \a parent.
 */
//AbstractPokitService::AbstractPokitService(const QBluetoothUuid &serviceUuid,
//    QLowEnergyController * const controller, QObject * parent)
//    : QObject(parent), d_ptr(new AbstractPokitServicePrivate(serviceUuid, controller, this))
//{

//}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
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

/// \todo document virtual bool readCharacteristics() = 0;

bool AbstractPokitService::autoDiscover() const
{
    Q_D(const AbstractPokitService);
    return d->autoDiscover;
}

void AbstractPokitService::setAutoDiscover(const bool discover)
{
    Q_D(AbstractPokitService);
    d->autoDiscover = discover;
}

QLowEnergyService * AbstractPokitService::service()
{
    Q_D(AbstractPokitService);
    return d->service;
}

const QLowEnergyService * AbstractPokitService::service() const
{
    Q_D(const AbstractPokitService);
    return d->service;
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
AbstractPokitServicePrivate::AbstractPokitServicePrivate(const QBluetoothUuid &serviceUuid,
    QLowEnergyController * controller, AbstractPokitService * const q)
    : autoDiscover(true), controller(controller), service(nullptr), serviceUuid(serviceUuid),
      q_ptr(q)
{
    if (controller) {
        connect(controller, &QLowEnergyController::connected,
                this, &AbstractPokitServicePrivate::connected);

        connect(controller, &QLowEnergyController::discoveryFinished,
                this, &AbstractPokitServicePrivate::discoveryFinished);

        connect(controller, &QLowEnergyController::serviceDiscovered,
                this, &AbstractPokitServicePrivate::serviceDiscovered);

        createServiceObject();
    }

}

/// \todo This could support a "replaceExisting" argument, but its unclear if that's wanted, and if
/// so, what data consistency issues might arise.
bool AbstractPokitServicePrivate::createServiceObject()
{
    if (!controller) {
        return false;
    }

    if (service) {
        qCDebug(pokitService) << "Already have service object" << service;
        return true;
    }

    service = controller->createServiceObject(serviceUuid, this);
    if (!service) {
        return false;
    }
    qCDebug(pokitService) << "Service object created" << service;

    connect(service, &QLowEnergyService::stateChanged,
            this, &AbstractPokitServicePrivate::stateChanged);
    connect(service, &QLowEnergyService::characteristicRead,
            this, &AbstractPokitServicePrivate::characteristicRead);
    connect(service, &QLowEnergyService::characteristicWritten,
            this, &AbstractPokitServicePrivate::characteristicWritten);

    if (autoDiscover) {
        service->discoverDetails();
    }
    return true;
}

bool AbstractPokitServicePrivate::readCharacteristic(const QBluetoothUuid &uuid)
{
    if (!service) {
        qCDebug(pokitService) << "Cannot read characteristic without a service object" << uuid;
        return false;
    }

    const QLowEnergyCharacteristic characteristic = service->characteristic(uuid);
    if (!characteristic.isValid()) {
        qCDebug(pokitService) << "Cannot read characteristic that is not yet valid" << uuid;
        return false;
    }

    qCDebug(pokitService) << "Reading characteristic" << uuid << characteristic.properties()
                          << characteristic.name() << characteristic.value();
    service->readCharacteristic(characteristic);
    return true;
}

void AbstractPokitServicePrivate::connected()
{
    if (!controller) {
        qCWarning(pokitService) << "Connected with no controller set" << sender();
        return;
    }

    qCDebug(pokitService) << "Connected" << controller->remoteName()
        << controller->remoteAddress() << controller->remoteDeviceUuid();
    if (autoDiscover) {
        controller->discoverServices();
    }
}

void AbstractPokitServicePrivate::discoveryFinished()
{
    if (!controller) {
        qCWarning(pokitService) << "Discovery finished with no controller set" << sender();
        return;
    }

    qCDebug(pokitService) << "Discovery finished" << controller->remoteName()
        << controller->remoteAddress() << controller->remoteDeviceUuid();

    if (!createServiceObject()) {
        qCWarning(pokitService) << "Discovery finished, but service not found";
        /// \todo Might need to emit an error signal here?
    }
}

void AbstractPokitServicePrivate::serviceDiscovered(const QBluetoothUuid &newService)
{
    if ((!service) && (newService == serviceUuid)) {
        qCDebug(pokitService) << "Service discovered " << newService;
        createServiceObject();
    }
}

void AbstractPokitServicePrivate::stateChanged(QLowEnergyService::ServiceState newState)
{
    qCDebug(pokitService) << "State changed" << newState;

    if (newState == QLowEnergyService::
            #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            ServiceDiscovered
            #else
            RemoteServiceDiscovered
            #endif
        ) {
        Q_Q(AbstractPokitService);
        qCDebug(pokitService) << "Service details discovered";
        emit q->serviceDetailsDiscovered();
    }
}

/// \todo Document these two too.
//virtual void characteristicRead(const QLowEnergyCharacteristic &characteristic,
//                                const QByteArray &value) = 0;
//virtual void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
//                                   const QByteArray &newValue) = 0;

/// \endcond
