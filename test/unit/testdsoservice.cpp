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
        QTest::addRow(#mode) << DsoService::Mode::mode << QStringLiteral(expected)
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
        QTest::addRow(#range) << DsoService::VoltageRange::range << QStringLiteral(expected)
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
        QTest::addRow(#range) << DsoService::VoltageRange::range << QVariant(expected)
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
        QTest::addRow(#range) << DsoService::VoltageRange::range << QVariant(expected)
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
        QTest::addRow(#range) << DsoService::CurrentRange::range << QStringLiteral(expected)
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
        QTest::addRow(#range) << DsoService::CurrentRange::range << QVariant(expected)
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
        QTest::addRow(#range) << DsoService::CurrentRange::range << QVariant(expected)
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

void TestDsoService::range_Range()
{
    const DsoService::Range range;
    QCOMPARE((quint8)range.currentRange, (quint8)0);
    QCOMPARE((quint8)range.voltageRange, (quint8)0);

    const DsoService::Range voltage(DsoService::VoltageRange::_6V_to_12V);
    QCOMPARE(voltage.voltageRange, DsoService::VoltageRange::_6V_to_12V);

    const DsoService::Range current(DsoService::CurrentRange::_150mA_to_300mA);
    QCOMPARE(voltage.currentRange, DsoService::CurrentRange::_150mA_to_300mA);
}

void TestDsoService::range_Operators()
{
    QVERIFY(DsoService::Range(DsoService::VoltageRange::_300mV_to_2V) ==
            DsoService::Range(DsoService::VoltageRange::_300mV_to_2V));

    QVERIFY(DsoService::Range(DsoService::VoltageRange::_300mV_to_2V) !=
            DsoService::Range(DsoService::VoltageRange::_2V_to_6V));

    QVERIFY(DsoService::Range(DsoService::VoltageRange::_300mV_to_2V) <
            DsoService::Range(DsoService::VoltageRange::_2V_to_6V));

    QVERIFY(DsoService::Range(DsoService::VoltageRange::_300mV_to_2V) <=
            DsoService::Range(DsoService::VoltageRange::_2V_to_6V));

    QVERIFY(!(DsoService::Range(DsoService::VoltageRange::_2V_to_6V) <=
              DsoService::Range(DsoService::VoltageRange::_300mV_to_2V)));

    QVERIFY(DsoService::Range(DsoService::VoltageRange::_2V_to_6V) >
            DsoService::Range(DsoService::VoltageRange::_300mV_to_2V));

    QVERIFY(DsoService::Range(DsoService::VoltageRange::_2V_to_6V) >=
            DsoService::Range(DsoService::VoltageRange::_300mV_to_2V));

    QVERIFY(!(DsoService::Range(DsoService::VoltageRange::_300mV_to_2V) >=
              DsoService::Range(DsoService::VoltageRange::_2V_to_6V)));
}

void TestDsoService::toString_Range_data()
{
    QTest::addColumn<DsoService::Range>("range");
    QTest::addColumn<DsoService::Mode>("mode");
    QTest::addColumn<QString>("expected");

    #define QTPOKIT_ADD_TEST_ROW(mode, member, range, expected) {\
        QTest::addRow(#mode "," #range) << DsoService::Range(DsoService::range) \
            << DsoService::Mode::mode << QStringLiteral(expected); \
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
    QTest::addColumn<DsoService::Settings>("settings");
    QTest::addColumn<QByteArray>("expected");

    // Valid "refresh" settings.
    QTest::addRow("refresh")
        << DsoService::Settings{
           DsoService::Command::ResendData,
           0, DsoService::Mode::Idle,
           { DsoService::VoltageRange::_0_to_300mV }, 0, 0
        }
        << QByteArray("\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

    // Contrived "start" settings.
    QTest::addRow("zeroed")
        << DsoService::Settings{
           DsoService::Command::FreeRunning,
           0, DsoService::Mode::Idle,
           { DsoService::VoltageRange::_0_to_300mV }, 0, 0
        }
        << QByteArray("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

    // Realistic "start" settings example.
    QTest::addRow("AcVoltage")
        << DsoService::Settings{
           DsoService::Command::RisingEdgeTrigger,
           1.0f, DsoService::Mode::AcVoltage,
           { DsoService::VoltageRange::_12V_to_30V }, 1000000, 1000
        }
        << QByteArray("\x01\x00\x00\x80\x3f\x02\x04\x40\x42\x0f\x00\xe8\x03", 13);
}

void TestDsoService::encodeSettings()
{
    QFETCH(DsoService::Settings, settings);
    QFETCH(QByteArray, expected);
    QCOMPARE(DsoServicePrivate::encodeSettings(settings), expected);
}

void TestDsoService::parseMetadata_data()
{
    QTest::addColumn<QByteArray>("value");
    QTest::addColumn<DsoService::Metadata>("expected");

    QTest::addRow("null") << QByteArray()
        << DsoService::Metadata{
           DsoService::DsoStatus::Error, std::numeric_limits<float>::quiet_NaN(),
           DsoService::Mode::Idle, { DsoService::VoltageRange::_0_to_300mV },
           0, 0, 0
        };

    // Metadata must be at least 17 bytes to be valid / parsable.
    QTest::addRow("too-small") << QByteArray(16, '\xFF')
        << DsoService::Metadata{
           DsoService::DsoStatus::Error, std::numeric_limits<float>::quiet_NaN(),
           DsoService::Mode::Idle, { DsoService::VoltageRange::_0_to_300mV },
           0, 0, 0
        };


    // Sample from a real Pokit Meter device.
    QTest::addRow("PokitMeter")
        << QByteArray("\x00\x98\xf7\x8b\x33\x02\x00\x40\x42\x0f\x00\x0a\x00\x0a\x00\x00\x00", 17)
        << DsoService::Metadata{
           DsoService::DsoStatus::Done, 6.517728934e-08f,
           DsoService::Mode::AcVoltage, { DsoService::VoltageRange::_0_to_300mV },
           1*1000*1000, 10, 10
        };

    // Sample from a real Pokit Pro device; note is has 4 extra unknown bytes.
    QTest::addRow("PokitPro")
        << QByteArray("\x00\xcc\xec\xba\x33\x02\x00\x40\x42\x0f\x00"
                      "\xe8\x03\x0a\x00\x00\x00\x14\x00\x00\x00", 21)
        << DsoService::Metadata{
              DsoService::DsoStatus::Done, 8.704373045e-08f,
              DsoService::Mode::AcVoltage, { DsoService::VoltageRange::_0_to_300mV },
              1*1000*1000, 1000, 10
        };
}

void TestDsoService::parseMetadata()
{
    QFETCH(QByteArray, value);
    QFETCH(DsoService::Metadata, expected);
    if (value.size() < 17) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Metadata requires \\d+ bytes, but only \\d+ present: 0x[a-zA-Z0-9,]*$")));
    }
    if (value.size() > 17) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Metadata has \\d+ extraneous bytes: 0x[a-zA-Z0-9,]*$")));
    }
    const DsoService::Metadata actual = DsoServicePrivate::parseMetadata(value);
    QCOMPARE(actual.status,             expected.status);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    QCOMPARE(qIsFinite(actual.scale),    qIsFinite(expected.scale));
    QCOMPARE(qIsInf(actual.scale),       qIsInf(expected.scale));
    QCOMPARE(qIsNaN(actual.scale),       qIsNaN(expected.scale));
    QCOMPARE(qFuzzyIsNull(actual.scale), qFuzzyIsNull(expected.scale));
    if ((qIsFinite(actual.scale)) && (!qFuzzyIsNull(actual.scale))) {
        QCOMPARE(actual.scale, expected.scale);
    }
    #else
    QCOMPARE(actual.scale,              expected.scale);
    #endif
    QCOMPARE(actual.mode,               expected.mode);
    QCOMPARE(actual.range,              expected.range);
    QCOMPARE(actual.samplingWindow,     expected.samplingWindow);
    QCOMPARE(actual.numberOfSamples,    expected.numberOfSamples);
    QCOMPARE(actual.samplingRate,       expected.samplingRate);
}

void TestDsoService::parseSamples_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<DsoService::Samples>("expected");

    QTest::addRow("empty") << QByteArray() << DsoService::Samples();

    // Sample from a real Pokit Pro device.
    QTest::addRow("PokitPro")
        << QByteArray("\xda\x36\x91\x24\x24\x09\x02\x80\x91\x64"
                      "\x48\x12\x23\x49\x01\xc0\x4a\x92\xdc\xf6", 20)
        << DsoService::Samples({14042,9361,2340,-32766,25745,4680,18723,-16383,-28086,-2340});

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
