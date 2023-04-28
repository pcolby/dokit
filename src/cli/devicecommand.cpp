// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/abstractpokitservice.h>
#include <qtpokit/pokitdevice.h>
#include <qtpokit/pokitdiscoveryagent.h>

/*!
 * \class DeviceCommand
 *
 * The AbstractCommand class extends AbstractCommand to add a PokitDevice instance.
 */

/*!
 * Construct a new DeviceCommand object with \a parent.
 */
DeviceCommand::DeviceCommand(QObject * const parent) : AbstractCommand(parent)
{

}

/*!
 * Begins scanning for the Pokit device.
 */
bool DeviceCommand::start()
{
    qCInfo(lc).noquote() << ((deviceToScanFor.isNull())
        ? tr("Looking for first available Pokit device...")
        : tr("Looking for device \"%1\"...").arg(deviceToScanFor));
    discoveryAgent->start();
    return true;
}

/*!
 * Disconnects the underlying Pokit device, and sets \a exitCode to be return to the OS once the
 * disconnection has taken place.
 */
void DeviceCommand::disconnect(int exitCode)
{
    qCDebug(lc).noquote() << tr("Disconnecting Pokit device...");
    Q_ASSERT(device);
    Q_ASSERT(device->controller());
    exitCodeOnDisconnect = exitCode;
    device->controller()->disconnectFromDevice();
}

/*!
 * \fn virtual AbstractPokitService * DeviceCommand::getService() = 0
 *
 * Returns a Pokit service object for the derived command class. This will be called by
 * deviceDiscovered() when the requested Pokit device has been found, after which
 * deviceDiscovered() will connect the returned service's common signals, and kick off the
 * device's connection process.
 */

/*!
 * Handles controller error events. This base implementation simply logs \a error and then exits
 * with `EXIT_FAILURE`. Derived classes may override this slot to implement their own error
 * handing if desired.
 */
void DeviceCommand::controllerError(QLowEnergyController::Error error)
{
    qCWarning(lc).noquote() << tr("Bluetooth controller error:") << error;
    QCoreApplication::exit(EXIT_FAILURE);
}

/*!
 * Handles devics disconnection events. This base implementation simply logs and exits the
 * application (via QCoreApplication::exit) with the current exitCodeOnDisconnect value, which is
 * initialise to `EXIT_FAILURE` in the constructor, but should be set to `EXIT_SUCESS` if/when
 * the derived command class has completed its actions and requested the disconnection (as opposed
 * to a spontaneous disconnection on error).
 */
void DeviceCommand::deviceDisconnected()
{
    qCDebug(lc).noquote() << tr("Pokit device disconnected. Exiting with code %1.")
        .arg(exitCodeOnDisconnect);
    QCoreApplication::exit(exitCodeOnDisconnect);
}

/*!
 * Handles service error events. This base implementation simply logs \a error and then exits
 * with `EXIT_FAILURE`. Derived classes may override this slot to implement their own error
 * handing if desired.
 *
 * \note As this base class does not construct services (derived classed do), its up to the derived
 * classed to connect this slot to the relevant service's error signal if desired.
 */
void DeviceCommand::serviceError(const QLowEnergyService::ServiceError error)
{
    qCWarning(lc).noquote() << tr("Bluetooth service error:") << error;
    QCoreApplication::exit(EXIT_FAILURE);
}

/*!
 * Handles service detail discovery events. This base implementation simply logs the event, and
 * nothing more. Derived classes may (usually do) override this slot to provide their own processing
 * when a services details have been discovered.
 */
void DeviceCommand::serviceDetailsDiscovered()
{
    qCDebug(lc).noquote() << tr("Service details discovered.");
}

/*!
 * Checks if \a info is the device (if any) we're looking for, and if so, create a contoller and
 * service, and begins connecting to the device.
 */
void DeviceCommand::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    if (device) {
        qCDebug(lc).noquote() << tr("Ignoring additional Pokit device \"%1\" (%2) at (%3).")
            .arg(info.name(), info.deviceUuid().toString(), info.address().toString());
        return;
    }

    if ((deviceToScanFor.isEmpty()) || (deviceToScanFor == info.name()) ||
        ((!info.address().isNull()) && (info.address() == QBluetoothAddress(deviceToScanFor))) ||
        ((!info.deviceUuid().isNull()) && (info.deviceUuid() == QBluetoothUuid(deviceToScanFor))))
    {
        qCDebug(lc).noquote() << tr("Found Pokit device \"%1\" (%2) at (%3).")
            .arg(info.name(), info.deviceUuid().toString(), info.address().toString());
        discoveryAgent->stop();

        device = new PokitDevice(info, this);
        connect(device->controller(), &QLowEnergyController::disconnected,
                this, &DeviceCommand::deviceDisconnected);
        connect(device->controller(),
            #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
            QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
            #else
            &QLowEnergyController::errorOccurred,
            #endif
            this, &DeviceCommand::controllerError, Qt::QueuedConnection);

        const AbstractPokitService * const service = getService();

        Q_ASSERT(service);
        connect(service, &AbstractPokitService::serviceDetailsDiscovered,
                this, &DeviceCommand::serviceDetailsDiscovered);
        connect(service, &AbstractPokitService::serviceErrorOccurred,
                this, &DeviceCommand::serviceError);

        qCDebug(lc).noquote() << tr("Connecting to Pokit device \"%1\" (%2) at (%3).")
            .arg(info.name(), info.deviceUuid().toString(), info.address().toString());
        device->controller()->connectToDevice();
        return;
    }

    qCDebug(lc).noquote() << tr("Ignoring non-matching Pokit device \"%1\" (%2) at (%3).")
        .arg(info.name(), info.deviceUuid().toString(), info.address().toString());
    return;
}

/*!
 * Checks that the requested device was discovered, and if not, reports and error and exits.
 */
void DeviceCommand::deviceDiscoveryFinished()
{
    if (!device) {
        qCWarning(lc).noquote() << ((deviceToScanFor.isNull())
            ? tr("Failed to find any Pokit device.")
            : tr("Failed to find device \"%1\".").arg(deviceToScanFor));
        QCoreApplication::exit(EXIT_FAILURE);
    }
}
