// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testpokitdiscoveryagent.h"

#include <qtpokit/pokitdiscoveryagent.h>

#include <QBluetoothUuid>

#define POKIT_STATUS_SERVICE_UUID QLatin1String("57d3a771-267c-4394-8872-78223e92aec5")

// QBluetoothDeviceInfo::setServiceUuids deprecated the completeness argument in Qt 5.13.
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
#define DATA_COMPLETENESS , QBluetoothDeviceInfo::DataUnavailable
#else
#define DATA_COMPLETENESS
#endif

void TestPokitDiscoveryAgent::isPokitDevice_data()
{
    QTest::addColumn<QBluetoothDeviceInfo>("info");
    QTest::addColumn<bool>("expected");

    QTest::addRow("invalid") << QBluetoothDeviceInfo() << false;

    QBluetoothDeviceInfo info(QBluetoothAddress(QLatin1String("11:22:33:44:55:66")), QLatin1String("info"), 0);
    QTest::addRow("unset") << info << false;

    info.setServiceUuids({} DATA_COMPLETENESS);
    QTest::addRow("empty") << info << false;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid())} DATA_COMPLETENESS);
    QTest::addRow("random") << info << false;

    info.setServiceUuids({ QBluetoothUuid(POKIT_STATUS_SERVICE_UUID) } DATA_COMPLETENESS);
    QTest::addRow("status") << info << true;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid()), QBluetoothUuid(QUuid::createUuid())} DATA_COMPLETENESS);
    QTest::addRow("two") << info << false;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid()), QBluetoothUuid(POKIT_STATUS_SERVICE_UUID)} DATA_COMPLETENESS);
    QTest::addRow("both") << info << true;
}

void TestPokitDiscoveryAgent::isPokitDevice()
{
    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(bool, expected);
    QCOMPARE(PokitDiscoveryAgent::isPokitDevice(info), expected);
}

QTEST_MAIN(TestPokitDiscoveryAgent)
