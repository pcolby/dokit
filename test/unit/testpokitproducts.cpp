// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testpokitproducts.h"

#include <qtpokit/pokitproducts.h>
#include <qtpokit/statusservice.h>

#define POKIT_METER_STATUS_SERVICE_UUID QLatin1String("57d3a771-267c-4394-8872-78223e92aec4")
#define POKIT_PRO_STATUS_SERVICE_UUID   QLatin1String("57d3a771-267c-4394-8872-78223e92aec5")

// QBluetoothDeviceInfo::setServiceUuids deprecated the completeness argument in Qt 5.13.
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
#define DATA_COMPLETENESS , QBluetoothDeviceInfo::DataUnavailable
#else
#define DATA_COMPLETENESS
#endif

void TestPokitProducts::toString_PokitProduct_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<QString>("expected");

    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << QStringLiteral("Pokit Meter");
    QTest::addRow("Pokit Pro")   << PokitProduct::PokitPro   << QStringLiteral("Pokit Pro");
}

void TestPokitProducts::toString_PokitProduct()
{
    QFETCH(PokitProduct, product);
    QFETCH(QString, expected);
    QCOMPARE(toString(product), expected);
}

void TestPokitProducts::isPokitProduct_data()
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

void TestPokitProducts::isPokitProduct()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(bool, expected);

    QBluetoothDeviceInfo info;
    QVERIFY(!::isPokitProduct(info));
    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
    QCOMPARE(::isPokitProduct(info), expected);
}

void TestPokitProducts::pokitProduct_data()
{
    QTest::addColumn<QBluetoothUuid>("uuid");
    QTest::addColumn<PokitProduct>("expected");

    QTest::addRow("Pokit Meter") << QBluetoothUuid(POKIT_METER_STATUS_SERVICE_UUID) << PokitProduct::PokitMeter;
    QTest::addRow("Pokit Pro")   << QBluetoothUuid(POKIT_PRO_STATUS_SERVICE_UUID)   << PokitProduct::PokitPro;
    QTest::addRow("pokitMeter")  << StatusService::ServiceUuids::pokitMeter         << PokitProduct::PokitMeter;
    QTest::addRow("pokitPro")    << StatusService::ServiceUuids::pokitPro           << PokitProduct::PokitPro;
}

void TestPokitProducts::pokitProduct()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(PokitProduct, expected);

    QBluetoothDeviceInfo info;
    QVERIFY(!::isPokitProduct(info));
    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
    QVERIFY(::pokitProduct(info) == expected);
}

QTEST_MAIN(TestPokitProducts)
