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

#include "setnamecommand.h"

#include <qtpokit/pokitdevice.h>
#include <qtpokit/statusservice.h>

/*!
 * \class SetNameCommand
 *
 * The SetNameCommand class implements the `set-name` CLI command.
 */

/*!
 * Construct a new SetNameCommand object with \a parent.
 */
SetNameCommand::SetNameCommand(QObject * const parent) : DeviceCommand(parent), service(nullptr)
{

}

QStringList SetNameCommand::requiredOptions() const
{
    return DeviceCommand::requiredOptions() + QStringList{
        QLatin1String("device"),
    };
}

QStringList SetNameCommand::supportedOptions() const
{
    return DeviceCommand::supportedOptions();
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList SetNameCommand::processOptions(const QCommandLineParser &parser)
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
bool SetNameCommand::start()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->status();
        Q_ASSERT(service);
        connect(service, &StatusService::serviceDetailsDiscovered,
                this, &SetNameCommand::serviceDetailsDiscovered);
    }
    qCDebug(lc).noquote() << tr("Connecting to device...");
    device->controller()->connectToDevice();
    return true;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override sets the device's name, via the Pokit Status service.
 */
void SetNameCommand::serviceDetailsDiscovered()
{
    /// \todo service->setDeviceName(...); then wait for the result signal.
    qCWarning(lc) << "Not implemented yet";
    QCoreApplication::exit(EXIT_FAILURE);
}
