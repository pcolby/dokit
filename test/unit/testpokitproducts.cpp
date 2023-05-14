// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testpokitproducts.h"

#include "pokitproducts_p.h"

#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>
#include <qtpokit/pokitproducts.h>
#include <qtpokit/statusservice.h>

Q_DECLARE_METATYPE(PokitProduct)

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

void TestPokitProducts::isPokitProduct_Uuids_data()
{
    isPokitProduct_data();
}

void TestPokitProducts::isPokitProduct_Uuids()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(bool, expected);

    QVERIFY(!::isPokitProduct(QList<QBluetoothUuid>{}));
    QCOMPARE(::isPokitProduct(QList<QBluetoothUuid>{ uuid }), expected);
}

//void TestPokitProducts::isPokitProduct_Controller_data()
//{
//    isPokitProduct_data();
//}

//void TestPokitProducts::isPokitProduct_Controller()
//{
//    QSKIP("Cannot create a controller with our own service UUIDs");

//    QFETCH(QBluetoothUuid, uuid);
//    QFETCH(bool, expected);

//    QBluetoothDeviceInfo info;
//    QLowEnergyController * controller = QLowEnergyController::createCentral(info);
//    QVERIFY(!::isPokitProduct(*controller));
//    delete controller;

//    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
//    qDebug() << info.serviceUuids(); /// Has our UUID.
//    controller = QLowEnergyController::createCentral(info);
//    qDebug() << controller->services(); ///< Always empty.
//    QCOMPARE(::isPokitProduct(*controller), expected);
//    delete controller;
//}

void TestPokitProducts::pokitProduct_Uuids_data()
{
    pokitProduct_data();
}

void TestPokitProducts::pokitProduct_Uuids()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(PokitProduct, expected);

    QVERIFY(::pokitProduct(QList<QBluetoothUuid>{uuid}) == expected);
}

//void TestPokitProducts::pokitProduct_Controller_data()
//{
//    pokitProduct_data();
//}

//void TestPokitProducts::pokitProduct_Controller()
//{
//    QSKIP("Cannot create a controller with our own service UUIDs");

//    QFETCH(QBluetoothUuid, uuid);
//    QFETCH(PokitProduct, expected);

//    QBluetoothDeviceInfo info;
//    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
//    qDebug() << info.serviceUuids(); /// Has our UUID.
//    QLowEnergyController * controller = QLowEnergyController::createCentral(info);
//    qDebug() << controller->services(); ///< Always empty.
//    QVERIFY(::pokitProduct(*controller) == expected);
//    delete controller;
//}

void TestPokitProducts::toString_Capacitance_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<QString>("expected");

    // We don't need to test exhaustively here - that's done by TestPokitPro::toString_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::CapacitanceRange::_100nF
                               << QStringLiteral("Up to 100nF");
}

void TestPokitProducts::toString_Capacitance()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QString, expected);
    QCOMPARE(CapacitanceRange::toString(product, range), expected);
}

void TestPokitProducts::maxValue_Capacitance_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<QVariant>("expected");

    // We don't need to test exhaustively here - that's done by TestPokitPro::maxValue_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::CapacitanceRange::_100nF << QVariant(100);
}

void TestPokitProducts::maxValue_Capacitance()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QVariant, expected);
    QCOMPARE(CapacitanceRange::maxValue(product, range), expected);
}

void TestPokitProducts::toString_Current_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<QString>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::toString_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::CurrentRange::_150mA
         << QStringLiteral("Up to 150mA");
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::CurrentRange::_500uA
        << QStringLiteral("Up to 500μA");
}

void TestPokitProducts::toString_Current()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QString, expected);
    QCOMPARE(CurrentRange::toString(product, range), expected);
}

void TestPokitProducts::maxValue_Current_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<QVariant>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::maxValue* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::CurrentRange::_150mA << QVariant(150000);
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::CurrentRange::_500uA << QVariant(500);
}

void TestPokitProducts::maxValue_Current()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QVariant, expected);
    QCOMPARE(CurrentRange::maxValue(product, range), expected);
}

void TestPokitProducts::toString_Resistance_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<QString>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::toString_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::ResistanceRange::_470K
                                 << QStringLiteral("Up to 470KΩ");
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::ResistanceRange::_3M
                               << QStringLiteral("Up to 3MΩ");
}

void TestPokitProducts::toString_Resistance()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QString, expected);
    QCOMPARE(ResistanceRange::toString(product, range), expected);
}

void TestPokitProducts::maxValue_Resistance_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<QVariant>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::maxValue_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::ResistanceRange::_470K << QVariant(470000);
    QTest::addRow("Pokit Pro")   << PokitProduct::PokitPro << +PokitPro::ResistanceRange::_3M << QVariant(3000000);
}

void TestPokitProducts::maxValue_Resistance()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QVariant, expected);
    QCOMPARE(ResistanceRange::maxValue(product, range), expected);
}

void TestPokitProducts::toString_Voltage_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<QString>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::toString_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::VoltageRange::_300mV
                                 << QStringLiteral("Up to 300mV");
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::VoltageRange::_600V
                               << QStringLiteral("Up to 600V");
}

void TestPokitProducts::toString_Voltage()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QString, expected);
    QCOMPARE(VoltageRange::toString(product, range), expected);
}

void TestPokitProducts::maxValue_Voltage_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<QVariant>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::maxValue_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::VoltageRange::_300mV << QVariant(300);
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::VoltageRange::_600V << QVariant(600000);
}

void TestPokitProducts::maxValue_Voltage()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QVariant, expected);
    QCOMPARE(VoltageRange::maxValue(product, range), expected);
}

QTEST_MAIN(TestPokitProducts)
