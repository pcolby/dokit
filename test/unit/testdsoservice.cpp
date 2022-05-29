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

void TestDsoService::encodeSettings_data()
{
//    QTest::addColumn<DsoService::Settings>("settings");
//    QTest::addColumn<bool>("updateIntervalIs32bit");
//    QTest::addColumn<QByteArray>("expected");

//    // Valid "stop" settings for Pokit Meter and Pokit Pro.
//    QTest::addRow("stop:meter")
//        << DsoService::Settings{
//            DsoService::Command::Stop,
//            0, DsoService::Mode::Idle,
//            { DsoService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << false
//        << QByteArray("\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 11);
//    QTest::addRow("stop:pro")
//        << DsoService::Settings{
//            DsoService::Command::Stop,
//            0, DsoService::Mode::Idle,
//            { DsoService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << true
//        << QByteArray("\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

//    // Valid "refresh" settings for Pokit Meter and Pokit Pro.
//    QTest::addRow("refresh:meter")
//        << DsoService::Settings{
//            DsoService::Command::Refresh,
//            0, DsoService::Mode::Idle,
//            { DsoService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << false
//        << QByteArray("\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 11);
//    QTest::addRow("refresh:pro")
//        << DsoService::Settings{
//            DsoService::Command::Refresh,
//            0, DsoService::Mode::Idle,
//            { DsoService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << true
//        << QByteArray("\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

//    // Contrived "start" settings.
//    QTest::addRow("zeroed")
//        << DsoService::Settings{
//            DsoService::Command::Start,
//            0, DsoService::Mode::Idle,
//            { DsoService::VoltageRange::_0_to_300mV }, 0, 0
//        }
//        << true
//        << QByteArray("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

//    // Realistic "start" settings example.
//    QTest::addRow("AcVoltage")
//        << DsoService::Settings{
//            DsoService::Command::Start,
//            0, DsoService::Mode::AcVoltage,
//            { DsoService::VoltageRange::_30V_to_60V }, 60*1000, (quint32)16537226070
//        }
//        << true
//        << QByteArray("\x00\x00\x00\x02\x05\x60\xea\x00\x00\x56\x0b\xb2\xd9", 13);

//    // "start" settings with the reserved parameter touched.
//    QTest::addRow("reserved")
//        << DsoService::Settings{
//            DsoService::Command::Start,
//            (quint16)0xAABB, DsoService::Mode::AcVoltage,
//            { DsoService::VoltageRange::_30V_to_60V }, 60*1000u, (quint32)16537226070
//        }
//        << true
//        << QByteArray("\x00\xBB\xAA\x02\x05\x60\xea\x00\x00\x56\x0b\xb2\xd9", 13);
}

void TestDsoService::encodeSettings()
{
//    QFETCH(DsoService::Settings, settings);
//    QFETCH(bool, updateIntervalIs32bit);
//    QFETCH(QByteArray, expected);
//    QCOMPARE(DsoServicePrivate::encodeSettings(settings, updateIntervalIs32bit), expected);
}

void TestDsoService::parseMetadata_data()
{
//    QTest::addColumn<QByteArray>("value");
//    QTest::addColumn<DsoService::Metadata>("expected");

//    QTest::addRow("null") << QByteArray()
//        << DsoService::Metadata{
//            DsoService::LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(),
//            DsoService::Mode::Idle, { DsoService::VoltageRange::_0_to_300mV },
//            0, 0, 0
//        };

//    // Metadata must be at least 15 bytes to be valid / parsable.
//    QTest::addRow("too-small") << QByteArray(14, '\xFF')
//        << DsoService::Metadata{
//            DsoService::LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(),
//            DsoService::Mode::Idle, { DsoService::VoltageRange::_0_to_300mV },
//            0, 0, 0
//        };

//    // Sample from a real Pokit Meter device.
//    QTest::addRow("PokitMeter")
//        << QByteArray("\x00\x9f\x0f\x49\x37\x00\x04\x3c\x00\x00\x00\xe9\xbb\x8c\x62", 15)
//        << DsoService::Metadata{
//            DsoService::LoggerStatus::Done, 1.19842e-05f,
//            DsoService::Mode::Idle, { DsoService::VoltageRange::_12V_to_30V },
//            60000, 0, 1653390313 // 2022-05-24 21:05:13.000 AEST.
//        };

//    // Sample from a real Pokit Pro device.
//    QTest::addRow("PokitMeter")
//        << QByteArray("\x00\x39\xf0\x45\x3c\x00\x04\x60\xea\x00\x00\x0d"
//                      "\x00\x00\x00\x30\x38\x00\x00\x43\xb9\x8c\x62", 23)
//        << DsoService::Metadata{
//            DsoService::LoggerStatus::Done, 0.0120812f,
//            DsoService::Mode::Idle, { DsoService::VoltageRange::_12V_to_30V },
//            60000, 13, 1653389635 // 2022-05-24 20:53:55.000 AEST.
//        };

//    // Made-up sample *extended* from a real Pokit Pro device (by appending 3 erroneous bytes).
//    QTest::addRow("PokitMeter")
//        << QByteArray("\x00\x39\xf0\x45\x3c\x00\x04\x60\xea\x00\x00\x0d"
//                      "\x00\x00\x00\x30\x38\x00\x00\x43\xb9\x8c\x62\x01\x02\0x3", 26)
//        << DsoService::Metadata{
//           DsoService::LoggerStatus::Done, 0.0120812f,
//           DsoService::Mode::Idle, { DsoService::VoltageRange::_12V_to_30V },
//           0, 0, 0 // Will safely parse all but these last three.
//        };
}

void TestDsoService::parseMetadata()
{
//    QFETCH(QByteArray, value);
//    QFETCH(DsoService::Metadata, expected);
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
//    const DsoService::Metadata actual = DsoServicePrivate::parseMetadata(value);
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

void TestDsoService::parseSamples_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<DsoService::Samples>("expected");

    QTest::addRow("empty") << QByteArray() << DsoService::Samples();

    // Real, albeit boring, sample from a Pokit Pro device.
    QTest::addRow("PokitPro")
        << QByteArray("\xff\x7f\xff\x7f\xff\x7f\xff\x7f\xff\x7f", 10)
        << DsoService::Samples({32767,32767,32767,32767,32767});

    // Check bytes are parsed in the correct (little-endian) order.
    QTest::addRow("endianness")
        << QByteArray("\x00\x00\x00\xff\xff\x00\xff\xff", 8)
        << DsoService::Samples({0,-256,255,-1});

    // Data must be even-length to be parsed.
    QTest::addRow("odd") << QByteArray(3, '\xff') << DsoService::Samples();
}

void TestDsoService::parseSamples()
{
    QFETCH(QByteArray, data);
    QFETCH(DsoService::Samples, expected);
    if ((data.size()%2) != 0) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Samples value has odd size \\d+ \\(should be even\\): 0x[a-zA-Z0-9,]*$")));
    }
    QCOMPARE(DsoServicePrivate::parseSamples(data), expected);
}

void TestDsoService::characteristicRead()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    DsoService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic read for DSO service .*$")));
    service.d_func()->characteristicRead(QLowEnergyCharacteristic(), QByteArray());
}

void TestDsoService::characteristicWritten()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    DsoService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic written for DSO service .*$")));
    service.d_func()->characteristicWritten(QLowEnergyCharacteristic(), QByteArray());
}

void TestDsoService::characteristicChanged()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    DsoService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic notified for DSO service .*$")));
    service.d_func()->characteristicChanged(QLowEnergyCharacteristic(), QByteArray());
}

QTEST_MAIN(TestDsoService)
