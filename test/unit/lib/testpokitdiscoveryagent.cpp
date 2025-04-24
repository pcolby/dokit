// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testpokitdiscoveryagent.h"

#include <qtpokit/pokitdiscoveryagent.h>
#include <qtpokit/statusservice.h>
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

QTPOKIT_BEGIN_NAMESPACE

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
    QTest::addColumn<QBluetoothUuid>("uuid");
    QTest::addColumn<bool>("expected");

    QTest::addRow("Pokit Meter") << QBluetoothUuid(POKIT_METER_STATUS_SERVICE_UUID) << true;
    QTest::addRow("Pokit Pro")   << QBluetoothUuid(POKIT_PRO_STATUS_SERVICE_UUID)   << true;
    QTest::addRow("pokitMeter")  << StatusService::ServiceUuids::pokitMeter         << true;
    QTest::addRow("pokitPro")    << StatusService::ServiceUuids::pokitPro           << true;
    QTest::addRow("null")        << QBluetoothUuid()                                << false;
    QTest::addRow("random")      << QBluetoothUuid(QUuid::createUuid())             << false;
}

void TestPokitDiscoveryAgent::deviceDiscovered()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(bool, expected);
    PokitDiscoveryAgent service(nullptr);
    QSignalSpy spy(&service, &PokitDiscoveryAgent::pokitDeviceDiscovered);
    QCOMPARE(spy.count(), 0); // No signals yet.

    QBluetoothDeviceInfo info;
    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
    service.d_func()->deviceDiscovered(info);
    QCOMPARE(spy.count(), (expected) ? 1 : 0);
}

void TestPokitDiscoveryAgent::deviceUpdated_data()
{
    #if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    QSKIP("Not applicable before Qt version 5.12.");
    #endif
    deviceDiscovered_data();
}

void TestPokitDiscoveryAgent::deviceUpdated()
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(bool, expected);
    PokitDiscoveryAgent service(nullptr);
    QSignalSpy spy(&service, &PokitDiscoveryAgent::pokitDeviceUpdated);
    QCOMPARE(spy.count(), 0); // No signals yet.

    QBluetoothDeviceInfo info;
    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
    service.d_func()->deviceUpdated(info, QBluetoothDeviceInfo::Fields());
    QCOMPARE(spy.count(), (expected) ? 1 : 0);
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

QTPOKIT_END_NAMESPACE

QTEST_MAIN(QTPOKIT_PREPEND_NAMESPACE(TestPokitDiscoveryAgent))
