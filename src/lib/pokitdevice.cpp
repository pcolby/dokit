// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
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
     multimeter() and status()); and
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
 * Constructs a new Pokit device controller wrapper with \a parent, and private implementation \a d.
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
#define QTPOKIT_INTERNAL_GET_SERVICE(typeName, varName) \
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
    QTPOKIT_INTERNAL_GET_SERVICE(CalibrationService, calibration);
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
    QTPOKIT_INTERNAL_GET_SERVICE(DataLoggerService, dataLogger);
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
    QTPOKIT_INTERNAL_GET_SERVICE(DeviceInfoService, deviceInfo);
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
    QTPOKIT_INTERNAL_GET_SERVICE(DsoService, dso);
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
    QTPOKIT_INTERNAL_GET_SERVICE(MultimeterService, multimeter);
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
    QTPOKIT_INTERNAL_GET_SERVICE(StatusService, status);
}
#undef QTPOKIT_INTERNAL_GET_SERVICE

/*!
 * Returns a human-readable name for the \a uuid service, or a null QString if unknonw.
 *
 * This is equivalent to QBluetoothUuid::serviceClassToString() but for services provided by Pokit
 * devices.
 */
QString PokitDevice::serviceToString(const QBluetoothUuid &uuid)
{
    static const QHash<QBluetoothUuid, QString> hash{
        { CalibrationService::serviceUuid, tr("Calibration") },
        { DataLoggerService::serviceUuid,  tr("Data Logger") },
        { DsoService::serviceUuid,         tr("DSO") },
        { MultimeterService::serviceUuid,  tr("Multimeter") },
        { StatusService::ServiceUuids::pokitMeter, tr("Status (Pokit Meter)") },
        { StatusService::ServiceUuids::pokitPro,   tr("Status (Pokit Pro)") },
        { DeviceInfoService::serviceUuid,
            QBluetoothUuid::serviceClassToString(QBluetoothUuid::ServiceClassUuid::DeviceInformation) },

        // The following are not specifically supported by this library, but strings provided for nicer debug output.
        { QBluetoothUuid::ServiceClassUuid::GenericAccess,
            QBluetoothUuid::serviceClassToString(QBluetoothUuid::ServiceClassUuid::GenericAccess) },
        { QBluetoothUuid::ServiceClassUuid::GenericAttribute,
            QBluetoothUuid::serviceClassToString(QBluetoothUuid::ServiceClassUuid::GenericAttribute) },
        { QBluetoothUuid(QStringLiteral("1d14d6ee-fd63-4fa1-bfa4-8f47b42119f0")), tr("OTA Firmware Update") },
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
    static const QHash<QBluetoothUuid, QString> hash{
        { CalibrationService::CharacteristicUuids::temperature, tr("Temperature") },
        { CalibrationService::CharacteristicUuids::getParam,    tr("Get Param") },
        { CalibrationService::CharacteristicUuids::setParam,    tr("Set Param") },

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
        { StatusService::CharacteristicUuids::torch,                 tr("Torch") },
        { StatusService::CharacteristicUuids::buttonPress,           tr("Button Press") },

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
        { DeviceInfoService::CharacteristicUuids::serialNumber,
            QBluetoothUuid::characteristicToString(QBluetoothUuid::CharacteristicType::SerialNumberString) },

        // The next two are not specifically supported by this library, but strings provided for nicer debug output.
        { QBluetoothUuid(QStringLiteral("f7bf3564-fb6d-4e53-88a4-5e37e0326063")), tr("OTA Control") },
        { QBluetoothUuid(QStringLiteral("984227f3-34fc-4045-a5d0-2c581f81a153")), tr("OTA Data Transfer") },
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
PokitDevicePrivate::PokitDevicePrivate(PokitDevice * const q) : q_ptr(q)
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

    qCDebug(lc).noquote() << tr(R"(Set new controller "%1" (%2) at (%3).)").arg(
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
void PokitDevicePrivate::connected() const
{
    if (controller == nullptr) {
        qCCritical(lc).noquote() << tr("PokitDevicePrivate::connected slot invoked without a controller.");
        return; // Just to avoid the nullptr dereference below.
    }
    qCDebug(lc).noquote() << tr(R"(Connected to "%1" (%2) at (%3).)").arg(
        controller->remoteName(), controller->remoteDeviceUuid().toString(),
        controller->remoteAddress().toString());
}

/*!
 * Handle connectionUpdated signals.
 */
void PokitDevicePrivate::connectionUpdated(const QLowEnergyConnectionParameters &newParameters) const
{
    qCDebug(lc).noquote() << tr("Connection updated:") << newParameters.latency()
        << newParameters.minimumInterval() << newParameters.maximumInterval()
        << newParameters.supervisionTimeout();
}

/*!
 * Handle disconnected signals.
 */
void PokitDevicePrivate::disconnected() const
{
    qCDebug(lc).noquote() << tr("Device disconnected.");
}

/*!
 * Handle discoveryFinished signals.
 */
void PokitDevicePrivate::discoveryFinished() const
{
    qCDebug(lc).noquote() << tr("Service discovery finished.");
}

/*!
 * Handle error signals.
 */
void PokitDevicePrivate::errorOccurred(QLowEnergyController::Error newError) const
{
    qCDebug(lc).noquote() << tr("Controller error:") << newError;
}

/*!
 * Handle serviceDiscovered signals.
 */
void PokitDevicePrivate::serviceDiscovered(const QBluetoothUuid &newService) const
{
    qCDebug(lc).noquote() << tr(R"(Service discovered: %1 "%2")")
        .arg(newService.toString(), PokitDevice::serviceToString(newService));
}

/*!
 * Handle stateChanged signals.
 */
void PokitDevicePrivate::stateChanged(QLowEnergyController::ControllerState state) const
{
    qCDebug(lc).noquote() << tr("State changed to:") << state;
}

/// \endcond
