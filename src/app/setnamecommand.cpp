// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

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

QStringList SetNameCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
        QLatin1String("new-name"),
    };
}

QStringList SetNameCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser);
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

    newName = parser.value(QLatin1String("new-name"));
    if (newName.isEmpty()) {
        errors.append(tr("New name cannot be empty."));
    } else if (newName.length() > 11) {
        errors.append(tr("New name cannot exceed 11 characters."));
    }
    return errors;
}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a StatusService object.
 */
AbstractPokitService * SetNameCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->status();
        Q_ASSERT(service);
        connect(service, &StatusService::deivceNameWritten,
                this, &SetNameCommand::deivceNameWritten);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override sets the device's name, via the Pokit Status service.
 */
void SetNameCommand::serviceDetailsDiscovered()
{
    qCInfo(lc).noquote() << tr("Setting device name to: %1").arg(newName);
    if (!service->setDeviceName(newName)) {
        QCoreApplication::exit(EXIT_FAILURE);
    }
}

/*!
 * Handles StatusService::deivceNameWritten events, by outputting the result and exiting.
 */
void SetNameCommand::deivceNameWritten()
{
    switch (format) {
    case OutputFormat::Csv:
        fputs(qPrintable(tr("set_name_result\nsuccess\n")), stdout);
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
