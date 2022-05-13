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
