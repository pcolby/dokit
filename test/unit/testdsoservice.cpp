// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdsoservice.h"

#include <qtpokit/dsoservice.h>
#include "dsoservice_p.h"

#include <QRegularExpression>

Q_DECLARE_METATYPE(DsoService::Mode);
Q_DECLARE_METATYPE(DsoService::VoltageRange);
Q_DECLARE_METATYPE(DsoService::CurrentRange);
Q_DECLARE_METATYPE(DsoService::Range);
Q_DECLARE_METATYPE(DsoService::Settings);
Q_DECLARE_METATYPE(DsoService::Metadata);

void TestDsoService::toString_Mode_data()
{
    QTest::addColumn<DsoService::Mode>("mode");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(mode, expected) \
        QTest::addRow(#mode) << DsoService::Mode::mode << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(Idle,      "Idle");
    QTPOKIT_ADD_TEST_ROW(DcVoltage, "DC voltage");
    QTPOKIT_ADD_TEST_ROW(AcVoltage, "AC voltage");
    QTPOKIT_ADD_TEST_ROW(DcCurrent, "DC current");
    QTPOKIT_ADD_TEST_ROW(AcCurrent, "AC current");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DsoService::Mode)5    << QString();
    QTest::addRow("max")     << (DsoService::Mode)0xFF << QString();
}

void TestDsoService::toString_Mode()
{
    QFETCH(DsoService::Mode, mode);
    QFETCH(QString, expected);
    QCOMPARE(DsoService::toString(mode), expected);
}

void TestDsoService::toString_VoltageRange_data()
{
    QTest::addColumn<DsoService::VoltageRange>("range");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DsoService::VoltageRange::range << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_300mV,  "0 to 300mV");
    QTPOKIT_ADD_TEST_ROW(_300mV_to_2V, "300mV to 2V");
    QTPOKIT_ADD_TEST_ROW(_2V_to_6V,    "2V to 6V");
    QTPOKIT_ADD_TEST_ROW(_6V_to_12V,   "6V to 12V");
    QTPOKIT_ADD_TEST_ROW(_12V_to_30V,  "12V to 30V");
    QTPOKIT_ADD_TEST_ROW(_30V_to_60V,  "30V to 60V");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DsoService::VoltageRange)6    << QString();
    QTest::addRow("max")     << (DsoService::VoltageRange)0xFF << QString();
}

void TestDsoService::toString_VoltageRange()
{
    QFETCH(DsoService::VoltageRange, range);
    QFETCH(QString, expected);
    QCOMPARE(DsoService::toString(range), expected);
}

void TestDsoService::minValue_VoltageRange_data()
{
    QTest::addColumn<DsoService::VoltageRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DsoService::VoltageRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_300mV,      0);
    QTPOKIT_ADD_TEST_ROW(_300mV_to_2V,   300);
    QTPOKIT_ADD_TEST_ROW(_2V_to_6V,     2000);
    QTPOKIT_ADD_TEST_ROW(_6V_to_12V,    6000);
    QTPOKIT_ADD_TEST_ROW(_12V_to_30V,  12000);
    QTPOKIT_ADD_TEST_ROW(_30V_to_60V,  30000);
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DsoService::VoltageRange)6    << QVariant();
    QTest::addRow("max")     << (DsoService::VoltageRange)0xFF << QVariant();
}

void TestDsoService::minValue_VoltageRange()
{
    QFETCH(DsoService::VoltageRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(DsoService::minValue(range), expected);
}

void TestDsoService::maxValue_VoltageRange_data()
{
    QTest::addColumn<DsoService::VoltageRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DsoService::VoltageRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_300mV,    300);
    QTPOKIT_ADD_TEST_ROW(_300mV_to_2V,  2000);
    QTPOKIT_ADD_TEST_ROW(_2V_to_6V,     6000);
    QTPOKIT_ADD_TEST_ROW(_6V_to_12V,   12000);
    QTPOKIT_ADD_TEST_ROW(_12V_to_30V,  30000);
    QTPOKIT_ADD_TEST_ROW(_30V_to_60V,  60000);
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DsoService::VoltageRange)6    << QVariant();
    QTest::addRow("max")     << (DsoService::VoltageRange)0xFF << QVariant();
}

void TestDsoService::maxValue_VoltageRange()
{
    QFETCH(DsoService::VoltageRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(DsoService::maxValue(range), expected);
}

void TestDsoService::toString_CurrentRange_data()
{
    QTest::addColumn<DsoService::CurrentRange>("range");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DsoService::CurrentRange::range << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_10mA,      "0 to 10mA");
    QTPOKIT_ADD_TEST_ROW(_10mA_to_30mA,   "10mA to 30mA");
    QTPOKIT_ADD_TEST_ROW(_30mA_to_150mA,  "30mA to 150mA");
    QTPOKIT_ADD_TEST_ROW(_150mA_to_300mA, "150mA to 300mA");
    QTPOKIT_ADD_TEST_ROW(_300mA_to_3A,    "300mA to 3A");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DsoService::CurrentRange)5    << QString();
    QTest::addRow("max")     << (DsoService::CurrentRange)0xFF << QString();
}

void TestDsoService::toString_CurrentRange()
{
    QFETCH(DsoService::CurrentRange, range);
    QFETCH(QString, expected);
    QCOMPARE(DsoService::toString(range), expected);
}

void TestDsoService::minValue_CurrentRange_data()
{
    QTest::addColumn<DsoService::CurrentRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DsoService::CurrentRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_10mA,        0);
    QTPOKIT_ADD_TEST_ROW(_10mA_to_30mA,    10);
    QTPOKIT_ADD_TEST_ROW(_30mA_to_150mA,   30);
    QTPOKIT_ADD_TEST_ROW(_150mA_to_300mA, 150);
    QTPOKIT_ADD_TEST_ROW(_300mA_to_3A,    300);
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DsoService::CurrentRange)5    << QVariant();
    QTest::addRow("max")     << (DsoService::CurrentRange)0xFF << QVariant();
}

void TestDsoService::minValue_CurrentRange()
{
    QFETCH(DsoService::CurrentRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(DsoService::minValue(range), expected);
}

void TestDsoService::maxValue_CurrentRange_data()
{
    QTest::addColumn<DsoService::CurrentRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DsoService::CurrentRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_10mA,        10);
    QTPOKIT_ADD_TEST_ROW(_10mA_to_30mA,     30);
    QTPOKIT_ADD_TEST_ROW(_30mA_to_150mA,   150);
    QTPOKIT_ADD_TEST_ROW(_150mA_to_300mA,  300);
    QTPOKIT_ADD_TEST_ROW(_300mA_to_3A,    3000);
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DsoService::CurrentRange)5    << QVariant();
    QTest::addRow("max")     << (DsoService::CurrentRange)0xFF << QVariant();
}

void TestDsoService::maxValue_CurrentRange()
{
    QFETCH(DsoService::CurrentRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(DsoService::maxValue(range), expected);
}

void TestDsoService::toString_Range_data()
{
    QTest::addColumn<DsoService::Range>("range");
    QTest::addColumn<DsoService::Mode>("mode");
    QTest::addColumn<QString>("expected");

    #define QTPOKIT_ADD_TEST_ROW(mode, member, range, expected) {\
        DsoService::Range rangeUnion; \
        rangeUnion.member = DsoService::range; \
        QTest::addRow(#mode "," #range) \
            << rangeUnion << DsoService::Mode::mode << QStringLiteral(expected); \
    }
    QTPOKIT_ADD_TEST_ROW(DcVoltage, voltageRange, VoltageRange::_0_to_300mV,  "0 to 300mV");
    QTPOKIT_ADD_TEST_ROW(DcVoltage, voltageRange, VoltageRange::_30V_to_60V,  "30V to 60V");
    QTPOKIT_ADD_TEST_ROW(AcVoltage, voltageRange, VoltageRange::_0_to_300mV,  "0 to 300mV");
    QTPOKIT_ADD_TEST_ROW(AcVoltage, voltageRange, VoltageRange::_30V_to_60V,  "30V to 60V");
    QTPOKIT_ADD_TEST_ROW(DcCurrent, currentRange, CurrentRange::_0_to_10mA,   "0 to 10mA");
    QTPOKIT_ADD_TEST_ROW(DcCurrent, currentRange, CurrentRange::_300mA_to_3A, "300mA to 3A");
    QTPOKIT_ADD_TEST_ROW(AcCurrent, currentRange, CurrentRange::_0_to_10mA,   "0 to 10mA");
    QTPOKIT_ADD_TEST_ROW(AcCurrent, currentRange, CurrentRange::_300mA_to_3A, "300mA to 3A");
    QTPOKIT_ADD_TEST_ROW(Idle,      voltageRange, VoltageRange::_0_to_300mV,  ""); // Invalid.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestDsoService::toString_Range()
{
    QFETCH(DsoService::Range, range);
    QFETCH(DsoService::Mode, mode);
    QFETCH(QString, expected);
    QCOMPARE(DsoService::toString(range, mode), expected);
}

void TestDsoService::readCharacteristics()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DsoService service(nullptr);
    QVERIFY(!service.readCharacteristics());
}

void TestDsoService::readMetadataCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DsoService service(nullptr);
    QVERIFY(!service.readMetadataCharacteristic());
}

void TestDsoService::setSettings()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DsoService service(nullptr);
    QVERIFY(!service.setSettings({}));
}

void TestDsoService::startDso()
{
    // Verify safe error handling, when called correctly.
    DsoService service(nullptr);
    DsoService::Settings settings;
    settings.command = DsoService::Command::FreeRunning;
    QVERIFY(!service.startDso(settings));
    settings.command = DsoService::Command::RisingEdgeTrigger;
    QVERIFY(!service.startDso(settings));
    settings.command = DsoService::Command::FallingEdgeTrigger;
    QVERIFY(!service.startDso(settings));

    #ifndef QT_NO_DEBUG
    qInfo("Skipping some test conditions that would otherwise Q_ASSERT.");
    return;
    #endif

    settings.command = DsoService::Command::ResendData;
    QTest::ignoreMessage(QtWarningMsg, "Settings command must not be 'ResendData'.");
    QVERIFY(!service.startDso(settings));
}

void TestDsoService::fetchSamples()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DsoService service(nullptr);
    QVERIFY(!service.fetchSamples());
}

void TestDsoService::metadata()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DsoService service(nullptr);
    QVERIFY(qIsNaN(service.metadata().scale));
}

void TestDsoService::enableMetadataNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DsoService service(nullptr);
    QVERIFY(!service.enableMetadataNotifications());
}

void TestDsoService::disableMetadataNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DsoService service(nullptr);
    QVERIFY(!service.disableMetadataNotifications());
}

void TestDsoService::enableReadingNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DsoService service(nullptr);
    QVERIFY(!service.enableReadingNotifications());
}

void TestDsoService::disableReadingNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DsoService service(nullptr);
    QVERIFY(!service.disableReadingNotifications());
}

QTEST_MAIN(TestDsoService)
