// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdataloggerservice.h"

#include <qtpokit/dataloggerservice.h>

Q_DECLARE_METATYPE(DataLoggerService::Mode);
Q_DECLARE_METATYPE(DataLoggerService::VoltageRange);
Q_DECLARE_METATYPE(DataLoggerService::CurrentRange);
Q_DECLARE_METATYPE(DataLoggerService::Range);

void TestDataLoggerService::toString_Mode_data()
{
    QTest::addColumn<DataLoggerService::Mode>("mode");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(mode, expected) \
        QTest::addRow(#mode) << DataLoggerService::Mode::mode << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(Idle,      "Idle");
    QTPOKIT_ADD_TEST_ROW(DcVoltage, "DC voltage");
    QTPOKIT_ADD_TEST_ROW(AcVoltage, "AC voltage");
    QTPOKIT_ADD_TEST_ROW(DcCurrent, "DC current");
    QTPOKIT_ADD_TEST_ROW(AcCurrent, "AC current");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DataLoggerService::Mode)5    << QString();
    QTest::addRow("max")     << (DataLoggerService::Mode)0xFF << QString();
}

void TestDataLoggerService::toString_Mode()
{
    QFETCH(DataLoggerService::Mode, mode);
    QFETCH(QString, expected);
    QCOMPARE(DataLoggerService::toString(mode), expected);
}

void TestDataLoggerService::toString_VoltageRange_data()
{
    QTest::addColumn<DataLoggerService::VoltageRange>("range");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DataLoggerService::VoltageRange::range << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_300mV,  "0 to 300mV");
    QTPOKIT_ADD_TEST_ROW(_300mV_to_2V, "300mV to 2V");
    QTPOKIT_ADD_TEST_ROW(_2V_to_6V,    "2V to 6V");
    QTPOKIT_ADD_TEST_ROW(_6V_to_12V,   "6V to 12V");
    QTPOKIT_ADD_TEST_ROW(_12V_to_30V,  "12V to 30V");
    QTPOKIT_ADD_TEST_ROW(_30V_to_60V,  "30V to 60V");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DataLoggerService::VoltageRange)6    << QString();
    QTest::addRow("max")     << (DataLoggerService::VoltageRange)0xFF << QString();
}

void TestDataLoggerService::toString_VoltageRange()
{
    QFETCH(DataLoggerService::VoltageRange, range);
    QFETCH(QString, expected);
    QCOMPARE(DataLoggerService::toString(range), expected);
}

void TestDataLoggerService::minValue_VoltageRange_data()
{
    QTest::addColumn<DataLoggerService::VoltageRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DataLoggerService::VoltageRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_300mV,      0);
    QTPOKIT_ADD_TEST_ROW(_300mV_to_2V,   300);
    QTPOKIT_ADD_TEST_ROW(_2V_to_6V,     2000);
    QTPOKIT_ADD_TEST_ROW(_6V_to_12V,    6000);
    QTPOKIT_ADD_TEST_ROW(_12V_to_30V,  12000);
    QTPOKIT_ADD_TEST_ROW(_30V_to_60V,  30000);
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DataLoggerService::VoltageRange)6    << QVariant();
    QTest::addRow("max")     << (DataLoggerService::VoltageRange)0xFF << QVariant();
}

void TestDataLoggerService::minValue_VoltageRange()
{
    QFETCH(DataLoggerService::VoltageRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(DataLoggerService::minValue(range), expected);
}

void TestDataLoggerService::maxValue_VoltageRange_data()
{
    QTest::addColumn<DataLoggerService::VoltageRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DataLoggerService::VoltageRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_300mV,    300);
    QTPOKIT_ADD_TEST_ROW(_300mV_to_2V,  2000);
    QTPOKIT_ADD_TEST_ROW(_2V_to_6V,     6000);
    QTPOKIT_ADD_TEST_ROW(_6V_to_12V,   12000);
    QTPOKIT_ADD_TEST_ROW(_12V_to_30V,  30000);
    QTPOKIT_ADD_TEST_ROW(_30V_to_60V,  60000);
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DataLoggerService::VoltageRange)6    << QVariant();
    QTest::addRow("max")     << (DataLoggerService::VoltageRange)0xFF << QVariant();
}

void TestDataLoggerService::maxValue_VoltageRange()
{
    QFETCH(DataLoggerService::VoltageRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(DataLoggerService::maxValue(range), expected);
}

void TestDataLoggerService::toString_CurrentRange_data()
{
    QTest::addColumn<DataLoggerService::CurrentRange>("range");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DataLoggerService::CurrentRange::range << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_10mA,      "0 to 10mA");
    QTPOKIT_ADD_TEST_ROW(_10mA_to_30mA,   "10mA to 30mA");
    QTPOKIT_ADD_TEST_ROW(_30mA_to_150mA,  "30mA to 150mA");
    QTPOKIT_ADD_TEST_ROW(_150mA_to_300mA, "150mA to 300mA");
    QTPOKIT_ADD_TEST_ROW(_300mA_to_3A,    "300mA to 3A");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DataLoggerService::CurrentRange)5    << QString();
    QTest::addRow("max")     << (DataLoggerService::CurrentRange)0xFF << QString();
}

void TestDataLoggerService::toString_CurrentRange()
{
    QFETCH(DataLoggerService::CurrentRange, range);
    QFETCH(QString, expected);
    QCOMPARE(DataLoggerService::toString(range), expected);
}

void TestDataLoggerService::minValue_CurrentRange_data()
{
    QTest::addColumn<DataLoggerService::CurrentRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DataLoggerService::CurrentRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_10mA,        0);
    QTPOKIT_ADD_TEST_ROW(_10mA_to_30mA,    10);
    QTPOKIT_ADD_TEST_ROW(_30mA_to_150mA,   30);
    QTPOKIT_ADD_TEST_ROW(_150mA_to_300mA, 150);
    QTPOKIT_ADD_TEST_ROW(_300mA_to_3A,    300);
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DataLoggerService::CurrentRange)5    << QVariant();
    QTest::addRow("max")     << (DataLoggerService::CurrentRange)0xFF << QVariant();
}

void TestDataLoggerService::minValue_CurrentRange()
{
    QFETCH(DataLoggerService::CurrentRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(DataLoggerService::minValue(range), expected);
}

void TestDataLoggerService::maxValue_CurrentRange_data()
{
    QTest::addColumn<DataLoggerService::CurrentRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << DataLoggerService::CurrentRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_10mA,        10);
    QTPOKIT_ADD_TEST_ROW(_10mA_to_30mA,     30);
    QTPOKIT_ADD_TEST_ROW(_30mA_to_150mA,   150);
    QTPOKIT_ADD_TEST_ROW(_150mA_to_300mA,  300);
    QTPOKIT_ADD_TEST_ROW(_300mA_to_3A,    3000);
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DataLoggerService::CurrentRange)5    << QVariant();
    QTest::addRow("max")     << (DataLoggerService::CurrentRange)0xFF << QVariant();
}

void TestDataLoggerService::maxValue_CurrentRange()
{
    QFETCH(DataLoggerService::CurrentRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(DataLoggerService::maxValue(range), expected);
}

void TestDataLoggerService::toString_Range_data()
{
    QTest::addColumn<DataLoggerService::Range>("range");
    QTest::addColumn<DataLoggerService::Mode>("mode");
    QTest::addColumn<QString>("expected");

    #define QTPOKIT_ADD_TEST_ROW(mode, member, range, expected) \
    QTest::addRow(#mode "," #range) \
        << DataLoggerService::Range{ member = DataLoggerService::range } \
        << DataLoggerService::Mode::mode \
        << QStringLiteral(expected)
    QTPOKIT_ADD_TEST_ROW(DcVoltage, .voltageRange, VoltageRange::_0_to_300mV,  "0 to 300mV");
    QTPOKIT_ADD_TEST_ROW(DcVoltage, .voltageRange, VoltageRange::_30V_to_60V,  "30V to 60V");
    QTPOKIT_ADD_TEST_ROW(AcVoltage, .voltageRange, VoltageRange::_0_to_300mV,  "0 to 300mV");
    QTPOKIT_ADD_TEST_ROW(AcVoltage, .voltageRange, VoltageRange::_30V_to_60V,  "30V to 60V");
    QTPOKIT_ADD_TEST_ROW(DcCurrent, .currentRange, CurrentRange::_0_to_10mA,   "0 to 10mA");
    QTPOKIT_ADD_TEST_ROW(DcCurrent, .currentRange, CurrentRange::_300mA_to_3A, "300mA to 3A");
    QTPOKIT_ADD_TEST_ROW(AcCurrent, .currentRange, CurrentRange::_0_to_10mA,   "0 to 10mA");
    QTPOKIT_ADD_TEST_ROW(AcCurrent, .currentRange, CurrentRange::_300mA_to_3A, "300mA to 3A");
    QTPOKIT_ADD_TEST_ROW(Idle,      .voltageRange, VoltageRange::_0_to_300mV,  ""); // Invalid.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestDataLoggerService::toString_Range()
{
    QFETCH(DataLoggerService::Range, range);
    QFETCH(DataLoggerService::Mode, mode);
    QFETCH(QString, expected);
    QCOMPARE(DataLoggerService::toString(range, mode), expected);
}

void TestDataLoggerService::readCharacteristics()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.readCharacteristics());
}

void TestDataLoggerService::readMetadataCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.readMetadataCharacteristic());
}

void TestDataLoggerService::setSettings()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.setSettings({}));
}

void TestDataLoggerService::startLogger()
{
    // Verify safe error handling, when called correctly.
    DataLoggerService service(nullptr);
    DataLoggerService::Settings settings;
    settings.command = DataLoggerService::Command::Start;
    QVERIFY(!service.startLogger(settings));

    #ifndef QT_NO_DEBUG
    qInfo("Skipping some test conditions that would otherwise Q_ASSERT.");
    return;
    #endif

    settings.command = DataLoggerService::Command::Stop;
    QTest::ignoreMessage(QtWarningMsg, "Settings command must be 'Start'.");
    QVERIFY(!service.startLogger(settings));

    settings.command = DataLoggerService::Command::Refresh;
    QTest::ignoreMessage(QtWarningMsg, "Settings command must be 'Start'.");
    QVERIFY(!service.startLogger(settings));
}

void TestDataLoggerService::stopLogger()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.stopLogger());
}

void TestDataLoggerService::fetchSamples()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.fetchSamples());
}

void TestDataLoggerService::metadata()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(qIsNaN(service.metadata().scale));
}

void TestDataLoggerService::enableMetadataNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.enableMetadataNotifications());
}

void TestDataLoggerService::disableMetadatNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.disableMetadatNotifications());
}

void TestDataLoggerService::enableReadingNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.enableReadingNotifications());
}

void TestDataLoggerService::disableReadingNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.disableReadingNotifications());
}

QTEST_MAIN(TestDataLoggerService)
