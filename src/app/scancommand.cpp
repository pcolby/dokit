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

#include "scancommand.h"

#include <qtpokit/pokitdevicedisoveryagent.h>
#include <qtpokit/utils.h>

#include <QJsonDocument>

/*!
 * \class ScanCommand
 *
 * The ScanCommand class implements the `scan` CLI command, by scanning for nearby Pokit Bluetooth
 * devices. When devices are found, they are logged to stdout in the chosen format.
 */

/*!
 * Construct a new ScanCommand object with \a parent.
 */
ScanCommand::ScanCommand(QObject * const parent) : AbstractCommand(parent)
{
    /// \todo Support logging device update events too.
    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::pokitDeviceUpdated,
            this, &ScanCommand::deviceUpdated);
}

QStringList ScanCommand::requiredOptions() const
{
    return AbstractCommand::requiredOptions();
}

QStringList ScanCommand::supportedOptions() const
{
    return AbstractCommand::supportedOptions() + QStringList{
    };
}

/// \copydoc AbstractCommand::processOptions
QStringList ScanCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = AbstractCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    return errors;
}

/*!
 * Begins scanning for Pokit devices.
 */
bool ScanCommand::start()
{
    Q_ASSERT(discoveryAgent);
    qCInfo(lc).noquote() << tr("Scanning for Pokit devices...");
    discoveryAgent->start();
    return true;
}

/*!
 * Handles discovered Pokit devices, writing \a info to stdout.
 */
void ScanCommand::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    switch (format) {
    case OutputFormat::Csv:
        /// \todo Only show the header row once.
        fputs(qPrintable(tr("uuid,address,name,major_class,minor_class,signal_strength\n")), stdout);
        fputs(qPrintable(QString::fromLatin1("%1,%2,%3,%4,%5,%6\n").arg(info.deviceUuid().toString(),
            info.address().toString(), escapeCsvField(info.name()), toString(info.majorDeviceClass()),
            toString(info.majorDeviceClass(), info.minorDeviceClass())).arg(info.rssi())), stdout);
        break;
    case OutputFormat::Json:
        fputs(QJsonDocument(toJson(info)).toJson(), stdout);
        break;
    case OutputFormat::Text:
        fputs(qPrintable(tr("%1 %2 %3 %4\n").arg(info.deviceUuid().toString(),
            info.address().toString(), info.name()).arg(info.rssi())), stdout);
        break;
    }
}

/*!
 * Handles updated Pokit devices, writing \a info to stdout. Currently \a updatedFields us unused.
 */
void ScanCommand::deviceUpdated(const QBluetoothDeviceInfo &info,
                                const QBluetoothDeviceInfo::Fields updatedFields)
{
    Q_UNUSED(updatedFields)
    switch (format) {
    case OutputFormat::Csv:
        fputs(qPrintable(QString::fromLatin1("%1,%2,%3,%4,%5,%6\n").arg(info.deviceUuid().toString(),
            info.address().toString(), escapeCsvField(info.name()), toString(info.majorDeviceClass()),
            toString(info.majorDeviceClass(), info.minorDeviceClass())).arg(info.rssi())), stdout);
        break;
    case OutputFormat::Json:
        fputs(QJsonDocument(toJson(info)).toJson(), stdout);
        break;
    case OutputFormat::Text:
        fputs(qPrintable(tr("%1 %2 %3 %4\n").arg(info.deviceUuid().toString(),
            info.address().toString(), info.name()).arg(info.rssi())), stdout);
        break;
    }
}

/*!
 * Handles the completion of device discovery. In this override we simply exit, as the scan command
 * is nothing more than logging of discovered devices.
 */
void ScanCommand::deviceDiscoveryFinished()
{
    qCDebug(lc).noquote() << tr("Finished scanning for Pokit devices.");
    QCoreApplication::quit();
}
