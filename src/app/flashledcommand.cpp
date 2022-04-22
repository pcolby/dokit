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

#include "flashledcommand.h"

#include <qtpokit/pokitdevice.h>
#include <qtpokit/statusservice.h>

/*!
 * \class FlashLedCommand
 *
 * The FlashLedCommand class implements the `flash-led` CLI command.
 */

/*!
 * Construct a new FlashLedCommand object with \a parent.
 */
FlashLedCommand::FlashLedCommand(QObject * const parent) : DeviceCommand(parent), service(nullptr)
{

}

QStringList FlashLedCommand::requiredOptions() const
{
    return DeviceCommand::requiredOptions() + QStringList{
        QLatin1String("device"),
    };
}

QStringList FlashLedCommand::supportedOptions() const
{
    return DeviceCommand::supportedOptions();
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList FlashLedCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = DeviceCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    return errors;
}

/*!
 * Begins scanning for Pokit devices.
 */
bool FlashLedCommand::start()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->status();
        Q_ASSERT(service);
        connect(service, &StatusService::serviceDetailsDiscovered,
                this, &FlashLedCommand::serviceDetailsDiscovered);
    }
    qCDebug(lc).noquote() << tr("Connecting to device...");
    device->controller()->connectToDevice();
    return true;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override flashes the device's LED, via the Pokit Status service.
 */
void FlashLedCommand::serviceDetailsDiscovered()
{
    /// \todo service->flashLed(); then wait for the result signal.
    qCWarning(lc) << "Not implemented yet";
    QCoreApplication::exit(EXIT_FAILURE);
}
