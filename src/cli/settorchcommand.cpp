// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "settorchcommand.h"

#include <qtpokit/pokitdevice.h>

#include <iostream>

/*!
 * \class SetTorchCommand
 *
 * The SetTorchCommand class implements the `set-torch` CLI command.
 */

/*!
 * Construct a new SetTorchCommand object with \a parent.
 */
SetTorchCommand::SetTorchCommand(QObject * const parent) : DeviceCommand(parent)
{

}

QStringList SetTorchCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
        QLatin1String("mode"),
    };
}

QStringList SetTorchCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser);
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList SetTorchCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = DeviceCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    if (const QString value = parser.value(QLatin1String("mode"));
        value.trimmed().compare(QLatin1String("on"), Qt::CaseInsensitive) == 0) {
        newStatus = StatusService::TorchStatus::On;
    } else if (value.trimmed().compare(QLatin1String("off"), Qt::CaseInsensitive) == 0) {
        newStatus = StatusService::TorchStatus::Off;
    } else {
        errors.append(tr("Invalid status value: %1").arg(value));
    }
    return errors;
}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a StatusService object.
 */
AbstractPokitService * SetTorchCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->status();
        Q_ASSERT(service);
        connect(service, &StatusService::torchStatusWritten,
                this, &SetTorchCommand::torchStatusWritten);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override sets the device's name, via the Pokit Status service.
 */
void SetTorchCommand::serviceDetailsDiscovered()
{
    qCInfo(lc).noquote() << tr("Setting torch %1").arg(StatusService::toString(newStatus).toLower());
    if (!service->setTorchStatus(newStatus)) {
        QCoreApplication::exit(EXIT_FAILURE);
    }
}

/*!
 * Handles StatusService::torchStatusWritten events, by outputting the result and exiting.
 */
void SetTorchCommand::torchStatusWritten()
{
    switch (format) {
    case OutputFormat::Csv:
        std::cout << qUtf8Printable(tr("set_torch_result\nsuccess\n"));
        break;
    case OutputFormat::Json:
        std::cout << qUtf8Printable(QLatin1String("true\n"));
        break;
    case OutputFormat::Text:
        std::cout << qUtf8Printable(tr("Done.\n"));
        break;
    }
    if (device) disconnect(); // Will exit the application once disconnected.
}
