// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "flashledcommand.h"
#include "../stringliterals_p.h"

#include <qtpokit/pokitdevice.h>
#include <qtpokit/statusservice.h>

#include <iostream>

DOKIT_USE_STRINGLITERALS

/*!
 * \class FlashLedCommand
 *
 * The FlashLedCommand class implements the `flash-led` CLI command.
 */

/*!
 * Construct a new FlashLedCommand object with \a parent.
 */
FlashLedCommand::FlashLedCommand(QObject * const parent) : DeviceCommand(parent)
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
        std::cout << qUtf8Printable(tr("flash_led_result\nsuccess\n"));
        break;
    case OutputFormat::Json:
        std::cout << qUtf8Printable(u"true\n"_s);
        break;
    case OutputFormat::Text:
        std::cout << qUtf8Printable(tr("Done.\n"));
        break;
    }
    if (device) disconnect(); // Will exit the application once disconnected.
}
