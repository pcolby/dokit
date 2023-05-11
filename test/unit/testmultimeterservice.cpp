// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testmultimeterservice.h"

#include <qtpokit/multimeterservice.h>
#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>
#include "multimeterservice_p.h"

#include <QRegularExpression>

Q_DECLARE_METATYPE(MultimeterService::Mode)
Q_DECLARE_METATYPE(MultimeterService::Settings)
Q_DECLARE_METATYPE(MultimeterService::Reading)

void TestMultimeterService::toString_Mode_data()
{
    QTest::addColumn<MultimeterService::Mode>("mode");
    QTest::addColumn<QString>("expected");
    #define DOKIT_ADD_TEST_ROW(mode, expected) \
        QTest::addRow(#mode) << MultimeterService::Mode::mode << QStringLiteral(expected)
    DOKIT_ADD_TEST_ROW(Idle,        "Idle");
    DOKIT_ADD_TEST_ROW(DcVoltage,   "DC voltage");
    DOKIT_ADD_TEST_ROW(AcVoltage,   "AC voltage");
    DOKIT_ADD_TEST_ROW(DcCurrent,   "DC current");
    DOKIT_ADD_TEST_ROW(AcCurrent,   "AC current");
    DOKIT_ADD_TEST_ROW(Resistance,  "Resistance");
    DOKIT_ADD_TEST_ROW(Diode,       "Diode");
    DOKIT_ADD_TEST_ROW(Continuity,  "Continuity");
    DOKIT_ADD_TEST_ROW(Temperature, "Temperature");
    DOKIT_ADD_TEST_ROW(Capacitance, "Capacitance");
    DOKIT_ADD_TEST_ROW(ExternalTemperature, "External temperature");
    #undef DOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (MultimeterService::Mode)11   << QString();
    QTest::addRow("max")     << (MultimeterService::Mode)0xFF << QString();
}

void TestMultimeterService::toString_Mode()
{
    QFETCH(MultimeterService::Mode, mode);
    QFETCH(QString, expected);
    QCOMPARE(MultimeterService::toString(mode), expected);
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
           MultimeterService::Mode::Idle, +PokitMeter::VoltageRange::_300mV, 0
        }
        << QByteArray("\x00\x00\x00\x00\x00\x00", 6);

    QTest::addRow("AcVoltage")
        << MultimeterService::Settings{
           MultimeterService::Mode::AcVoltage, +PokitMeter::VoltageRange::_6V,
           1000
        }
        << QByteArray("\x02\x02\xE8\x03\x00\x00", 6);

    QTest::addRow("Diode")
        << MultimeterService::Settings{
           MultimeterService::Mode::Diode, +PokitMeter::VoltageRange::AutoRange,
           5000
        }
        << QByteArray("\x06\xFF\x88\x13\x00\x00", 6);

    QTest::addRow("Resistance")
        << MultimeterService::Settings{
           MultimeterService::Mode::Resistance, +PokitMeter::VoltageRange::AutoRange,
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
           MultimeterService::Mode::Idle, 0
        };

    // Metadata must be at least 7 bytes to be valid / parsable.
    QTest::addRow("too-small") << QByteArray(6, '\xFF')
        << MultimeterService::Reading{
           MultimeterService::MeterStatus::Error, std::numeric_limits<float>::quiet_NaN(),
           MultimeterService::Mode::Idle, 0
        };

    // Sample from a real Pokit Meter device.
    QTest::addRow("PokitMeter")
        << QByteArray("\x00\x00\x00\x00\x00\x01\x03", 7)
        << MultimeterService::Reading{
           MultimeterService::MeterStatus::AutoRangeOff, 0.0f,
           MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::_12V
        };

    // Sample from a real Pokit Pro device.
    QTest::addRow("PokitPro")
        << QByteArray("\x00\x94\x89\xfa\x3b\x02\x00", 7)
        << MultimeterService::Reading{
           MultimeterService::MeterStatus::AutoRangeOff, 0.007645795122f,
           MultimeterService::Mode::AcVoltage, +PokitMeter::VoltageRange::_300mV
        };

    // Made-up sample *extended* from a real Pokit Pro device (by appending 3 erroneous bytes).
    QTest::addRow("PokitPro")
        << QByteArray("\x00\x94\x89\xfa\x3b\x02\x00\x01\x02\x03", 10)
        << MultimeterService::Reading{
           MultimeterService::MeterStatus::AutoRangeOff, 0.007645795122f,
           MultimeterService::Mode::AcVoltage, +PokitMeter::VoltageRange::_300mV
        };
}

void TestMultimeterService::parseReading()
{
    QFETCH(QByteArray, value);
    QFETCH(MultimeterService::Reading, expected);
    if (value.size() < 7) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Reading requires \\d+ byte/s, but only \\d+ present: 0x[a-zA-Z0-9,]*$")));
    }
    if (value.size() > 7) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Reading has \\d+ extraneous byte/s: 0x[a-zA-Z0-9,]*$")));
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
    QCOMPARE(actual.range, expected.range);
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

void TestMultimeterService::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    MultimeterService service(nullptr);
    QVERIFY(!service.tr("ignored").isEmpty());
}

QTEST_MAIN(TestMultimeterService)
