// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testmultimeterservice.h"

#include <qtpokit/multimeterservice.h>
#include "multimeterservice_p.h"

#include <QRegularExpression>

Q_DECLARE_METATYPE(MultimeterService::Mode);
Q_DECLARE_METATYPE(MultimeterService::VoltageRange);
Q_DECLARE_METATYPE(MultimeterService::CurrentRange);
Q_DECLARE_METATYPE(MultimeterService::Range);
Q_DECLARE_METATYPE(MultimeterService::Settings);

void TestMultimeterService::toString_Mode_data()
{
    QTest::addColumn<MultimeterService::Mode>("mode");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(mode, expected) \
        QTest::addRow(#mode) << MultimeterService::Mode::mode << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(Idle,        "Idle");
    QTPOKIT_ADD_TEST_ROW(DcVoltage,   "DC voltage");
    QTPOKIT_ADD_TEST_ROW(AcVoltage,   "AC voltage");
    QTPOKIT_ADD_TEST_ROW(DcCurrent,   "DC current");
    QTPOKIT_ADD_TEST_ROW(AcCurrent,   "AC current");
    QTPOKIT_ADD_TEST_ROW(Resistance,  "Resistance");
    QTPOKIT_ADD_TEST_ROW(Diode,       "Diode");
    QTPOKIT_ADD_TEST_ROW(Continuity,  "Continuity");
    QTPOKIT_ADD_TEST_ROW(Temperature, "Temperature");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::Mode)9    << QString();
    QTest::addRow("max")     << (MultimeterService::Mode)0xFF << QString();
}

void TestMultimeterService::toString_Mode()
{
    QFETCH(MultimeterService::Mode, mode);
    QFETCH(QString, expected);
    QCOMPARE(MultimeterService::toString(mode), expected);
}

void TestMultimeterService::toString_VoltageRange_data()
{
    QTest::addColumn<MultimeterService::VoltageRange>("range");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << MultimeterService::VoltageRange::range << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_300mV,  "0 to 300mV");
    QTPOKIT_ADD_TEST_ROW(_300mV_to_2V, "300mV to 2V");
    QTPOKIT_ADD_TEST_ROW(_2V_to_6V,    "2V to 6V");
    QTPOKIT_ADD_TEST_ROW(_6V_to_12V,   "6V to 12V");
    QTPOKIT_ADD_TEST_ROW(_12V_to_30V,  "12V to 30V");
    QTPOKIT_ADD_TEST_ROW(_30V_to_60V,  "30V to 60V");
    QTPOKIT_ADD_TEST_ROW(AutoRange,    "Auto-range");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::VoltageRange)6    << QString();
    QTest::addRow("max")     << (MultimeterService::VoltageRange)0xFE << QString();
}

void TestMultimeterService::toString_VoltageRange()
{
    QFETCH(MultimeterService::VoltageRange, range);
    QFETCH(QString, expected);
    QCOMPARE(MultimeterService::toString(range), expected);
}

void TestMultimeterService::minValue_VoltageRange_data()
{
    QTest::addColumn<MultimeterService::VoltageRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << MultimeterService::VoltageRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_300mV,      0);
    QTPOKIT_ADD_TEST_ROW(_300mV_to_2V,   300);
    QTPOKIT_ADD_TEST_ROW(_2V_to_6V,     2000);
    QTPOKIT_ADD_TEST_ROW(_6V_to_12V,    6000);
    QTPOKIT_ADD_TEST_ROW(_12V_to_30V,  12000);
    QTPOKIT_ADD_TEST_ROW(_30V_to_60V,  30000);
    QTPOKIT_ADD_TEST_ROW(AutoRange,    QStringLiteral("Auto"));
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::VoltageRange)6    << QVariant();
    QTest::addRow("max")     << (MultimeterService::VoltageRange)0xFE << QVariant();
}

void TestMultimeterService::minValue_VoltageRange()
{
    QFETCH(MultimeterService::VoltageRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(MultimeterService::minValue(range), expected);
}

void TestMultimeterService::maxValue_VoltageRange_data()
{
    QTest::addColumn<MultimeterService::VoltageRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << MultimeterService::VoltageRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_300mV,    300);
    QTPOKIT_ADD_TEST_ROW(_300mV_to_2V,  2000);
    QTPOKIT_ADD_TEST_ROW(_2V_to_6V,     6000);
    QTPOKIT_ADD_TEST_ROW(_6V_to_12V,   12000);
    QTPOKIT_ADD_TEST_ROW(_12V_to_30V,  30000);
    QTPOKIT_ADD_TEST_ROW(_30V_to_60V,  60000);
    QTPOKIT_ADD_TEST_ROW(AutoRange,    QStringLiteral("Auto"));
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::VoltageRange)6    << QVariant();
    QTest::addRow("max")     << (MultimeterService::VoltageRange)0xFE << QVariant();
}

void TestMultimeterService::maxValue_VoltageRange()
{
    QFETCH(MultimeterService::VoltageRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(MultimeterService::maxValue(range), expected);
}

void TestMultimeterService::toString_CurrentRange_data()
{
    QTest::addColumn<MultimeterService::CurrentRange>("range");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << MultimeterService::CurrentRange::range << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_10mA,      "0 to 10mA");
    QTPOKIT_ADD_TEST_ROW(_10mA_to_30mA,   "10mA to 30mA");
    QTPOKIT_ADD_TEST_ROW(_30mA_to_150mA,  "30mA to 150mA");
    QTPOKIT_ADD_TEST_ROW(_150mA_to_300mA, "150mA to 300mA");
    QTPOKIT_ADD_TEST_ROW(_300mA_to_3A,    "300mA to 3A");
    QTPOKIT_ADD_TEST_ROW(AutoRange,       "Auto-range");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::CurrentRange)5    << QString();
    QTest::addRow("max")     << (MultimeterService::CurrentRange)0xFE << QString();
}

void TestMultimeterService::toString_CurrentRange()
{
    QFETCH(MultimeterService::CurrentRange, range);
    QFETCH(QString, expected);
    QCOMPARE(MultimeterService::toString(range), expected);
}

void TestMultimeterService::minValue_CurrentRange_data()
{
    QTest::addColumn<MultimeterService::CurrentRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << MultimeterService::CurrentRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_10mA,        0);
    QTPOKIT_ADD_TEST_ROW(_10mA_to_30mA,    10);
    QTPOKIT_ADD_TEST_ROW(_30mA_to_150mA,   30);
    QTPOKIT_ADD_TEST_ROW(_150mA_to_300mA, 150);
    QTPOKIT_ADD_TEST_ROW(_300mA_to_3A,    300);
    QTPOKIT_ADD_TEST_ROW(AutoRange,       QStringLiteral("Auto"));
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::CurrentRange)5    << QVariant();
    QTest::addRow("max")     << (MultimeterService::CurrentRange)0xFE << QVariant();
}

void TestMultimeterService::minValue_CurrentRange()
{
    QFETCH(MultimeterService::CurrentRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(MultimeterService::minValue(range), expected);
}

void TestMultimeterService::maxValue_CurrentRange_data()
{
    QTest::addColumn<MultimeterService::CurrentRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << MultimeterService::CurrentRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_10mA,        10);
    QTPOKIT_ADD_TEST_ROW(_10mA_to_30mA,     30);
    QTPOKIT_ADD_TEST_ROW(_30mA_to_150mA,   150);
    QTPOKIT_ADD_TEST_ROW(_150mA_to_300mA,  300);
    QTPOKIT_ADD_TEST_ROW(_300mA_to_3A,    3000);
    QTPOKIT_ADD_TEST_ROW(AutoRange,       QStringLiteral("Auto"));
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::CurrentRange)5    << QVariant();
    QTest::addRow("max")     << (MultimeterService::CurrentRange)0xFE << QVariant();
}

void TestMultimeterService::maxValue_CurrentRange()
{
    QFETCH(MultimeterService::CurrentRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(MultimeterService::maxValue(range), expected);
}

void TestMultimeterService::toString_ResistanceRange_data()
{
    QTest::addColumn<MultimeterService::ResistanceRange>("range");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << MultimeterService::ResistanceRange::range << QStringLiteral(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_160,     "0 to 160 ohms");
    QTPOKIT_ADD_TEST_ROW(_160_to_330,   "160 to 330 ohms");
    QTPOKIT_ADD_TEST_ROW(_330_to_890,   "330 to 890 ohms");
    QTPOKIT_ADD_TEST_ROW(_890_to_1K5,   "890 to 1.5K ohms");
    QTPOKIT_ADD_TEST_ROW(_1K5_to_10K,   "1.5K to 10K ohms");
    QTPOKIT_ADD_TEST_ROW(_10K_to_100K,  "10K to 100K ohms");
    QTPOKIT_ADD_TEST_ROW(_100K_to_470K, "100K to 470K ohms");
    QTPOKIT_ADD_TEST_ROW(_470K_to_1M,   "470K to 1M ohms");
    QTPOKIT_ADD_TEST_ROW(AutoRange,     "Auto-range");
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::ResistanceRange)8    << QString();
    QTest::addRow("max")     << (MultimeterService::ResistanceRange)0xFE << QString();
}

void TestMultimeterService::toString_ResistanceRange()
{
    QFETCH(MultimeterService::ResistanceRange, range);
    QFETCH(QString, expected);
    QCOMPARE(MultimeterService::toString(range), expected);
}

void TestMultimeterService::minValue_ResistanceRange_data()
{
    QTest::addColumn<MultimeterService::ResistanceRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << MultimeterService::ResistanceRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_160,            0);
    QTPOKIT_ADD_TEST_ROW(_160_to_330,        160);
    QTPOKIT_ADD_TEST_ROW(_330_to_890,        330);
    QTPOKIT_ADD_TEST_ROW(_890_to_1K5,        890);
    QTPOKIT_ADD_TEST_ROW(_1K5_to_10K,       1500);
    QTPOKIT_ADD_TEST_ROW(_10K_to_100K,   10*1000);
    QTPOKIT_ADD_TEST_ROW(_100K_to_470K, 100*1000);
    QTPOKIT_ADD_TEST_ROW(_470K_to_1M,   470*1000);
    QTPOKIT_ADD_TEST_ROW(AutoRange,     QStringLiteral("Auto"));
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::ResistanceRange)8    << QVariant();
    QTest::addRow("max")     << (MultimeterService::ResistanceRange)0xFE << QVariant();
}

void TestMultimeterService::minValue_ResistanceRange()
{
    QFETCH(MultimeterService::ResistanceRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(MultimeterService::minValue(range), expected);
}

void TestMultimeterService::maxValue_ResistanceRange_data()
{
    QTest::addColumn<MultimeterService::ResistanceRange>("range");
    QTest::addColumn<QVariant>("expected");
    #define QTPOKIT_ADD_TEST_ROW(range, expected) \
        QTest::addRow(#range) << MultimeterService::ResistanceRange::range << QVariant(expected);
    QTPOKIT_ADD_TEST_ROW(_0_to_160,             160);
    QTPOKIT_ADD_TEST_ROW(_160_to_330,           330);
    QTPOKIT_ADD_TEST_ROW(_330_to_890,           890);
    QTPOKIT_ADD_TEST_ROW(_890_to_1K5,          1500);
    QTPOKIT_ADD_TEST_ROW(_1K5_to_10K,       10*1000);
    QTPOKIT_ADD_TEST_ROW(_10K_to_100K,     100*1000);
    QTPOKIT_ADD_TEST_ROW(_100K_to_470K,    470*1000);
    QTPOKIT_ADD_TEST_ROW(_470K_to_1M,   1*1000*1000);
    QTPOKIT_ADD_TEST_ROW(AutoRange,     QStringLiteral("Auto"));
    #undef QTPOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::ResistanceRange)8    << QVariant();
    QTest::addRow("max")     << (MultimeterService::ResistanceRange)0xFE << QVariant();
}

void TestMultimeterService::maxValue_ResistanceRange()
{
    QFETCH(MultimeterService::ResistanceRange, range);
    QFETCH(QVariant, expected);
    QCOMPARE(MultimeterService::maxValue(range), expected);
}

void TestMultimeterService::toString_Range_data()
{
    QTest::addColumn<MultimeterService::Range>("range");
    QTest::addColumn<MultimeterService::Mode>("mode");
    QTest::addColumn<QString>("expected");

    #define QTPOKIT_ADD_TEST_ROW(mode, member, range, expected) {\
        MultimeterService::Range rangeUnion; \
        rangeUnion.member = MultimeterService::range; \
        QTest::addRow(#mode "," #range) \
            << rangeUnion << MultimeterService::Mode::mode << QStringLiteral(expected); \
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

void TestMultimeterService::toString_Range()
{
    QFETCH(MultimeterService::Range, range);
    QFETCH(MultimeterService::Mode, mode);
    QFETCH(QString, expected);
    QCOMPARE(MultimeterService::toString(range, mode), expected);
}

void TestMultimeterService::readCharacteristics()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    MultimeterService service(nullptr);
    QVERIFY(!service.readCharacteristics());
}

void TestMultimeterService::readReadingCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    MultimeterService service(nullptr);
    QVERIFY(!service.readReadingCharacteristic());
}

void TestMultimeterService::setSettings()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    MultimeterService service(nullptr);
    QVERIFY(!service.setSettings({}));
}

void TestMultimeterService::reading()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    MultimeterService service(nullptr);
    QVERIFY(qIsNaN(service.reading().value));
}

void TestMultimeterService::enableReadingNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    MultimeterService service(nullptr);
    QVERIFY(!service.enableReadingNotifications());
}

void TestMultimeterService::disableReadingNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    MultimeterService service(nullptr);
    QVERIFY(!service.disableReadingNotifications());
}

QTEST_MAIN(TestMultimeterService)
