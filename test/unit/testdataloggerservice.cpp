// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdataloggerservice.h"

#include <qtpokit/dataloggerservice.h>
#include "dataloggerservice_p.h"

#include <QRegularExpression>

Q_DECLARE_METATYPE(DataLoggerService::Mode);
Q_DECLARE_METATYPE(DataLoggerService::VoltageRange);
Q_DECLARE_METATYPE(DataLoggerService::CurrentRange);
Q_DECLARE_METATYPE(DataLoggerService::Range);
Q_DECLARE_METATYPE(DataLoggerService::Settings);
Q_DECLARE_METATYPE(DataLoggerService::Metadata);

void TestDataLoggerService::toString_Mode_data()
{
    QTest::addColumn<DataLoggerService::Mode>("mode");
    QTest::addColumn<QString>("expected");
    #define QTPOKIT_ADD_TEST_ROW(mode, expected) \
        QTest::addRow(#mode) << DataLoggerService::Mode::mode << QStringLiteral(expected)
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
        QTest::addRow(#range) << DataLoggerService::VoltageRange::range << QStringLiteral(expected)
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
        QTest::addRow(#range) << DataLoggerService::VoltageRange::range << QVariant(expected)
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
        QTest::addRow(#range) << DataLoggerService::VoltageRange::range << QVariant(expected)
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
        QTest::addRow(#range) << DataLoggerService::CurrentRange::range << QStringLiteral(expected)
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
        QTest::addRow(#range) << DataLoggerService::CurrentRange::range << QVariant(expected)
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
        QTest::addRow(#range) << DataLoggerService::CurrentRange::range << QVariant(expected)
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

    #define QTPOKIT_ADD_TEST_ROW(mode, member, range, expected) {\
        DataLoggerService::Range rangeUnion; \
        rangeUnion.member = DataLoggerService::range; \
        QTest::addRow(#mode "," #range) \
            << rangeUnion << DataLoggerService::Mode::mode << QStringLiteral(expected); \
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

void TestDataLoggerService::disableMetadataNotifications()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DataLoggerService service(nullptr);
    QVERIFY(!service.disableMetadataNotifications());
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

void TestDataLoggerService::encodeSettings_data()
{
    QTest::addColumn<DataLoggerService::Settings>("settings");
    QTest::addColumn<bool>("updateIntervalIs32bit");
    QTest::addColumn<QByteArray>("expected");

    // Valid "stop" settings for Pokit Meter and Pokit Pro.
    QTest::addRow("stop:meter")
        << DataLoggerService::Settings{
            DataLoggerService::Command::Stop,
            0, DataLoggerService::Mode::Idle,
            { DataLoggerService::VoltageRange::_0_to_300mV }, 0, 0
        }
        << false
        << QByteArray("\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 11);
    QTest::addRow("stop:pro")
        << DataLoggerService::Settings{
            DataLoggerService::Command::Stop,
            0, DataLoggerService::Mode::Idle,
            { DataLoggerService::VoltageRange::_0_to_300mV }, 0, 0
        }
        << true
        << QByteArray("\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

    // Valid "refresh" settings for Pokit Meter and Pokit Pro.
    QTest::addRow("refresh:meter")
        << DataLoggerService::Settings{
            DataLoggerService::Command::Refresh,
            0, DataLoggerService::Mode::Idle,
            { DataLoggerService::VoltageRange::_0_to_300mV }, 0, 0
        }
        << false
        << QByteArray("\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 11);
    QTest::addRow("refresh:pro")
        << DataLoggerService::Settings{
            DataLoggerService::Command::Refresh,
            0, DataLoggerService::Mode::Idle,
            { DataLoggerService::VoltageRange::_0_to_300mV }, 0, 0
        }
        << true
        << QByteArray("\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

    // Contrived "start" settings.
    QTest::addRow("zeroed")
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start,
            0, DataLoggerService::Mode::Idle,
            { DataLoggerService::VoltageRange::_0_to_300mV }, 0, 0
        }
        << true
        << QByteArray("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

    // Realistic "start" settings example.
    QTest::addRow("AcVoltage")
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start,
            0, DataLoggerService::Mode::AcVoltage,
            { DataLoggerService::VoltageRange::_30V_to_60V }, 60*1000, (quint32)16537226070
        }
        << true
        << QByteArray("\x00\x00\x00\x02\x05\x60\xea\x00\x00\x56\x0b\xb2\xd9", 13);

    // "start" settings with the reserved parameter touched.
    QTest::addRow("reserved")
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start,
            (quint16)0xAABB, DataLoggerService::Mode::AcVoltage,
            { DataLoggerService::VoltageRange::_30V_to_60V }, 60*1000u, (quint32)16537226070
        }
        << true
        << QByteArray("\x00\xBB\xAA\x02\x05\x60\xea\x00\x00\x56\x0b\xb2\xd9", 13);
}

void TestDataLoggerService::encodeSettings()
{
    QFETCH(DataLoggerService::Settings, settings);
    QFETCH(bool, updateIntervalIs32bit);
    QFETCH(QByteArray, expected);
    QCOMPARE(DataLoggerServicePrivate::encodeSettings(settings, updateIntervalIs32bit), expected);
}

void TestDataLoggerService::parseMetadata_data()
{
    QTest::addColumn<QByteArray>("value");
    QTest::addColumn<DataLoggerService::Metadata>("expected");

    QTest::addRow("null") << QByteArray()
        << DataLoggerService::Metadata{
            DataLoggerService::LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(),
            DataLoggerService::Mode::Idle, { DataLoggerService::VoltageRange::_0_to_300mV },
            0, 0, 0
        };

    // Metadata must be at least 15 bytes to be valid / parsable.
    QTest::addRow("too-small") << QByteArray(14, '\xFF')
        << DataLoggerService::Metadata{
            DataLoggerService::LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(),
            DataLoggerService::Mode::Idle, { DataLoggerService::VoltageRange::_0_to_300mV },
            0, 0, 0
        };

    // Sample from a real Pokit Meter device.
    QTest::addRow("PokitMeter")
        << QByteArray("\x00\x9f\x0f\x49\x37\x00\x04\x3c\x00\x00\x00\xe9\xbb\x8c\x62", 15)
        << DataLoggerService::Metadata{
            DataLoggerService::LoggerStatus::Done, 1.198417067e-05f,
            DataLoggerService::Mode::Idle, { DataLoggerService::VoltageRange::_12V_to_30V },
            60000, 0, 1653390313 // 2022-05-24 21:05:13.000 AEST.
        };

    // Sample from a real Pokit Pro device.
    QTest::addRow("PokitPro")
        << QByteArray("\x00\x39\xf0\x45\x3c\x00\x04\x60\xea\x00\x00\x0d"
                      "\x00\x00\x00\x30\x38\x00\x00\x43\xb9\x8c\x62", 23)
        << DataLoggerService::Metadata{
            DataLoggerService::LoggerStatus::Done, 0.01208119933f,
            DataLoggerService::Mode::Idle, { DataLoggerService::VoltageRange::_12V_to_30V },
            60000, 13, 1653389635 // 2022-05-24 20:53:55.000 AEST.
        };

    // Made-up sample *extended* from a real Pokit Pro device (by appending 3 erroneous bytes).
    QTest::addRow("PokitPro")
        << QByteArray("\x00\x39\xf0\x45\x3c\x00\x04\x60\xea\x00\x00\x0d"
                      "\x00\x00\x00\x30\x38\x00\x00\x43\xb9\x8c\x62\x01\x02\0x3", 26)
        << DataLoggerService::Metadata{
           DataLoggerService::LoggerStatus::Done, 0.01208119933f,
           DataLoggerService::Mode::Idle, { DataLoggerService::VoltageRange::_12V_to_30V },
           0, 0, 0 // Will safely parse all but these last three.
        };
}

void TestDataLoggerService::parseMetadata()
{
    QFETCH(QByteArray, value);
    QFETCH(DataLoggerService::Metadata, expected);
    if (value.size() < 15) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Metadata requires \\d+ bytes, but only \\d+ present: 0x[a-zA-Z0-9,]*$")));
    }
    if (value.size() > 23) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Metadata has \\d+ extraneous bytes: 0x[a-zA-Z0-9,]*$")));
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Cannot decode metadata of \\d+ bytes: 0x[a-zA-Z0-9,.]*$")));
    }
    const DataLoggerService::Metadata actual = DataLoggerServicePrivate::parseMetadata(value);
    QCOMPARE(actual.status, expected.status);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    QCOMPARE(qIsFinite(actual.scale),    qIsFinite(expected.scale));
    QCOMPARE(qIsInf(actual.scale),       qIsInf(expected.scale));
    QCOMPARE(qIsNaN(actual.scale),       qIsNaN(expected.scale));
    QCOMPARE(qFuzzyIsNull(actual.scale), qFuzzyIsNull(expected.scale));
    if ((qIsFinite(actual.scale)) && (!qFuzzyIsNull(actual.scale))) {
        QCOMPARE(actual.scale, expected.scale);
    }
    #else
    QCOMPARE(actual.scale, expected.scale);
    #endif
    QCOMPARE(actual.mode, expected.mode);
    QCOMPARE(actual.range.currentRange, expected.range.currentRange);
    QCOMPARE(actual.range.voltageRange, expected.range.voltageRange);
    QCOMPARE(actual.updateInterval, expected.updateInterval);
    QCOMPARE(actual.numberOfSamples, expected.numberOfSamples);
    QCOMPARE(actual.timestamp, expected.timestamp);
}

void TestDataLoggerService::parseSamples_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<DataLoggerService::Samples>("expected");

    QTest::addRow("empty") << QByteArray() << DataLoggerService::Samples();

    // Real, albeit boring, sample from a Pokit Pro device.
    QTest::addRow("PokitPro")
        << QByteArray("\xff\x7f\xff\x7f\xff\x7f\xff\x7f\xff\x7f", 10)
        << DataLoggerService::Samples({32767,32767,32767,32767,32767});

    // Check bytes are parsed in the correct (little-endian) order.
    QTest::addRow("endianness")
        << QByteArray("\x00\x00\x00\xff\xff\x00\xff\xff", 8)
        << DataLoggerService::Samples({0,-256,255,-1});

    // Data must be even-length to be parsed.
    QTest::addRow("odd") << QByteArray(3, '\xff') << DataLoggerService::Samples();
}

void TestDataLoggerService::parseSamples()
{
    QFETCH(QByteArray, data);
    QFETCH(DataLoggerService::Samples, expected);
    if ((data.size()%2) != 0) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Samples value has odd size \\d+ \\(should be even\\): 0x[a-zA-Z0-9,]*$")));
    }
    QCOMPARE(DataLoggerServicePrivate::parseSamples(data), expected);
}

void TestDataLoggerService::characteristicRead()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    DataLoggerService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic read for Data Logger service .*$")));
    service.d_func()->characteristicRead(QLowEnergyCharacteristic(), QByteArray());
}

void TestDataLoggerService::characteristicWritten()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    DataLoggerService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic written for Data Logger service .*$")));
    service.d_func()->characteristicWritten(QLowEnergyCharacteristic(), QByteArray());
}

void TestDataLoggerService::characteristicChanged()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    DataLoggerService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic notified for Data Logger service .*$")));
    service.d_func()->characteristicChanged(QLowEnergyCharacteristic(), QByteArray());
}

QTEST_MAIN(TestDataLoggerService)
