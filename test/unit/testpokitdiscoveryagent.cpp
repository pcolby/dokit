// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testpokitdiscoveryagent.h"

#include <qtpokit/pokitdiscoveryagent.h>
#include "pokitdiscoveryagent_p.h"

#include <QBluetoothUuid>
#include <QRegularExpression>
#include <QSignalSpy>

#define POKIT_METER_STATUS_SERVICE_UUID QLatin1String("57d3a771-267c-4394-8872-78223e92aec4")
#define POKIT_PRO_STATUS_SERVICE_UUID   QLatin1String("57d3a771-267c-4394-8872-78223e92aec5")

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

    QBluetoothDeviceInfo info(QBluetoothAddress(QLatin1String("11:22:33:44:55:66")),
                              QLatin1String("info"), 0);
    QTest::addRow("unset") << info << false;

    info.setServiceUuids({} DATA_COMPLETENESS);
    QTest::addRow("empty") << info << false;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid())} DATA_COMPLETENESS);
    QTest::addRow("random") << info << false;

    info.setServiceUuids({ QBluetoothUuid(POKIT_METER_STATUS_SERVICE_UUID) } DATA_COMPLETENESS);
    QTest::addRow("status") << info << true;

    info.setServiceUuids({ QBluetoothUuid(POKIT_PRO_STATUS_SERVICE_UUID) } DATA_COMPLETENESS);
    QTest::addRow("status") << info << true;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid()),
                          QBluetoothUuid(QUuid::createUuid())} DATA_COMPLETENESS);
    QTest::addRow("two") << info << false;

    info.setServiceUuids({QBluetoothUuid(POKIT_METER_STATUS_SERVICE_UUID),
                          QBluetoothUuid(POKIT_PRO_STATUS_SERVICE_UUID)} DATA_COMPLETENESS);
    QTest::addRow("both") << info << true;
}

void TestPokitDiscoveryAgent::isPokitDevice()
{
    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(bool, expected);
    QCOMPARE(PokitDiscoveryAgent::isPokitDevice(info), expected);
}

void TestPokitDiscoveryAgent::isPokitMeter_data()
{
    QTest::addColumn<QBluetoothDeviceInfo>("info");
    QTest::addColumn<bool>("expected");

    QTest::addRow("invalid") << QBluetoothDeviceInfo() << false;

    QBluetoothDeviceInfo info(QBluetoothAddress(QLatin1String("11:22:33:44:55:66")),
                              QLatin1String("info"), 0);
    QTest::addRow("unset") << info << false;

    info.setServiceUuids({} DATA_COMPLETENESS);
    QTest::addRow("empty") << info << false;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid())} DATA_COMPLETENESS);
    QTest::addRow("random") << info << false;

    info.setServiceUuids({ QBluetoothUuid(POKIT_METER_STATUS_SERVICE_UUID) } DATA_COMPLETENESS);
    QTest::addRow("status") << info << true;

    info.setServiceUuids({ QBluetoothUuid(POKIT_PRO_STATUS_SERVICE_UUID) } DATA_COMPLETENESS);
    QTest::addRow("status") << info << false; // Pro != Meter.

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid()),
                          QBluetoothUuid(QUuid::createUuid())} DATA_COMPLETENESS);
    QTest::addRow("two") << info << false;

    info.setServiceUuids({QBluetoothUuid(POKIT_METER_STATUS_SERVICE_UUID),
                          QBluetoothUuid(POKIT_PRO_STATUS_SERVICE_UUID)} DATA_COMPLETENESS);
    QTest::addRow("both") << info << true;
}

void TestPokitDiscoveryAgent::isPokitMeter()
{
    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(bool, expected);
    QCOMPARE(PokitDiscoveryAgent::isPokitMeter(info), expected);
}

void TestPokitDiscoveryAgent::isPokitPro_data()
{
    QTest::addColumn<QBluetoothDeviceInfo>("info");
    QTest::addColumn<bool>("expected");

    QTest::addRow("invalid") << QBluetoothDeviceInfo() << false;

    QBluetoothDeviceInfo info(QBluetoothAddress(QLatin1String("11:22:33:44:55:66")),
                              QLatin1String("info"), 0);
    QTest::addRow("unset") << info << false;

    info.setServiceUuids({} DATA_COMPLETENESS);
    QTest::addRow("empty") << info << false;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid())} DATA_COMPLETENESS);
    QTest::addRow("random") << info << false;

    info.setServiceUuids({ QBluetoothUuid(POKIT_METER_STATUS_SERVICE_UUID) } DATA_COMPLETENESS);
    QTest::addRow("status") << info << false; // Meter != Pro.

    info.setServiceUuids({ QBluetoothUuid(POKIT_PRO_STATUS_SERVICE_UUID) } DATA_COMPLETENESS);
    QTest::addRow("status") << info << true;

    info.setServiceUuids({QBluetoothUuid(QUuid::createUuid()),
                          QBluetoothUuid(QUuid::createUuid())} DATA_COMPLETENESS);
    QTest::addRow("two") << info << false;

    info.setServiceUuids({QBluetoothUuid(POKIT_METER_STATUS_SERVICE_UUID),
                          QBluetoothUuid(POKIT_PRO_STATUS_SERVICE_UUID)} DATA_COMPLETENESS);
    QTest::addRow("both") << info << true;
}

void TestPokitDiscoveryAgent::isPokitPro()
{
    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(bool, expected);
    QCOMPARE(PokitDiscoveryAgent::isPokitPro(info), expected);
}

//void TestPokitDiscoveryAgent::start()
//{
//    QSKIP("Cannot test without impacting Bluetooth devices.");
//}

void TestPokitDiscoveryAgent::cancelled()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDiscoveryAgent service(nullptr);
    service.d_func()->canceled();
}

void TestPokitDiscoveryAgent::deviceDiscovered_data()
{
    isPokitDevice_data();
}

void TestPokitDiscoveryAgent::deviceDiscovered()
{
    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(bool, expected);
    PokitDiscoveryAgent service(nullptr);
    QSignalSpy spy(&service, &PokitDiscoveryAgent::pokitDeviceDiscovered);
    QCOMPARE(spy.count(), 0); // No signals yet.
    service.d_func()->deviceDiscovered(info);
    QCOMPARE(spy.count(), (expected) ? 1 : 0);
}

void TestPokitDiscoveryAgent::deviceUpdated_data()
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    isPokitDevice_data();
    #endif
}

void TestPokitDiscoveryAgent::deviceUpdated()
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(bool, expected);
    PokitDiscoveryAgent service(nullptr);
    QSignalSpy spy(&service, &PokitDiscoveryAgent::pokitDeviceUpdated);
    QCOMPARE(spy.count(), 0); // No signals yet.
    service.d_func()->deviceUpdated(info, QBluetoothDeviceInfo::Fields());
    QCOMPARE(spy.count(), (expected) ? 1 : 0);
    #else
    QSKIP("Not applicable before Qt version 5.12.");
    #endif
}

void TestPokitDiscoveryAgent::error()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDiscoveryAgent service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Pokit device scan error: .*$")));
    service.d_func()->error(QBluetoothDeviceDiscoveryAgent::Error::UnknownError);
}

void TestPokitDiscoveryAgent::finished()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDiscoveryAgent service(nullptr);
    service.d_func()->finished();
}

void TestPokitDiscoveryAgent::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    PokitDiscoveryAgent service(nullptr);
    QVERIFY(!service.tr("ignored").isEmpty());
}

QTEST_MAIN(TestPokitDiscoveryAgent)
