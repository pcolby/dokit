// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testgenericaccessservice.h"

#include <qtpokit/genericaccessservice.h>
#include "genericaccessservice_p.h"

#include <QRegularExpression>

void TestGenericAccessService::readCharacteristics()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    GenericAccessService service(nullptr);
    QVERIFY(!service.readCharacteristics());
}

void TestGenericAccessService::readAppearanceCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    GenericAccessService service(nullptr);
    QVERIFY(!service.readAppearanceCharacteristic());
}

void TestGenericAccessService::readDeviceNameCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    GenericAccessService service(nullptr);
    QVERIFY(!service.readDeviceNameCharacteristic());
}

void TestGenericAccessService::appearance()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const GenericAccessService service(nullptr);
    QCOMPARE(service.appearance(), std::numeric_limits<quint16>::max());
}

void TestGenericAccessService::deviceName()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const GenericAccessService service(nullptr);
    QVERIFY(service.deviceName().isNull());
}

void TestGenericAccessService::setDeviceName()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    GenericAccessService service(nullptr);
    QVERIFY(!service.setDeviceName(QStringLiteral("ignored")));
}

void TestGenericAccessService::parseAppearance_data()
{
    /// \todo
}

void TestGenericAccessService::parseAppearance()
{
    /// \todo
}

void TestGenericAccessService::characteristicRead()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    GenericAccessService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic read for Generic Access service .*$")));
    service.d_func()->characteristicRead(QLowEnergyCharacteristic(), QByteArray());
}

void TestGenericAccessService::characteristicWritten()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    GenericAccessService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic written for Generic Access service .*$")));
    service.d_func()->characteristicWritten(QLowEnergyCharacteristic(), QByteArray());
}

QTEST_MAIN(TestGenericAccessService)
