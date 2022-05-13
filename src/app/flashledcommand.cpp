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

QStringList FlashLedCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
    };
}

QStringList FlashLedCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser);
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
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a StatusService object.
 */
AbstractPokitService * FlashLedCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->status();
        Q_ASSERT(service);
        connect(service, &StatusService::deviceLedFlashed,
                this, &FlashLedCommand::deviceLedFlashed);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override flashes the device's LED, via the Pokit Status service.
 */
void FlashLedCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    qCInfo(lc).noquote() << tr("Flashing Pokit device LED...");
    if (!service->flashLed()) {
        QCoreApplication::exit(EXIT_FAILURE);
    }
}

/*!
 * Handles StatusService::deviceLedFlashed events, by outputting the result and exiting.
 */
void FlashLedCommand::deviceLedFlashed()
{
    switch (format) {
    case OutputFormat::Csv:
        fputs(qPrintable(tr("flash_led_result\nsuccess\n")), stdout);
        break;
    case OutputFormat::Json:
        fputs(qPrintable(QLatin1String("true\n")), stdout);
        break;
    case OutputFormat::Text:
        fputs(qPrintable(tr("Done.\n")), stdout);
        break;
    }
    disconnect(); // Will exit the application once disconnected.
}
