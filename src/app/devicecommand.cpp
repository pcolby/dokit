/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "devicecommand.h"

#include <qtpokit/abstractpokitservice.h>
#include <qtpokit/pokitdevice.h>
#include <qtpokit/pokitdiscoveryagent.h>
#include <QRegularExpression>

/*!
 * \class DeviceCommand
 *
 * The AbstractCommand class extends AbstractCommand to add a PokitDevice instance.
 */

/*!
 * Construct a new DeviceCommand object with \a parent.
 */
DeviceCommand::DeviceCommand(QObject * const parent) : AbstractCommand(parent), device(nullptr)
{

}

/*!
 * \copybrief AbstractCommand::processOptions
 *
 * This implementation extends AbstractCommand::processOptions to process device related CLI
 * opitons (epsecially `--device`).
 */
QStringList DeviceCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = AbstractCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }
    return errors;
}

/*!
 * Begins scanning for the Pokit device.
 */
bool DeviceCommand::start()
{
    qCInfo(lc).noquote() << ((deviceToScanFor.isNull())
        ? tr("Looking first available Pokit device...")
        : tr("Looking for device \"%1\"...").arg(deviceToScanFor));
    discoveryAgent->start();
    return true;
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
        connect(device->controller(),
            #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
            QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
            #else
            &QLowEnergyController::errorOccurred,
            #endif
            this, &DeviceCommand::controllerError, Qt::QueuedConnection);

        AbstractPokitService * const service = getService();

        Q_ASSERT(service);
        connect(service, &AbstractPokitService::serviceDetailsDiscovered,
                this, &DeviceCommand::serviceDetailsDiscovered);
        connect(service, &AbstractPokitService::serviceErrorOccurred,
                this, &DeviceCommand::serviceError);

        qCDebug(lc).noquote() << tr("Connecting to Pokit device \"%1\" (%2) at (%3).")
            .arg(info.name(), info.deviceUuid().toString(), info.address().toString());
        device->controller()->connectToDevice();
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
