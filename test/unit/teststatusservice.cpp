// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "teststatusservice.h"

#include <qtpokit/statusservice.h>
#include "statusservice_p.h"

#include <QRegularExpression>

Q_DECLARE_METATYPE(StatusService::DeviceCharacteristics)
Q_DECLARE_METATYPE(StatusService::DeviceStatus)
Q_DECLARE_METATYPE(StatusService::BatteryStatus)
Q_DECLARE_METATYPE(StatusService::Status)

// Serialiser for QCOMPARE to output QBluetoothAddress objects on test failures.
char *toString(const QBluetoothAddress &address)
{
    return qstrdup(("QBluetoothAddress(" + address.toString().toLocal8Bit() + ")").constData());
}

void TestStatusService::toString_DeviceStatus_data()
{
    QTest::addColumn<StatusService::DeviceStatus>("status");
    QTest::addColumn<QString>("expected");
    #define DOKIT_ADD_TEST_ROW(status, expected) \
        QTest::addRow(#status) << StatusService::DeviceStatus::status << QStringLiteral(expected)
    DOKIT_ADD_TEST_ROW(Idle,                  "Idle");
    DOKIT_ADD_TEST_ROW(MultimeterDcVoltage,   "MultimeterDcVoltage");
    DOKIT_ADD_TEST_ROW(MultimeterAcVoltage,   "MultimeterAcVoltage");
    DOKIT_ADD_TEST_ROW(MultimeterDcCurrent,   "MultimeterDcCurrent");
    DOKIT_ADD_TEST_ROW(MultimeterAcCurrent,   "MultimeterAcCurrent");
    DOKIT_ADD_TEST_ROW(MultimeterResistance,  "MultimeterResistance");
    DOKIT_ADD_TEST_ROW(MultimeterDiode,       "MultimeterDiode");
    DOKIT_ADD_TEST_ROW(MultimeterContinuity,  "MultimeterContinuity");
    DOKIT_ADD_TEST_ROW(MultimeterTemperature, "MultimeterTemperature");
    DOKIT_ADD_TEST_ROW(DsoModeSampling,       "DsoModeSampling");
    DOKIT_ADD_TEST_ROW(LoggerModeSampling,    "LoggerModeSampling");
    #undef DOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (StatusService::DeviceStatus)11   << QString();
    QTest::addRow("max")     << (StatusService::DeviceStatus)0xFF << QString();
}

void TestStatusService::toString_DeviceStatus()
{
    QFETCH(StatusService::DeviceStatus, status);
    QFETCH(QString, expected);
    QCOMPARE(StatusService::toString(status), expected);
}

void TestStatusService::toString_BatteryStatus_data()
{
    QTest::addColumn<StatusService::BatteryStatus>("status");
    QTest::addColumn<QString>("expected");
    #define DOKIT_ADD_TEST_ROW(status, expected) \
        QTest::addRow(#status) << StatusService::BatteryStatus::status << QStringLiteral(expected)
    DOKIT_ADD_TEST_ROW(Low,  "Low");
    DOKIT_ADD_TEST_ROW(Good, "Good");
    #undef DOKIT_ADD_TEST_ROW
    QTest::addRow("invalid") << (StatusService::BatteryStatus)2    << QString();
    QTest::addRow("max")     << (StatusService::BatteryStatus)0xFF << QString();
}

void TestStatusService::toString_BatteryStatus()
{
    QFETCH(StatusService::BatteryStatus, status);
    QFETCH(QString, expected);
    QCOMPARE(StatusService::toString(status), expected);
}

void TestStatusService::readCharacteristics()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    StatusService service(nullptr);
    QVERIFY(!service.readCharacteristics());
}

void TestStatusService::readDeviceCharacteristics()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    StatusService service(nullptr);
    QVERIFY(!service.readDeviceCharacteristics());
}

void TestStatusService::readStatusCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    StatusService service(nullptr);
    QVERIFY(!service.readStatusCharacteristic());
}

void TestStatusService::readNameCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    StatusService service(nullptr);
    QVERIFY(!service.readNameCharacteristic());
}

void TestStatusService::deviceCharacteristics()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const StatusService service(nullptr);
    QVERIFY(service.deviceCharacteristics().firmwareVersion.isNull());
}

void TestStatusService::status()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const StatusService service(nullptr);
    QVERIFY(qIsNaN(service.status().batteryVoltage));
}

void TestStatusService::deviceName()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const StatusService service(nullptr);
    QVERIFY(service.deviceName().isNull());
}

void TestStatusService::setDeviceName()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    StatusService service(nullptr);
    QVERIFY(!service.setDeviceName(QStringLiteral("ignored")));
}

void TestStatusService::flashLed()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    StatusService service(nullptr);
    QVERIFY(!service.flashLed());
}

void TestStatusService::parseDeviceCharacteristics_data()
{
    QTest::addColumn<QByteArray>("value");
    QTest::addColumn<StatusService::DeviceCharacteristics>("expected");

    QTest::addRow("null") << QByteArray()
        << StatusService::DeviceCharacteristics{
            QVersionNumber(), 0, 0, 0, 0, 0, 0, QBluetoothAddress()
        };

    // Device Characteristics must be at least 20 bytes to be valid / parsable.
    QTest::addRow("too-small") << QByteArray(19, '\xFF')
        << StatusService::DeviceCharacteristics{
           QVersionNumber(), 0, 0, 0, 0, 0, 0, QBluetoothAddress()
        };

    // Sample from a real Pokit Meter device.
    QTest::addRow("PokitMeter")
        << QByteArray("\x01\x04\x3c\x00\x02\x00\xe8\x03\xe8\x03"
                      "\x00\x20\x00\x00\x84\x2e\x14\x2c\x03\xa8", 20)
        << StatusService::DeviceCharacteristics{
           QVersionNumber(1,4), 60, 2, 1000, 1000, 8192, 0,
           QBluetoothAddress(QStringLiteral("84:2E:14:2C:03:A8"))
        };

    // Sample from a real Pokit Pro device.
    QTest::addRow("PokitPro")
        << QByteArray("\x01\x03\x52\x03\x0a\x00\xb8\x0b\xe8\x03"
                      "\x00\x40\x00\x00\x5c\x02\x72\x09\xaa\x25", 20)
        << StatusService::DeviceCharacteristics{
           QVersionNumber(1,3), 850, 10, 3000, 1000, 16384, 0,
           QBluetoothAddress(QStringLiteral("5C:02:72:09:AA:25"))
        };
}

void TestStatusService::parseDeviceCharacteristics()
{
    QFETCH(QByteArray, value);
    QFETCH(StatusService::DeviceCharacteristics, expected);
    if (value.size() < 20) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            R"(^Device Characterisitcs requires \d+ byte/s, but only \d+ present: 0x[a-zA-Z0-9,]*$)")));
    }
    if (value.size() > 20) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            R"(^Device Characterisitcs has \d+ extraneous byte/s: 0x[a-zA-Z0-9,]*$)")));
    }
    const StatusService::DeviceCharacteristics actual =
        StatusServicePrivate::parseDeviceCharacteristics(value);
    QCOMPARE(actual.firmwareVersion,     expected.firmwareVersion);
    QCOMPARE(actual.maximumVoltage,      expected.maximumVoltage);
    QCOMPARE(actual.maximumCurrent,      expected.maximumCurrent);
    QCOMPARE(actual.maximumResistance,   expected.maximumResistance);
    QCOMPARE(actual.maximumSamplingRate, expected.maximumSamplingRate);
    QCOMPARE(actual.samplingBufferSize,  expected.samplingBufferSize);
    QCOMPARE(actual.capabilityMask,      expected.capabilityMask);
    QCOMPARE(actual.macAddress,          expected.macAddress);
}

void TestStatusService::parseStatus_data()
{
    QTest::addColumn<QByteArray>("value");
    QTest::addColumn<StatusService::Status>("expected");

    QTest::addRow("null") << QByteArray()
        << StatusService::Status{
           static_cast<StatusService::DeviceStatus>
               (std::numeric_limits<std::underlying_type_t<StatusService::DeviceStatus>>::max()),
           std::numeric_limits<float>::quiet_NaN(),
           static_cast<StatusService::BatteryStatus>
               (std::numeric_limits<std::underlying_type_t<StatusService::BatteryStatus>>::max()),
        };

    // Status must be at least 5 bytes to be valid / parsable.
    QTest::addRow("too-small") << QByteArray(4, '\xFF')
    << StatusService::Status{
       static_cast<StatusService::DeviceStatus>
           (std::numeric_limits<std::underlying_type_t<StatusService::DeviceStatus>>::max()),
       std::numeric_limits<float>::quiet_NaN(),
       static_cast<StatusService::BatteryStatus>
           (std::numeric_limits<std::underlying_type_t<StatusService::BatteryStatus>>::max()),
    };


    // Sample from a real Pokit Meter device. Note that the battery status is invalid (std::max)
    // because Pokit Meter devices don't report the battery status (legnth is 5, not 6 bytes).
    QTest::addRow("PokitMeter")
        << QByteArray("\x00\x25\x07\x33\x40", 5)
        << StatusService::Status{
           StatusService::DeviceStatus::Idle, 2.797311068f,
           static_cast<StatusService::BatteryStatus>
               (std::numeric_limits<std::underlying_type_t<StatusService::BatteryStatus>>::max()),
        };

    // Sample from a real Pokit Pro device; note is has 4 extra unknown bytes. Note, this has 2 more
    // bytes than the longest documented format (6 bytes).
    QTest::addRow("PokitPro")
        << QByteArray("\x02\x64\x3b\x83\x40\x01\x00\x00", 8)
        << StatusService::Status{
           StatusService::DeviceStatus::MultimeterAcVoltage, 4.100999832f,
           StatusService::BatteryStatus::Good
        };
}

void TestStatusService::parseStatus()
{
    QFETCH(QByteArray, value);
    QFETCH(StatusService::Status, expected);
    if (value.size() < 5) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            R"(^Status requires \d+ byte/s, but only \d+ present: 0x[a-zA-Z0-9,]*$)")));
    }
    if (value.size() > 6) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            R"(^Status has \d+ extraneous byte/s: 0x[a-zA-Z0-9,]*$)")));
    }
    const StatusService::Status actual = StatusServicePrivate::parseStatus(value);
    QCOMPARE(actual.deviceStatus,   expected.deviceStatus);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    QCOMPARE(qIsFinite(actual.batteryVoltage),    qIsFinite(expected.batteryVoltage));
    QCOMPARE(qIsInf(actual.batteryVoltage),       qIsInf(expected.batteryVoltage));
    QCOMPARE(qIsNaN(actual.batteryVoltage),       qIsNaN(expected.batteryVoltage));
    QCOMPARE(qFuzzyIsNull(actual.batteryVoltage), qFuzzyIsNull(expected.batteryVoltage));
    if ((qIsFinite(actual.batteryVoltage)) && (!qFuzzyIsNull(actual.batteryVoltage))) {
        QCOMPARE(actual.batteryVoltage, expected.batteryVoltage);
    }
    #else
    QCOMPARE(actual.batteryVoltage, expected.batteryVoltage);
    #endif
    QCOMPARE(actual.batteryStatus,  expected.batteryStatus);
}

void TestStatusService::serviceDiscovered()
{
    // Service UUID begins as null.
    StatusService service(nullptr);
    QVERIFY(service.d_func()->serviceUuid.isNull());

    // Random service UUIDs won't affect the object's service UUID.
    service.d_func()->serviceDiscovered(QBluetoothUuid::createUuid());
    QVERIFY(service.d_func()->serviceUuid.isNull());

    // Pokit Meter's Status service UUId will be assigned.
    service.d_func()->serviceDiscovered(StatusService::ServiceUuids::pokitMeter);
    QCOMPARE(service.d_func()->serviceUuid, StatusService::ServiceUuids::pokitMeter);

    // Pokit Pro's Status service UUId will be assigned.
    service.d_func()->serviceDiscovered(StatusService::ServiceUuids::pokitPro);
    QCOMPARE(service.d_func()->serviceUuid, StatusService::ServiceUuids::pokitPro);

    // Random service UUIDs still won't affect the object's service UUID.
    service.d_func()->serviceDiscovered(QBluetoothUuid::createUuid());
    QCOMPARE(service.d_func()->serviceUuid, StatusService::ServiceUuids::pokitPro); // ie unchanged.
}

void TestStatusService::characteristicRead()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    StatusService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic read for Status service .*$")));
    service.d_func()->characteristicRead(QLowEnergyCharacteristic(), QByteArray());
}

void TestStatusService::characteristicWritten()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    StatusService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic written for Status service .*$")));
    service.d_func()->characteristicWritten(QLowEnergyCharacteristic(), QByteArray());
}

void TestStatusService::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    StatusService service(nullptr);
    QVERIFY(!service.tr("ignored").isEmpty());
}

QTEST_MAIN(TestStatusService)
