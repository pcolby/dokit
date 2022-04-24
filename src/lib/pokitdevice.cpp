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

#include <qtpokit/calibrationservice.h>
#include <qtpokit/deviceinfoservice.h>
#include <qtpokit/genericaccessservice.h>
#include <qtpokit/statusservice.h>

#include "pokitdevice_p.h"
#include "logging_p.h"

#include <QMutexLocker>

/*!
 * \class PokitDevice
 *
 * The PokitDevice class simplifies Pokit device access.
 *
 * It does this by wrapping QLowEnergyController to provide:
 * * automatic detection of device UUID vs MAC address strings for device IDs; and
 * * convenient Pokit service factory methods (dataLogger(), deviceInformation(), dso(),
 *   genericAccess(), multimeter() and status()); and
 * * consistent debug logging of QLowEnergyController events.
 *
 * But this class is entirely optional, in that all features of all all QtPokit classes can be used
 * wihtout this class.  It's just a (meaningful) convenience.
 */

/*!
 * Constructs a new Pokit device controller wrapper for \a addressOrUuid, with \a parent.
 *
 * \a addressOrUuid must be either a UUID (as accepted by the QBluetoothUuid constructor), or a
 * device address (as accepted by the QBluetoothAddress contructur).  Specifically, any of the
 * following formats (where `x` is hex digit):
 *
 * ```
 * "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"
 * "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
 * "xx:xx:xx:xx:xx:xx"
 * "xxxxxxxxxxxx"
 * ```
 *
 * \see QBluetoothAddress::QBluetoothAddress(const QString &address)
 * \see QBluetoothUuid::QBluetoothUuid(const QString &uuid)
 */
PokitDevice::PokitDevice(const QString &addressOrUuid, QObject *parent)
    : QObject(parent), d_ptr(new PokitDevicePrivate(this))
{
    Q_D(PokitDevice);
    const QBluetoothDeviceInfo info = (addressOrUuid.contains(QLatin1Char('-'))
        ? QBluetoothDeviceInfo(QBluetoothUuid(addressOrUuid), QString(), 0)
        : QBluetoothDeviceInfo(QBluetoothAddress(addressOrUuid), QString(), 0));
    d->setController(QLowEnergyController::createCentral(info, this));
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

/*!
 * Returns a non-const pointer to the controller used to access the Pokit device.
 */
QLowEnergyController * PokitDevice::controller()
{
    Q_D(PokitDevice);
    return d->controller;
}

/*!
 * Returns a const pointer to the controller used to access the Pokit device.
 */
const QLowEnergyController * PokitDevice::controller() const
{
    Q_D(const PokitDevice);
    return d->controller;
}

/// \cond
#define POKIT_INTERNAL_GET_SERVICE(typeName, varName) \
    Q_D(PokitDevice);                                 \
    const QMutexLocker scopedLock(&d->varName##Mutex);\
    if (d->varName == nullptr) {                      \
        d->varName = new typeName(d->controller);     \
    }                                                 \
    return d->varName                                 \
/// \endcond

/*!
 * Returns a pointer to a CalibrationService instance that uses this device's controller for access.
 *
 * This is a convenience function, that always returns the same pointer (for this PokitDevice
 * instance), but the service itself is lazily created (in a threadsafe manner) on the first
 * invocation of this function.
 *
 * \todo Implement this function when the DataLoggerService is implemented.
 */
CalibrationService * PokitDevice::calibration()
{
    POKIT_INTERNAL_GET_SERVICE(CalibrationService, calibration);
}

/*!
 * Returns a pointer to a DataLoggerService instance that uses this device's controller for access.
 *
 * This is a convenience function, that always returns the same pointer (for this PokitDevice
 * instance), but the service itself is lazily created (in a threadsafe manner) on the first
 * invocation of this function.
 *
 * \todo Implement this function when the DataLoggerService is implemented.
 */
DataLoggerService * PokitDevice::dataLogger()
{
    qCDebug(pokitController) << "Not implemented" << __func__;
//    POKIT_INTERNAL_GET_SERVICE(DataLoggerService, dataLogger);
    return nullptr;
}

/*!
 * Returns a pointer to DeviceInformationService instance that uses this device's controller for
 * access.
 *
 * This is a convenience function, that always returns the same pointer (for this PokitDevice
 * instance), but the service itself is lazily created (in a threadsafe manner) on the first
 * invocation of this function.
 */
DeviceInfoService * PokitDevice::deviceInformation()
{
    POKIT_INTERNAL_GET_SERVICE(DeviceInfoService, deviceInfo);
}

/*!
 * Returns a pointer to DsoService instance that uses this device's controller for access.
 *
 * This is a convenience function, that always returns the same pointer (for this PokitDevice
 * instance), but the service itself is lazily created (in a threadsafe manner) on the first
 * invocation of this function.
 *
 * \todo Implement this function when the DsoService is implemented.
 */
DsoService * PokitDevice::dso()
{
    qCDebug(pokitController) << "Not implemented" << __func__;
//    POKIT_INTERNAL_GET_SERVICE(DsoService, dso);
    return nullptr;
}

/*!
 * Returns a pointer to GenericAccessService instance that uses this device's controller for access.
 *
 * This is a convenience function, that always returns the same pointer (for this PokitDevice
 * instance), but the service itself is lazily created (in a threadsafe manner) on the first
 * invocation of this function.
 */
GenericAccessService * PokitDevice::genericAccess()
{
    POKIT_INTERNAL_GET_SERVICE(GenericAccessService, genericAccess);
}

/*!
 * Returns a pointer to MultimeterService instance that uses this device's controller for access.
 *
 * This is a convenience function, that always returns the same pointer (for this PokitDevice
 * instance), but the service itself is lazily created (in a threadsafe manner) on the first
 * invocation of this function.
 *
 * \todo Implement this function when the MultimeterService is implemented.
 */
MultimeterService * PokitDevice::multimeter()
{
    qCDebug(pokitController) << "Not implemented" << __func__;
//    POKIT_INTERNAL_GET_SERVICE(MultimeterService, multimeter);
    return nullptr;
}

/*!
 * Returns a pointer to StatusService instance that uses this device's controller for access.
 *
 * This is a convenience function, that always returns the same pointer (for this PokitDevice
 * instance), but the service itself is lazily created (in a threadsafe manner) on the first
 * invocation of this function.
 */
StatusService * PokitDevice::status()
{
    POKIT_INTERNAL_GET_SERVICE(StatusService, status);
}
#undef POKIT_INTERNAL_GET_SERVICE

/*!
 * \cond internal
 * \class PokitDevicePrivate
 *
 * The PokitDevicePrivate class provides private implementation for PokitDevice.
 */

/*!
 * Constructs a new PokitDevicePrivate object with public implementation \a q.
 */
PokitDevicePrivate::PokitDevicePrivate(PokitDevice * const q)
    : controller(nullptr), calibration(nullptr), dataLogger(nullptr), deviceInfo(nullptr),
      dso(nullptr), genericAccess(nullptr), multimeter(nullptr), status(nullptr), q_ptr(q)
{

}

/*!
 * Sets \a newController to be used for accessing Pokit devices.
 *
 * If a controller has already been set (and is not the same pointer), then the previous controller
 * will be disconnected, and replaced with \a newController.
 *
 * This function will not take ownership of the new controller. The caller is responsible for
 * ensuring that \a newContorller remains valid for the lifetime of this instance, or until this
 * function is used again to replace \a newController with another one (which may be a nullptr).
 *
 * \see controller
 * \see PokitDevice::controller()
 */
void PokitDevicePrivate::setController(QLowEnergyController * newController)
{
    if (newController == this->controller) {
        qCDebug(pokitController).noquote() << tr("Controller already set to:") << newController;
        return;
    }

    if (this->controller) {
        qCDebug(pokitController).noquote() << tr("Disconnecting signals from previous controller:")
            << controller;
        disconnect(this->controller, nullptr, this, nullptr);
    }

    qCDebug(pokitController).noquote() << tr("Setting new controller:") << newController;
    this->controller = newController;
    if (!newController) {
        return; // Don't bother continuing to connect if new controller is null.
    }

    qCDebug(pokitController).noquote() << tr("Set new controller \"%1\" (%2) at (%3).").arg(
        controller->remoteName(), controller->remoteDeviceUuid().toString(),
        controller->remoteAddress().toString());

    connect(controller, &QLowEnergyController::connected,
            this, &PokitDevicePrivate::connected);

    connect(controller, &QLowEnergyController::connectionUpdated,
            this, &PokitDevicePrivate::connectionUpdated);

    connect(controller, &QLowEnergyController::disconnected,
            this, &PokitDevicePrivate::disconnected);

    connect(controller, &QLowEnergyController::discoveryFinished,
            this, &PokitDevicePrivate::discoveryFinished);


    connect(controller,
    #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
        QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
    #else
        &QLowEnergyController::errorOccurred,
    #endif
        this, &PokitDevicePrivate::errorOccurred);


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
    qCDebug(pokitController).noquote() << tr("Connected to \"%1\" (%2) at (%3).").arg(
        controller->remoteName(), controller->remoteDeviceUuid().toString(),
        controller->remoteAddress().toString());
}

/*!
 * Handle connectionUpdated signals.
 */
void PokitDevicePrivate::connectionUpdated(const QLowEnergyConnectionParameters &newParameters)
{
    qCDebug(pokitController).noquote() << tr("Connection updated:") << newParameters.latency()
        << newParameters.minimumInterval() << newParameters.maximumInterval()
        << newParameters.supervisionTimeout();
}

/*!
 * Handle disconnected signals.
 */
void PokitDevicePrivate::disconnected()
{
    qCDebug(pokitController).noquote() << tr("Device disconnected.");
}

/*!
 * Handle discoveryFinished signals.
 */
void PokitDevicePrivate::discoveryFinished()
{
    qCDebug(pokitController).noquote() << tr("Service discovery finished.");
}

/*!
 * Handle error signals.
 */
void PokitDevicePrivate::errorOccurred(QLowEnergyController::Error newError)
{
    qCDebug(pokitController).noquote() << tr("Controller error:") << newError;
}

/*!
 * Handle serviceDiscovered signals.
 */
void PokitDevicePrivate::serviceDiscovered(const QBluetoothUuid &newService)
{
    qCDebug(pokitController).noquote() << tr("Service discovered:") << newService;
}

/*!
 * Handle stateChanged signals.
 */
void PokitDevicePrivate::stateChanged(QLowEnergyController::ControllerState state)
{
    qCDebug(pokitController).noquote() << tr("State changed to:") << state;
}

/// \endcond
