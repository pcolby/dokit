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

#include <qtpokit/pokitdevice.h>
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

QStringList DeviceCommand::requiredOptions() const
{
    return AbstractCommand::requiredOptions() + QStringList{
        QLatin1String("device"),
    };
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

    // Parse the device option (if supported, and supplied); must be either a MAC address or UUID.
    Q_ASSERT(requiredOptions().contains(QLatin1String("device")));
    Q_ASSERT(parser.isSet(QLatin1String("device")));
    /// \todo Probably should warn if using MAC on OSX (since its not supported by the OS?).
    const QString deviceString = parser.value(QLatin1String("device"));
    static QRegularExpression pattern(QLatin1String("^("
        "[0-9a-fA-F]{12}|"
        "([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}|"
        "[0-9a-fA-F]{8}(-[0-9a-fA-F]{4}){3}-[0-9a-fA-F]{12}|"
        "\\{[0-9a-fA-F]{8}(-[0-9a-fA-F]{4}){3}-[0-9a-fA-F]{12}\\}"
    ")$"));
    if (!pattern.match(deviceString).hasMatch()) {
        errors.append(tr("Unrecogised device name or address: %1")
            .arg(deviceString));
    } else {
        /// \todo Not sure if we should consruct here, or just record the address for later?
        device = new PokitDevice(deviceString, this);
        connect(device->controller(),
            #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
            QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
            #else
            &QLowEnergyController::errorOccurred,
            #endif
            this, &DeviceCommand::controllerError, Qt::QueuedConnection);
    }

    return errors;
}

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
