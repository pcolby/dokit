// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testpokitproducts.h"
#include "../github.h"

#include "pokitproducts_p.h"

#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>
#include <qtpokit/pokitproducts.h>
#include <qtpokit/statusservice.h>

#include <QRegularExpression>

Q_DECLARE_METATYPE(QTPOKIT_PREPEND_NAMESPACE(PokitProduct))

#define POKIT_METER_STATUS_SERVICE_UUID QLatin1String("57d3a771-267c-4394-8872-78223e92aec4")
#define POKIT_PRO_STATUS_SERVICE_UUID   QLatin1String("57d3a771-267c-4394-8872-78223e92aec5")

// QBluetoothDeviceInfo::setServiceUuids deprecated the completeness argument in Qt 5.13.
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
#define DATA_COMPLETENESS , QBluetoothDeviceInfo::DataUnavailable
#else
#define DATA_COMPLETENESS
#endif

QTPOKIT_BEGIN_NAMESPACE

void TestPokitProducts::toString_PokitProduct_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<QString>("expected");

    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << QStringLiteral("Pokit Meter");
    QTest::addRow("Pokit Pro")   << PokitProduct::PokitPro   << QStringLiteral("Pokit Pro");
    QTest::addRow("invalid")     << static_cast<PokitProduct>(200) << QString();
}

void TestPokitProducts::toString_PokitProduct()
{
    QFETCH(PokitProduct, product);
    QFETCH(QString, expected);

    if (expected.isNull()) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(R"(^Unknown PokitProduct value: \d+$)")));
    }

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
    QVERIFY(!QTPOKIT_PREPEND_NAMESPACE(isPokitProduct(info)));
    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
    QCOMPARE(QTPOKIT_PREPEND_NAMESPACE(isPokitProduct(info)), expected);
}

void TestPokitProducts::pokitProduct_data()
{
    QTest::addColumn<QBluetoothUuid>("uuid");
    QTest::addColumn<PokitProduct>("expected");

    QTest::addRow("Pokit Meter") << QBluetoothUuid(POKIT_METER_STATUS_SERVICE_UUID) << PokitProduct::PokitMeter;
    QTest::addRow("Pokit Pro")   << QBluetoothUuid(POKIT_PRO_STATUS_SERVICE_UUID)   << PokitProduct::PokitPro;
    QTest::addRow("pokitMeter")  << StatusService::ServiceUuids::pokitMeter         << PokitProduct::PokitMeter;
    QTest::addRow("pokitPro")    << StatusService::ServiceUuids::pokitPro           << PokitProduct::PokitPro;
    QTest::addRow("invalid")     << QBluetoothUuid()                                << PokitProduct::PokitMeter;
}

void TestPokitProducts::pokitProduct()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(PokitProduct, expected);

    QBluetoothDeviceInfo info;
    QVERIFY(!QTPOKIT_PREPEND_NAMESPACE(isPokitProduct(info)));
    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
    if (uuid.isNull()) {
        QTest::ignoreMessage(QtWarningMsg, "Device is not a Pokit product");
    }
    QVERIFY(QTPOKIT_PREPEND_NAMESPACE(pokitProduct(info)) == expected);
}

void TestPokitProducts::isPokitProduct_Uuids_data()
{
    isPokitProduct_data();
}

void TestPokitProducts::isPokitProduct_Uuids()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(bool, expected);

    QVERIFY(!QTPOKIT_PREPEND_NAMESPACE(isPokitProduct(QList<QBluetoothUuid>{})));
    QCOMPARE(QTPOKIT_PREPEND_NAMESPACE(isPokitProduct(QList<QBluetoothUuid>{ uuid })), expected);
}

void TestPokitProducts::isPokitProduct_Controller_data()
{
    if (gitHubActionsRunnerOsVersion() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 14)) {
        QSKIP("BLE controller operations hang on GitHub Actions's macOS 14 runners");
    }
    isPokitProduct_data();
}

void TestPokitProducts::isPokitProduct_Controller()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(bool, expected);

    QBluetoothDeviceInfo info;
    QLowEnergyController * controller = QLowEnergyController::createCentral(info);
    QVERIFY(!QTPOKIT_PREPEND_NAMESPACE(isPokitProduct(*controller)));
    delete controller;

    Q_UNUSED(uuid);
    Q_UNUSED(expected);
//    QSKIP("Cannot create a controller with our own service UUIDs");
//    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
//    qDebug() << info.serviceUuids(); /// Has our UUID.
//    controller = QLowEnergyController::createCentral(info);
//    qDebug() << controller->services(); ///< Always empty.
//    QCOMPARE(::isPokitProduct(*controller), expected);
//    delete controller;
}

void TestPokitProducts::pokitProduct_Uuids_data()
{
    pokitProduct_data();
}

void TestPokitProducts::pokitProduct_Uuids()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(PokitProduct, expected);

    if (uuid.isNull()) {
        QTest::ignoreMessage(QtWarningMsg, "Device is not a Pokit product");
    }

    QVERIFY(QTPOKIT_PREPEND_NAMESPACE(pokitProduct(QList<QBluetoothUuid>{uuid})) == expected);
}

void TestPokitProducts::pokitProduct_Controller_data()
{
    if (gitHubActionsRunnerOsVersion() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 14)) {
        QSKIP("BLE controller operations hang on GitHub Actions's macOS 14 runners");
    }
    pokitProduct_data();
}

void TestPokitProducts::pokitProduct_Controller()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(PokitProduct, expected);

    QBluetoothDeviceInfo info;
    QLowEnergyController * controller = QLowEnergyController::createCentral(info);
    QTest::ignoreMessage(QtWarningMsg, "Device is not a Pokit product");
    QVERIFY(QTPOKIT_PREPEND_NAMESPACE(pokitProduct(*controller)) == PokitProduct::PokitMeter);
    delete controller;

    Q_UNUSED(uuid);
    Q_UNUSED(expected);
//    QSKIP("Cannot create a controller with our own service UUIDs");
//    info.setServiceUuids({ uuid } DATA_COMPLETENESS);
//    qDebug() << info.serviceUuids(); /// Has our UUID.
//    controller = QLowEnergyController::createCentral(info);
//    qDebug() << controller->services(); ///< Always empty.
//    QVERIFY(::pokitProduct(*controller) == expected);
//    delete controller;
}

void TestPokitProducts::toString_Capacitance_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<QString>("expected");

    // We don't need to test exhaustively here - that's done by TestPokitPro::toString_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << (quint8)0 << QString();
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::CapacitanceRange::_100nF
                               << QStringLiteral("Up to 100nF");
    QTest::addRow("invalid") << static_cast<PokitProduct>(200) << (quint8)0 << QString();
}

void TestPokitProducts::toString_Capacitance()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QString, expected);

    if (product == PokitProduct::PokitMeter) {
        QTest::ignoreMessage(QtWarningMsg, "Pokit Meter has no capacitance support");
    } else if (expected.isNull()) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(R"(^Unknown PokitProduct value: \d+$)")));
    }

    QCOMPARE(CapacitanceRange::toString(product, range), expected);
}

void TestPokitProducts::maxValue_Capacitance_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<quint32>("expected");

    // We don't need to test exhaustively here - that's done by TestPokitPro::maxValue_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << (quint8)0 << (quint32)0;
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::CapacitanceRange::_100nF << quint32(100);
    QTest::addRow("invalid") << static_cast<PokitProduct>(200) << (quint8)0 << (quint32)0;
}

void TestPokitProducts::maxValue_Capacitance()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(quint32, expected);

    if (product == PokitProduct::PokitMeter) {
        QTest::ignoreMessage(QtWarningMsg, "Pokit Meter has no capacitance support");
    } else if (!expected) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(R"(^Unknown PokitProduct value: \d+$)")));
    }

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
                               << QString::fromUtf8("Up to 500μA");
    QTest::addRow("invalid") << static_cast<PokitProduct>(200) << (quint8)0 << QString();
}

void TestPokitProducts::toString_Current()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QString, expected);

    if (expected.isNull()) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(R"(^Unknown PokitProduct value: \d+$)")));
    }

    QCOMPARE(CurrentRange::toString(product, range), expected);
}

void TestPokitProducts::maxValue_Current_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<quint32>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::maxValue* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::CurrentRange::_150mA << quint32(150000);
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::CurrentRange::_500uA << quint32(500);
    QTest::addRow("invalid") << static_cast<PokitProduct>(200) << (quint8)0 << (quint32)0;
}

void TestPokitProducts::maxValue_Current()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(quint32, expected);

    if (!expected) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(R"(^Unknown PokitProduct value: \d+$)")));
    }

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
                                 << QString::fromUtf8("Up to 470KΩ");
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::ResistanceRange::_3M
                               << QString::fromUtf8("Up to 3MΩ");
    QTest::addRow("invalid") << static_cast<PokitProduct>(200) << (quint8)0 << QString();
}

void TestPokitProducts::toString_Resistance()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QString, expected);

    if (expected.isNull()) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(R"(^Unknown PokitProduct value: \d+$)")));
    }

    QCOMPARE(ResistanceRange::toString(product, range), expected);
}

void TestPokitProducts::maxValue_Resistance_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<quint32>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::maxValue_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::ResistanceRange::_470K << quint32(470000);
    QTest::addRow("Pokit Pro")   << PokitProduct::PokitPro << +PokitPro::ResistanceRange::_3M << quint32(3000000);
    QTest::addRow("invalid") << static_cast<PokitProduct>(200) << (quint8)0 << (quint32)0;
}

void TestPokitProducts::maxValue_Resistance()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(quint32, expected);

    if (!expected) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(R"(^Unknown PokitProduct value: \d+$)")));
    }

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
    QTest::addRow("invalid") << static_cast<PokitProduct>(200) << (quint8)0 << QString();
}

void TestPokitProducts::toString_Voltage()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(QString, expected);

    if (expected.isNull()) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(R"(^Unknown PokitProduct value: \d+$)")));
    }

    QCOMPARE(VoltageRange::toString(product, range), expected);
}

void TestPokitProducts::maxValue_Voltage_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<quint32>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::maxValue_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::VoltageRange::_300mV << quint32(300);
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::VoltageRange::_600V << quint32(600000);
    QTest::addRow("invalid") << static_cast<PokitProduct>(200) << (quint8)0 << (quint32)0;
}

void TestPokitProducts::maxValue_Voltage()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(quint32, expected);

    if (!expected) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(R"(^Unknown PokitProduct value: \d+$)")));
    }

    QCOMPARE(VoltageRange::maxValue(product, range), expected);
}

QTPOKIT_END_NAMESPACE

QTEST_MAIN(QTPOKIT_PREPEND_NAMESPACE(TestPokitProducts))
