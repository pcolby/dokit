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
#include <qtpokit/uuids.h>

#include <QCoreApplication>
#include <QJsonDocument>
#include <QLoggingCategory>

/*!
 * Construct a new ScanCommand object with \a parent.
 */
ScanCommand::ScanCommand(QObject * const parent) : AbstractCommand(parent)
{
    discoveryAgent = new PokitDeviceDiscoveryAgent(this);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::pokitDeviceDiscovered,
            this, &ScanCommand::deviceDiscovered);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::finished, this, [this]() {
        qCDebug(lc) << "Finished scanning for Pokit devices.";
        QCoreApplication::quit();
    });
}

QStringList ScanCommand::requiredOptions() const
{
    return AbstractCommand::requiredOptions();
}

QStringList ScanCommand::supportedOptions() const
{
    return AbstractCommand::supportedOptions();
}

QStringList ScanCommand::processOptions(const QCommandLineParser &parser)
{
    Q_ASSERT(discoveryAgent);

    QStringList errors = AbstractCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    if (parser.isSet(QLatin1String("timeout"))) {
        /// \todo Validate the value format.
        discoveryAgent->setLowEnergyDiscoveryTimeout(
            parser.value(QStringLiteral("timeout")).toInt()*1000);
        qCDebug(lc) << tr("Set scan timeout to %1").arg(discoveryAgent->lowEnergyDiscoveryTimeout());
    }

    return errors;
}

/*!
 * Begins scanning for Pokit devices.
 */
bool ScanCommand::start()
{
    Q_ASSERT(discoveryAgent);
    qCDebug(lc).noquote().nospace() << tr("Scanning for Pokit devices...");
    discoveryAgent->start();
    return true;
}

/*!
 * Handles discovered Pokit devices, writing \a info to stdout.
 *
 * \todo Support alternative output formats: text, json, other?
 */
void ScanCommand::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    fputs(QJsonDocument(toJson(info)).toJson(), stdout);
}
