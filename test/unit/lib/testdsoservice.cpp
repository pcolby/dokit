// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdsoservice.h"

#include <qtpokit/dsoservice.h>
#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>
#include "dsoservice_p.h"

#include <QRegularExpression>

Q_DECLARE_METATYPE(QTPOKIT_PREPEND_NAMESPACE(DsoService::Mode))
Q_DECLARE_METATYPE(QTPOKIT_PREPEND_NAMESPACE(DsoService::Settings))
Q_DECLARE_METATYPE(QTPOKIT_PREPEND_NAMESPACE(DsoService::Metadata))
Q_DECLARE_METATYPE(QTPOKIT_PREPEND_NAMESPACE(PokitProduct))

QTPOKIT_BEGIN_NAMESPACE

void TestDsoService::toString_Mode_data()
{
    QTest::addColumn<DsoService::Mode>("mode");
    QTest::addColumn<QString>("expected");
    #define DOKIT_ADD_TEST_ROW(mode, expected) \
        QTest::addRow(#mode) << DsoService::Mode::mode << QStringLiteral(expected)
    DOKIT_ADD_TEST_ROW(Idle,      "Idle");
    DOKIT_ADD_TEST_ROW(DcVoltage, "DC voltage");
    DOKIT_ADD_TEST_ROW(AcVoltage, "AC voltage");
    DOKIT_ADD_TEST_ROW(DcCurrent, "DC current");
    DOKIT_ADD_TEST_ROW(AcCurrent, "AC current");
    #undef DOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (DsoService::Mode)5    << QString();
    QTest::addRow("max")     << (DsoService::Mode)0xFF << QString();
}

void TestDsoService::toString_Mode()
{
    QFETCH(DsoService::Mode, mode);
    QFETCH(QString, expected);
    QCOMPARE(DsoService::toString(mode), expected);
}

void TestDsoService::toString_Range_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<DsoService::Mode>("mode");
    QTest::addColumn<QString>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::toString_* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Idle") << PokitProduct::PokitMeter << +PokitMeter::CurrentRange::_150mA
                          << DsoService::Mode::Idle << QString();
    QTest::addRow("Voltage:Meter") << PokitProduct::PokitMeter << +PokitMeter::VoltageRange::_300mV
                                   << DsoService::Mode::AcVoltage << QStringLiteral("Up to 300mV");
    QTest::addRow("Voltage:Pro") << PokitProduct::PokitPro << +PokitPro::VoltageRange::_600V
                                 << DsoService::Mode::AcVoltage << QString::fromUtf8("Up to 600V");
    QTest::addRow("Current:Meter") << PokitProduct::PokitMeter << +PokitMeter::CurrentRange::_150mA
                                   << DsoService::Mode::DcCurrent << QStringLiteral("Up to 150mA");
    QTest::addRow("Current:Pro") << PokitProduct::PokitPro << +PokitPro::CurrentRange::_500uA
                                 << DsoService::Mode::DcCurrent << QString::fromUtf8("Up to 500μA");
}

void TestDsoService::toString_Range()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(DsoService::Mode, mode);
    QFETCH(QString, expected);

    // Test the static version.
    QCOMPARE(DsoService::toString(product, range, mode), expected);

    // Test the instance version.
    DsoService service(nullptr);
    service.setPokitProduct(product);
    QCOMPARE(service.toString(range, mode), expected);
}

void TestDsoService::maxValue_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint8>("range");
    QTest::addColumn<DsoService::Mode>("mode");
    QTest::addColumn<quint32>("expected");

    // We don't need to test exhaustively here - that's done by TestPokit{Meter,Pro}::maxValue* functions).
    // So here we just need to test that the right product's range is selected.
    QTest::addRow("Idle") << PokitProduct::PokitMeter << +PokitMeter::CurrentRange::_150mA
                          << DsoService::Mode::Idle << (quint32)0;
    QTest::addRow("Voltage:Meter") << PokitProduct::PokitMeter << +PokitMeter::VoltageRange::_300mV
                                   << DsoService::Mode::AcVoltage << (quint32)300;
    QTest::addRow("Voltage:Pro") << PokitProduct::PokitPro << +PokitPro::VoltageRange::_600V
                                 << DsoService::Mode::AcVoltage<< (quint32)600000;
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << +PokitMeter::CurrentRange::_150mA
                                 << DsoService::Mode::DcCurrent << (quint32)150000;
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro << +PokitPro::CurrentRange::_500uA
                               << DsoService::Mode::DcCurrent << (quint32)500;
}

void TestDsoService::maxValue()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint8, range);
    QFETCH(DsoService::Mode, mode);
    QFETCH(quint32, expected);

    // Test the static version.
    QCOMPARE(DsoService::maxValue(product, range, mode), expected);

    // Test the instance version.
    DsoService service(nullptr);
    service.setPokitProduct(product);
    QCOMPARE(service.maxValue(range, mode), expected);
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
           0, 0, 0
        }
        << QByteArray("\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

    // Contrived "start" settings.
    QTest::addRow("zeroed")
        << DsoService::Settings{
           DsoService::Command::FreeRunning,
           0, DsoService::Mode::Idle,
           0, 0, 0
        }
        << QByteArray("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);

    // Realistic "start" settings example.
    QTest::addRow("AcVoltage")
        << DsoService::Settings{
           DsoService::Command::RisingEdgeTrigger,
           1.0f, DsoService::Mode::AcVoltage,
           4, 1000000, 1000
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
           DsoService::Mode::Idle, 0, 0, 0, 0
        };

    // Metadata must be at least 17 bytes to be valid / parsable.
    QTest::addRow("too-small") << QByteArray(16, '\xFF')
        << DsoService::Metadata{
           DsoService::DsoStatus::Error, std::numeric_limits<float>::quiet_NaN(),
           DsoService::Mode::Idle, 0, 0, 0, 0
        };

    // Sample from a real Pokit Meter device.
    QTest::addRow("PokitMeter")
        << QByteArray("\x00\x98\xf7\x8b\x33\x02\x00\x40\x42\x0f\x00\x0a\x00\x0a\x00\x00\x00", 17)
        << DsoService::Metadata{
           DsoService::DsoStatus::Done, 6.517728934e-08f,
           DsoService::Mode::AcVoltage, 0, 1*1000*1000, 10, 10
        };

    // Sample from a real Pokit Pro device; note is has 4 extra unknown bytes.
    QTest::addRow("PokitPro")
        << QByteArray("\x00\xcc\xec\xba\x33\x02\x00\x40\x42\x0f\x00"
                      "\xe8\x03\x0a\x00\x00\x00\x14\x00\x00\x00", 21)
        << DsoService::Metadata{
              DsoService::DsoStatus::Done, 8.704373045e-08f,
              DsoService::Mode::AcVoltage, 0, 1*1000*1000, 1000, 10
        };
}

void TestDsoService::parseMetadata()
{
    QFETCH(QByteArray, value);
    QFETCH(DsoService::Metadata, expected);
    if (value.size() < 17) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            R"(^Metadata requires \d+ byte/s, but only \d+ present: 0x[a-zA-Z0-9,]*$)")));
    }
    if (value.size() > 17) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            R"(^Metadata has \d+ extraneous byte/s: 0x[a-zA-Z0-9,]*$)")));
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
            R"(^Samples value has odd size \d+ \(should be even\): 0x[a-zA-Z0-9,]*$)")));
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

void TestDsoService::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    DsoService service(nullptr);
    QVERIFY(!service.tr("ignored").isEmpty());
}

QTPOKIT_END_NAMESPACE

QTEST_MAIN(QTPOKIT_PREPEND_NAMESPACE(TestDsoService))
