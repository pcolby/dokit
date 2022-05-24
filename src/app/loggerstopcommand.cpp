// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "loggerstopcommand.h"

#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

/*!
 * \class LoggerStopCommand
 *
 * The LoggerStopCommand class implements the `logger stop` CLI command.
 */

/*!
 * Construct a new LoggerStopCommand object with \a parent.
 */
LoggerStopCommand::LoggerStopCommand(QObject * const parent)
    : DeviceCommand(parent), service(nullptr)
{

}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a DataLoggerService object.
 */
AbstractPokitService * LoggerStopCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->dataLogger();
        Q_ASSERT(service);
        connect(service, &DataLoggerService::settingsWritten,
        this, &LoggerStopCommand::settingsWritten);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override stops the device's logger.
 */
void LoggerStopCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    qCInfo(lc).noquote() << tr("Stopping logger.");
    service->stopLogger();
}

/*!
 * Invoked when the data logger settings have been written.
 */
void LoggerStopCommand::settingsWritten()
{
    qCDebug(lc).noquote() << tr("Settings written; data logger has stopped.");
    switch (format) {
    case OutputFormat::Csv:
        fputs(qPrintable(tr("logger_start_result\nsuccess\n")), stdout);
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
