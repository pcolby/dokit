/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "testpokitdiscoveryagent.h"

#include <qtpokit/pokitdiscoveryagent.h>

#include <QBluetoothUuid>

#define POKIT_STATUS_SERVICE_UUID QLatin1String("57d3a771-267c-4394-8872-78223e92aec5")

void TestPokitDiscoveryAgent::isPokitDevice_data()
{
    QTest::addColumn<QBluetoothDeviceInfo>("info");
    QTest::addColumn<bool>("expected");

    QTest::addRow("invalid") << QBluetoothDeviceInfo() << false;

    QBluetoothDeviceInfo info(QBluetoothAddress(QLatin1String("11:22:33:44:55:66")), QLatin1String("info"), 0);
    QTest::addRow("unset") << info << false;

    info.setServiceUuids({});
    QTest::addRow("empty") << info << false;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid())});
    QTest::addRow("random") << info << false;

    info.setServiceUuids({ QBluetoothUuid(POKIT_STATUS_SERVICE_UUID) });
    QTest::addRow("status") << info << true;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid()), QBluetoothUuid(QUuid::createUuid())});
    QTest::addRow("two") << info << false;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid()), QBluetoothUuid(POKIT_STATUS_SERVICE_UUID)});
    QTest::addRow("both") << info << true;
}

void TestPokitDiscoveryAgent::isPokitDevice()
{
    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(bool, expected);
    QCOMPARE(PokitDiscoveryAgent::isPokitDevice(info), expected);
}

QTEST_MAIN(TestPokitDiscoveryAgent)
