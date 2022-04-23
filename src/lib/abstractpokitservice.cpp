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
 * The AbstractPokitService class provides a common base for Pokit services classes.
 */

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

/*!
 * \fn virtual bool AbstractPokitService::readCharacteristics() = 0
 *
 * Read all characteristics.
 *
 * This convenience function will queue refresh requests of all characteristics supported by this
 * service.
 *
 * Relevant `*Service::*Read` signals will be emitted by derived class objects as each
 * characteristic is successfully read.
 */

/*!
 * Returns `true` if autodiscovery of services and service details is enabled, `false` otherwise.
 *
 * \see setAutoDiscover for more information on what autodiscovery provides.
 */
bool AbstractPokitService::autoDiscover() const
{
    Q_D(const AbstractPokitService);
    return d->autoDiscover;
}

/*!
 * If \a discover is \c true, autodiscovery will be attempted.
 *
 * Specifically, this may resulting in automatic invocation of:
 * * QLowEnergyController::discoverServices if/when the internal controller is connected; and
 * * QLowEnergyService::discoverDetails if/when an internal service object is created.
 *
 * \see autoDiscover
 */
void AbstractPokitService::setAutoDiscover(const bool discover)
{
    Q_D(AbstractPokitService);
    d->autoDiscover = discover;
}

/*!
 * Returns a non-const pointer to the internal service object, if any.
 */
QLowEnergyService * AbstractPokitService::service()
{
    Q_D(AbstractPokitService);
    return d->service;
}

/*!
 * Returns a const pointer to the internal service object, if any.
 */
const QLowEnergyService * AbstractPokitService::service() const
{
    Q_D(const AbstractPokitService);
    return d->service;
}

/*!
 * \fn void AbstractPokitService::serviceDetailsDiscovered()
 *
 * This signal is emitted when the Pokit service details have been discovered.
 *
 * Once this signal has been emitted, cached characteristics values should be immediately available
 * via derived classes' accessor functions, and refreshes can be queued via readCharacteristics()
 * and any related read functions provided by derived classes.
 */

/*!
 * \fn void AbstractPokitService::serviceErrorOccurred(QLowEnergyService::ServiceError newError)
 *
 *  This signal is emitted whenever an error occurs on the underlying QLowEnergyService.
 */

/*!
 * \cond internal
 * \class AbstractPokitServicePrivate
 *
 * The AbstractPokitServicePrivate class provides private implementation for AbstractPokitService.
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

/*!
 * Creates an internal service object from the internal controller.
 *
 * Any existing service object will *not* be replaced.
 *
 * Returns \c true if a service was created successfully, either now, or sometime previously.
 */
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

    connect(service,
    #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
        QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error),
    #else
        &QLowEnergyService::errorOccurred,
    #endif
        this, &AbstractPokitServicePrivate::errorOccurred);

    if (autoDiscover) {
        service->discoverDetails();
    }
    return true;
}


/*!
 * Read the \a uuid characteristic.
 *
 * If succesful, the `QLowEnergyService::characteristicRead` signal will be emitted by the internal
 * service object.  For convenience, derived classes should implement the characteristicRead()
 * virtual function to handle the read value.
 *
 * Returns \c true if the characteristic read request was successfully queued, \c false otherwise.
 *
 * \see AbstractPokitService::readCharacteristics()
 * \see AbstractPokitServicePrivate::characteristicRead()
 */
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

/*!
 * Handles `QLowEnergyController::connected` events.
 *
 * If `autoDiscover` is enabled, this will begin service discovery on the newly connected contoller.
 *
 * \see AbstractPokitService::autoDiscover()
 */
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

/*!
 * Handles `QLowEnergyController::discoveryFinished` events.
 *
 * As this event indicates that the conroller has finished discovering services, this function will
 * invoke createServiceObject() to create the internal service object (if not already created).
 */
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

/*!
 * Handles `QLowEnergyController::errorOccurred` events.
 *
 * This function simply re-emits \a newError as AbstractPokitService::serviceErrorOccurred.
 */
void AbstractPokitServicePrivate::errorOccurred(const QLowEnergyService::ServiceError newError)
{
    Q_Q(AbstractPokitService);
    qCDebug(pokitService) << "Service error" << newError;
    emit q->serviceErrorOccurred(newError);
}

/*!
 * Handles `QLowEnergyController::serviceDiscovered` events.
 *
 * If the discovered service is the one this (or rather the derived) class wraps, then
 * createServiceObject() will be invoked immediately (otherwise it will be invoked after full
 * service discovery has completed, ie in discoveryFinished()).
 */
void AbstractPokitServicePrivate::serviceDiscovered(const QBluetoothUuid &newService)
{
    if ((!service) && (newService == serviceUuid)) {
        qCDebug(pokitService) << "Service discovered " << newService;
        createServiceObject();
    }
}

/*!
 * Handles `QLowEnergyController::stateChanged` events.
 *
 * If \a newState indicates that service details have now been discovered, then
 * AbstractPokitService::serviceDetailsDiscovered will be emitted.
 *
 * \see AbstractPokitService::autoDiscover()
 */
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

/*!
 * \fn AbstractPokitServicePrivate::characteristicRead
 *
 * Handles `QLowEnergyService::characteristicRead` events.
 *
 * Derived classes must implement this function to handle the successful reads of \a characteristic,
 * typically by parsing \a value, then emitting a speciailised signal.
 */

/*!
 * \fn AbstractPokitServicePrivate::characteristicWritten
 *
 * Handles `QLowEnergyService::characteristicWritten` events.
 *
 * Derived classes must implement this function to handle the successful writes of
 * \a characteristic, typically by parsing \a newValue, then emitting a speciailised signal.
 */

/// \endcond
