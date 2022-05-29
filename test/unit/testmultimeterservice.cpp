// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testmultimeterservice.h"

#include <qtpokit/multimeterservice.h>
#include "multimeterservice_p.h"

#include <QRegularExpression>

Q_DECLARE_METATYPE(MultimeterService::Mode);
Q_DECLARE_METATYPE(MultimeterService::VoltageRange);
Q_DECLARE_METATYPE(MultimeterService::CurrentRange);
Q_DECLARE_METATYPE(MultimeterService::ResistanceRange);
Q_DECLARE_METATYPE(MultimeterService::Range);
Q_DECLARE_METATYPE(MultimeterService::Settings);
Q_DECLARE_METATYPE(MultimeterService::Reading);

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

void TestMultimeterService::encodeSettings_data()
{
    QTest::addColumn<MultimeterService::Settings>("settings");
    QTest::addColumn<QByteArray>("expected");

    QTest::addRow("zeroed")
        << MultimeterService::Settings{
           MultimeterService::Mode::Idle, { MultimeterService::VoltageRange::_0_to_300mV }, 0
        }
        << QByteArray("\x00\x00\x00\x00\x00\x00", 6);

    QTest::addRow("AcVoltage")
        << MultimeterService::Settings{
           MultimeterService::Mode::AcVoltage, { MultimeterService::VoltageRange::_2V_to_6V },
           1000
        }
        << QByteArray("\x02\x02\xE8\x03\x00\x00", 6);

    QTest::addRow("Diode")
        << MultimeterService::Settings{
           MultimeterService::Mode::Diode, { MultimeterService::VoltageRange::AutoRange },
           5000
        }
        << QByteArray("\x06\xFF\x88\x13\x00\x00", 6);

    QTest::addRow("Resistance")
        << MultimeterService::Settings{
           MultimeterService::Mode::Resistance, { MultimeterService::VoltageRange::AutoRange },
           60*1000
        }
        << QByteArray("\x05\xFF\x60\xEA\x00\x00", 6);
}

void TestMultimeterService::encodeSettings()
{
    QFETCH(MultimeterService::Settings, settings);
    QFETCH(QByteArray, expected);
    QCOMPARE(MultimeterServicePrivate::encodeSettings(settings), expected);
}

void TestMultimeterService::parseReading_data()
{
    QTest::addColumn<QByteArray>("value");
    QTest::addColumn<MultimeterService::Reading>("expected");

    QTest::addRow("null") << QByteArray()
        << MultimeterService::Reading{
           MultimeterService::MeterStatus::Error, std::numeric_limits<float>::quiet_NaN(),
           MultimeterService::Mode::Idle, { MultimeterService::VoltageRange::AutoRange }
        };

    // Metadata must be at least 7 bytes to be valid / parsable.
    QTest::addRow("too-small") << QByteArray(6, '\xFF')
        << MultimeterService::Reading{
           MultimeterService::MeterStatus::Error, std::numeric_limits<float>::quiet_NaN(),
           MultimeterService::Mode::Idle, { MultimeterService::VoltageRange::AutoRange }
        };

    // Sample from a real Pokit Meter device.
    QTest::addRow("PokitMeter")
        << QByteArray("\x00\x00\x00\x00\x00\x01\x03", 7)
        << MultimeterService::Reading{
           MultimeterService::MeterStatus::AutoRangeOff, 0.0f,
           MultimeterService::Mode::DcVoltage, { MultimeterService::VoltageRange::_6V_to_12V }
        };

    // Sample from a real Pokit Pro device.
    QTest::addRow("PokitMeter")
        << QByteArray("\x00\x94\x89\xfa\x3b\x02\x00", 7)
        << MultimeterService::Reading{
           MultimeterService::MeterStatus::AutoRangeOff, 0.0076458f,
           MultimeterService::Mode::AcVoltage, { MultimeterService::VoltageRange::_0_to_300mV }
        };

    // Made-up sample *extended* from a real Pokit Pro device (by appending 3 erroneous bytes).
    QTest::addRow("PokitMeter")
        << QByteArray("\x00\x94\x89\xfa\x3b\x02\x00\x01\x02\x03", 10)
        << MultimeterService::Reading{
           MultimeterService::MeterStatus::AutoRangeOff, 0.0076458f,
           MultimeterService::Mode::AcVoltage, { MultimeterService::VoltageRange::_0_to_300mV }
        };
}

void TestMultimeterService::parseReading()
{
    QFETCH(QByteArray, value);
    QFETCH(MultimeterService::Reading, expected);
    if (value.size() < 7) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Reading requires \\d+ bytes, but only \\d+ present: 0x[a-zA-Z0-9,]*$")));
    }
    if (value.size() > 7) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Reading has \\d+ extraneous bytes: 0x[a-zA-Z0-9,]*$")));
    }
    const MultimeterService::Reading actual = MultimeterServicePrivate::parseReading(value);
    QCOMPARE(actual.status, expected.status);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    QCOMPARE(qIsFinite(actual.value),    qIsFinite(expected.value));
    QCOMPARE(qIsInf(actual.value),       qIsInf(expected.value));
    QCOMPARE(qIsNaN(actual.value),       qIsNaN(expected.value));
    QCOMPARE(qFuzzyIsNull(actual.value), qFuzzyIsNull(expected.value));
    if ((qIsFinite(actual.value)) && (!qFuzzyIsNull(actual.value))) {
        QCOMPARE(actual.value, expected.value);
    }
    #else
    QCOMPARE(actual.value, expected.value);
    #endif
    QCOMPARE(actual.mode, expected.mode);
    QCOMPARE(actual.range.currentRange, expected.range.currentRange);
    QCOMPARE(actual.range.voltageRange, expected.range.voltageRange);
}

void TestMultimeterService::characteristicRead()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    MultimeterService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic read for Multimeter service .*$")));
    service.d_func()->characteristicRead(QLowEnergyCharacteristic(), QByteArray());
}

void TestMultimeterService::characteristicWritten()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    MultimeterService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic written for Multimeter service .*$")));
    service.d_func()->characteristicWritten(QLowEnergyCharacteristic(), QByteArray());
}

void TestMultimeterService::characteristicChanged()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    MultimeterService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic notified for Multimeter service .*$")));
    service.d_func()->characteristicChanged(QLowEnergyCharacteristic(), QByteArray());
}

QTEST_MAIN(TestMultimeterService)
