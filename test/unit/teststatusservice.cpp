// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "teststatusservice.h"

#include <qtpokit/statusservice.h>
#include "statusservice_p.h"

#include <QRegularExpression>

void TestStatusService::toString_DeviceStatus_data()
{
    /// \todo
}

void TestStatusService::toString_DeviceStatus()
{
    /// \todo
}

void TestStatusService::toString_BatteryStatus_data()
{
    /// \todo
}

void TestStatusService::toString_BatteryStatus()
{
    /// \todo
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
    /// \todo
}

void TestStatusService::parseDeviceCharacteristics()
{
    /// \todo
}

void TestStatusService::parseStatus_data()
{
    /// \todo
}

void TestStatusService::parseStatus()
{
    /// \todo
}

void TestStatusService::serviceDiscovered()
{
    /// \todo
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

QTEST_MAIN(TestStatusService)
