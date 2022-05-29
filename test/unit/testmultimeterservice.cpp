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
//    QTest::addColumn<MultimeterService::Settings>("settings");
//    QTest::addColumn<bool>("updateIntervalIs32bit");
//    QTest::addColumn<QByteArray>("expected");

//    // Valid "stop" settings for Pokit Meter and Pokit Pro.
//    QTest::addRow("stop:meter")
//        << MultimeterService::Settings{
//            MultimeterService::Command::Stop,
//            0, MultimeterService::Mode::Idle,
//            { MultimeterService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << false
//        << QByteArray("\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 11);
//    QTest::addRow("stop:pro")
//        << MultimeterService::Settings{
//            MultimeterService::Command::Stop,
//            0, MultimeterService::Mode::Idle,
//            { MultimeterService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << true
//        << QByteArray("\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

//    // Valid "refresh" settings for Pokit Meter and Pokit Pro.
//    QTest::addRow("refresh:meter")
//        << MultimeterService::Settings{
//            MultimeterService::Command::Refresh,
//            0, MultimeterService::Mode::Idle,
//            { MultimeterService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << false
//        << QByteArray("\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 11);
//    QTest::addRow("refresh:pro")
//        << MultimeterService::Settings{
//            MultimeterService::Command::Refresh,
//            0, MultimeterService::Mode::Idle,
//            { MultimeterService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << true
//        << QByteArray("\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

//    // Contrived "start" settings.
//    QTest::addRow("zeroed")
//        << MultimeterService::Settings{
//            MultimeterService::Command::Start,
//            0, MultimeterService::Mode::Idle,
//            { MultimeterService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << true
//        << QByteArray("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

//    // Realistic "start" settings example.
//    QTest::addRow("AcVoltage")
//        << MultimeterService::Settings{
//            MultimeterService::Command::Start,
//            0, MultimeterService::Mode::AcVoltage,
//            { MultimeterService::VoltageRange::_30V_to_60V }, 60*1000, (quint32)16537226070
//        }
//        << true
//        << QByteArray("\x00\x00\x00\x02\x05\x60\xea\x00\x00\x56\x0b\xb2\xd9", 13);

//    // "start" settings with the reserved parameter touched.
//    QTest::addRow("reserved")
//        << MultimeterService::Settings{
//            MultimeterService::Command::Start,
//            (quint16)0xAABB, MultimeterService::Mode::AcVoltage,
//            { MultimeterService::VoltageRange::_30V_to_60V }, 60*1000u, (quint32)16537226070
//        }
//        << true
//        << QByteArray("\x00\xBB\xAA\x02\x05\x60\xea\x00\x00\x56\x0b\xb2\xd9", 13);
}

void TestMultimeterService::encodeSettings()
{
//    QFETCH(MultimeterService::Settings, settings);
//    QFETCH(bool, updateIntervalIs32bit);
//    QFETCH(QByteArray, expected);
//    QCOMPARE(MultimeterServicePrivate::encodeSettings(settings, updateIntervalIs32bit), expected);
}

void TestMultimeterService::parseReading_data()
{
//    QTest::addColumn<QByteArray>("value");
//    QTest::addColumn<MultimeterService::Metadata>("expected");

//    QTest::addRow("null") << QByteArray()
//        << MultimeterService::Metadata{
//            MultimeterService::LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(),
//            MultimeterService::Mode::Idle, { MultimeterService::VoltageRange::_0_to_300mV },
//            0, 0, 0
//        };

//    // Metadata must be at least 15 bytes to be valid / parsable.
//    QTest::addRow("too-small") << QByteArray(14, '\xFF')
//        << MultimeterService::Metadata{
//            MultimeterService::LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(),
//            MultimeterService::Mode::Idle, { MultimeterService::VoltageRange::_0_to_300mV },
//            0, 0, 0
//        };

//    // Sample from a real Pokit Meter device.
//    QTest::addRow("PokitMeter")
//        << QByteArray("\x00\x9f\x0f\x49\x37\x00\x04\x3c\x00\x00\x00\xe9\xbb\x8c\x62", 15)
//        << MultimeterService::Metadata{
//            MultimeterService::LoggerStatus::Done, 1.19842e-05f,
//            MultimeterService::Mode::Idle, { MultimeterService::VoltageRange::_12V_to_30V },
//            60000, 0, 1653390313 // 2022-05-24 21:05:13.000 AEST.
//        };

//    // Sample from a real Pokit Pro device.
//    QTest::addRow("PokitMeter")
//        << QByteArray("\x00\x39\xf0\x45\x3c\x00\x04\x60\xea\x00\x00\x0d"
//                      "\x00\x00\x00\x30\x38\x00\x00\x43\xb9\x8c\x62", 23)
//        << MultimeterService::Metadata{
//            MultimeterService::LoggerStatus::Done, 0.0120812f,
//            MultimeterService::Mode::Idle, { MultimeterService::VoltageRange::_12V_to_30V },
//            60000, 13, 1653389635 // 2022-05-24 20:53:55.000 AEST.
//        };

//    // Made-up sample *extended* from a real Pokit Pro device (by appending 3 erroneous bytes).
//    QTest::addRow("PokitMeter")
//        << QByteArray("\x00\x39\xf0\x45\x3c\x00\x04\x60\xea\x00\x00\x0d"
//                      "\x00\x00\x00\x30\x38\x00\x00\x43\xb9\x8c\x62\x01\x02\0x3", 26)
//        << MultimeterService::Metadata{
//           MultimeterService::LoggerStatus::Done, 0.0120812f,
//           MultimeterService::Mode::Idle, { MultimeterService::VoltageRange::_12V_to_30V },
//           0, 0, 0 // Will safely parse all but these last three.
//        };
}

void TestMultimeterService::parseReading()
{
//    QFETCH(QByteArray, value);
//    QFETCH(MultimeterService::Metadata, expected);
//    if (value.size() < 15) {
//        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
//            "^Metadata requires \\d+ bytes, but only \\d+ present: 0x[a-zA-Z0-9,]*$")));
//    }
//    if (value.size() > 23) {
//        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
//            "^Metadata has \\d+ extraneous bytes: 0x[a-zA-Z0-9,]*$")));
//        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
//            "^Cannot decode metadata of \\d+ bytes: 0x[a-zA-Z0-9,.]*$")));
//    }
//    const MultimeterService::Metadata actual = MultimeterServicePrivate::parseMetadata(value);
//    QCOMPARE(actual.status, expected.status);
//    #if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
//    QCOMPARE(qIsFinite(actual.scale),    qIsFinite(expected.scale));
//    QCOMPARE(qIsInf(actual.scale),       qIsInf(expected.scale));
//    QCOMPARE(qIsNaN(actual.scale),       qIsNaN(expected.scale));
//    QCOMPARE(qFuzzyIsNull(actual.scale), qFuzzyIsNull(expected.scale));
//    if ((qIsFinite(actual.scale)) && (!qFuzzyIsNull(actual.scale))) {
//        QCOMPARE(actual.scale, expected.scale);
//    }
//    #else
//    QCOMPARE(actual.scale, expected.scale);
//    #endif
//    QCOMPARE(actual.mode, expected.mode);
//    QCOMPARE(actual.range.currentRange, expected.range.currentRange);
//    QCOMPARE(actual.range.voltageRange, expected.range.voltageRange);
//    QCOMPARE(actual.updateInterval, expected.updateInterval);
//    QCOMPARE(actual.numberOfSamples, expected.numberOfSamples);
//    QCOMPARE(actual.timestamp, expected.timestamp);
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
