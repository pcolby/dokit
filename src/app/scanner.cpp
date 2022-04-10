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

#include "scanner.h"

#include <qtpokit/pokitdevicedisoveryagent.h>
#include <qtpokit/utils.h>
#include <qtpokit/uuids.h>

#include <QCoreApplication>
#include <QJsonDocument>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(pokitScanner, "pokit.ui.scanner", QtInfoMsg);

/*!
 * Construct a new scanner object with \a parent.
 */
Scanner::Scanner(QObject * const parent) : QObject(parent)
{
    discoveryAgent = new PokitDeviceDiscoveryAgent(this);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::pokitDeviceDiscovered,
            this, &Scanner::deviceDiscovered);

    connect(discoveryAgent, &PokitDeviceDiscoveryAgent::finished, this, []() {
        qCDebug(pokitScanner) << "Finished scanning for Pokit devices.";
        QCoreApplication::quit();
    });
}

/*!
 * Begins scanning for Pokit devices, with \a timeout, in milliseconds.
 */
void Scanner::start(const int timeout)
{
    Q_ASSERT(discoveryAgent);
    qCDebug(pokitScanner).noquote().nospace() << "Scanning for Pokit devices (with "
        << (timeout ? QLocale().toString(timeout)+QLatin1String("ms") : QLatin1String("no"))
        << " timeout).";
    discoveryAgent->setLowEnergyDiscoveryTimeout(timeout);
    discoveryAgent->start();
}

/*!
 * Handles discovered Pokit devices, writing \a info to stdout.
 *
 * \todo Support alternative output formats: text, json, other?
 */
void Scanner::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    fputs(QJsonDocument(toJson(info)).toJson(), stdout);
}
