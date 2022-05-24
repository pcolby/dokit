// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the PokitDevice and PokitDevicePrivate classes.
 */

#include <qtpokit/pokitdevice.h>

#include <qtpokit/calibrationservice.h>
#include <qtpokit/dataloggerservice.h>
#include <qtpokit/deviceinfoservice.h>
#include <qtpokit/dsoservice.h>
#include <qtpokit/genericaccessservice.h>
#include <qtpokit/multimeterservice.h>
#include <qtpokit/statusservice.h>

#include "pokitdevice_p.h"

#include <QMutexLocker>

/*!
 * \class PokitDevice
 *
 * The PokitDevice class simplifies Pokit device access.
 *
 * It does this by wrapping QLowEnergyController to provide:
 * * convenient Pokit service factory methods (dataLogger(), deviceInformation(), dso(),
 *   genericAccess(), multimeter() and status()); and
 * * consistent debug logging of QLowEnergyController events.
 *
 * But this class is entirely optional, in that all features of all other QtPokit classes can be
 * used wihtout this class.  It's just a (meaningful) convenience.
 */

/*!
 * Constructs a new Pokit device controller wrapper for \a deviceInfo, with \a parent.
 *
 * Though not strictly necessary, \a deviceInfo should normally come from a
 * PokitDiscoveryAgent instance (or a QBluetoothDeviceDiscoveryAgent), otherwise connection
 * is likely to fail with QLowEnergyController::UnknownRemoteDeviceError.
 */
PokitDevice::PokitDevice(const QBluetoothDeviceInfo &deviceInfo, QObject *parent)
    : QObject(parent), d_ptr(new PokitDevicePrivate(this))
{
    Q_D(PokitDevice);
    d->setController(QLowEnergyController::createCentral(deviceInfo, this));
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
 */
DataLoggerService * PokitDevice::dataLogger()
{
    POKIT_INTERNAL_GET_SERVICE(DataLoggerService, dataLogger);
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
 */
DsoService * PokitDevice::dso()
{
    POKIT_INTERNAL_GET_SERVICE(DsoService, dso);
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
 */
MultimeterService * PokitDevice::multimeter()
{
    POKIT_INTERNAL_GET_SERVICE(MultimeterService, multimeter);
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
 * Returns a human-readable name for the \a uuid service, or a null QString if unknonw.
 *
 * This is equivalent to QBluetoothUuid::serviceClassToString() but for services provided by Pokit
 * devices.
 */
QString PokitDevice::serviceToString(const QBluetoothUuid &uuid)
{
    static QHash<QBluetoothUuid, QString> hash{
        { CalibrationService::serviceUuid, tr("Calibration") },
        { DataLoggerService::serviceUuid,  tr("Data Logger") },
        { DsoService::serviceUuid,         tr("DSO") },
        { MultimeterService::serviceUuid,  tr("Multimeter") },
        { StatusService::ServiceUuids::pokitMeter, tr("Status (Pokit Meter)") },
        { StatusService::ServiceUuids::pokitPro,   tr("Status (Pokit Pro)") },
        { DeviceInfoService::serviceUuid,
            QBluetoothUuid::serviceClassToString(QBluetoothUuid::ServiceClassUuid::DeviceInformation) },
        { GenericAccessService::serviceUuid,
            QBluetoothUuid::serviceClassToString(QBluetoothUuid::ServiceClassUuid::GenericAccess) },
    };
    return hash.value(uuid);
}

/*!
 * Returns a human-readable name for the \a uuid characteristic, or a null QString if unknown.
 *
 * This is equivalent to QBluetoothUuid::characteristicToString() but for characteristics provided
 * by Pokit devices.
 */
QString PokitDevice::charcteristicToString(const QBluetoothUuid &uuid)
{
    static QHash<QBluetoothUuid, QString> hash{
        { CalibrationService::CharacteristicUuids::temperature, tr("Temperature") },

        { DataLoggerService::CharacteristicUuids::metadata, tr("Metadata") },
        { DataLoggerService::CharacteristicUuids::reading,  tr("Reading") },
        { DataLoggerService::CharacteristicUuids::settings, tr("Settings") },

        { DsoService::CharacteristicUuids::metadata, tr("Metadata") },
        { DsoService::CharacteristicUuids::reading,  tr("Reading") },
        { DsoService::CharacteristicUuids::settings, tr("Settings") },

        { MultimeterService::CharacteristicUuids::reading,  tr("Reading") },
        { MultimeterService::CharacteristicUuids::settings, tr("Settings") },

        { StatusService::CharacteristicUuids::deviceCharacteristics, tr("Device Characteristics") },
        { StatusService::CharacteristicUuids::flashLed,              tr("Flash LED") },
        { StatusService::CharacteristicUuids::name,                  tr("Name") },
        { StatusService::CharacteristicUuids::status,                tr("Status") },

        { DeviceInfoService::CharacteristicUuids::firmwareRevision,
            QBluetoothUuid::characteristicToString(QBluetoothUuid::CharacteristicType::FirmwareRevisionString) },
        { DeviceInfoService::CharacteristicUuids::hardwareRevision,
            QBluetoothUuid::characteristicToString(QBluetoothUuid::CharacteristicType::HardwareRevisionString) },
        { DeviceInfoService::CharacteristicUuids::manufacturerName,
            QBluetoothUuid::characteristicToString(QBluetoothUuid::CharacteristicType::ManufacturerNameString) },
        { DeviceInfoService::CharacteristicUuids::modelNumber,
            QBluetoothUuid::characteristicToString(QBluetoothUuid::CharacteristicType::ModelNumberString) },
        { DeviceInfoService::CharacteristicUuids::softwareRevision,
            QBluetoothUuid::characteristicToString(QBluetoothUuid::CharacteristicType::SoftwareRevisionString) },

        { GenericAccessService::CharacteristicUuids::appearance,
            QBluetoothUuid::characteristicToString(QBluetoothUuid::CharacteristicType::Appearance) },
        { GenericAccessService::CharacteristicUuids::deviceName,
            QBluetoothUuid::characteristicToString(QBluetoothUuid::CharacteristicType::DeviceName) },
    };
    return hash.value(uuid);
}

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
        qCDebug(lc).noquote() << tr("Controller already set to:") << newController;
        return;
    }

    if (this->controller) {
        qCDebug(lc).noquote() << tr("Disconnecting signals from previous controller:")
            << controller;
        disconnect(this->controller, nullptr, this, nullptr);
    }

    qCDebug(lc).noquote() << tr("Setting new controller:") << newController;
    this->controller = newController;
    if (!newController) {
        return; // Don't bother continuing to connect if new controller is null.
    }

    qCDebug(lc).noquote() << tr("Set new controller \"%1\" (%2) at (%3).").arg(
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
    qCDebug(lc).noquote() << tr("Connected to \"%1\" (%2) at (%3).").arg(
        controller->remoteName(), controller->remoteDeviceUuid().toString(),
        controller->remoteAddress().toString());
}

/*!
 * Handle connectionUpdated signals.
 */
void PokitDevicePrivate::connectionUpdated(const QLowEnergyConnectionParameters &newParameters)
{
    qCDebug(lc).noquote() << tr("Connection updated:") << newParameters.latency()
        << newParameters.minimumInterval() << newParameters.maximumInterval()
        << newParameters.supervisionTimeout();
}

/*!
 * Handle disconnected signals.
 */
void PokitDevicePrivate::disconnected()
{
    qCDebug(lc).noquote() << tr("Device disconnected.");
}

/*!
 * Handle discoveryFinished signals.
 */
void PokitDevicePrivate::discoveryFinished()
{
    qCDebug(lc).noquote() << tr("Service discovery finished.");
}

/*!
 * Handle error signals.
 */
void PokitDevicePrivate::errorOccurred(QLowEnergyController::Error newError)
{
    qCDebug(lc).noquote() << tr("Controller error:") << newError;
}

/*!
 * Handle serviceDiscovered signals.
 */
void PokitDevicePrivate::serviceDiscovered(const QBluetoothUuid &newService)
{
    qCDebug(lc).noquote() << tr("Service discovered: %1 \"%2\"")
        .arg(newService.toString(), PokitDevice::serviceToString(newService));
}

/*!
 * Handle stateChanged signals.
 */
void PokitDevicePrivate::stateChanged(QLowEnergyController::ControllerState state)
{
    qCDebug(lc).noquote() << tr("State changed to:") << state;
}

/// \endcond
