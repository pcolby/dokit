// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdeviceinfoservice.h"
#include "../stringliterals_p.h"

#include <qtpokit/deviceinfoservice.h>
#include "deviceinfoservice_p.h"

#include <QRegularExpression>

QTPOKIT_BEGIN_NAMESPACE
DOKIT_USE_STRINGLITERALS

void TestDeviceInfoService::readCharacteristics()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DeviceInfoService service(nullptr);
    QVERIFY(!service.readCharacteristics());
}

void TestDeviceInfoService::readFirmwareRevisionCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DeviceInfoService service(nullptr);
    QVERIFY(!service.readFirmwareRevisionCharacteristic());
}

void TestDeviceInfoService::readHardwareRevisionCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DeviceInfoService service(nullptr);
    QVERIFY(!service.readHardwareRevisionCharacteristic());
}

void TestDeviceInfoService::readManufacturerCharacteristics()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DeviceInfoService service(nullptr);
    QVERIFY(!service.readManufacturerCharacteristics());
}

void TestDeviceInfoService::readModelNumberCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DeviceInfoService service(nullptr);
    QVERIFY(!service.readModelNumberCharacteristic());
}

void TestDeviceInfoService::readSoftwareRevisionCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DeviceInfoService service(nullptr);
    QVERIFY(!service.readSoftwareRevisionCharacteristic());
}

void TestDeviceInfoService::readSerialNumberCharacteristic()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    DeviceInfoService service(nullptr);
    QVERIFY(!service.readSerialNumberCharacteristic());
}

void TestDeviceInfoService::manufacturer()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const DeviceInfoService service(nullptr);
    QVERIFY(service.manufacturer().isNull());
}

void TestDeviceInfoService::modelNumber()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const DeviceInfoService service(nullptr);
    QVERIFY(service.modelNumber().isNull());
}

void TestDeviceInfoService::hardwareRevision()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const DeviceInfoService service(nullptr);
    QVERIFY(service.hardwareRevision().isNull());
}

void TestDeviceInfoService::firmwareRevision()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const DeviceInfoService service(nullptr);
    QVERIFY(service.firmwareRevision().isNull());
}

void TestDeviceInfoService::softwareRevision()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const DeviceInfoService service(nullptr);
    QVERIFY(service.softwareRevision().isNull());
}

void TestDeviceInfoService::serialNumber()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    const DeviceInfoService service(nullptr);
    QVERIFY(service.serialNumber().isNull());
}

void TestDeviceInfoService::characteristicRead()
{
    // Unfortunately we cannot construct QLowEnergyCharacteristic objects to test signal emissions.
    DeviceInfoService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(u"^Unknown characteristic read for Device Info service .*$"_s));
    service.d_func()->characteristicRead(QLowEnergyCharacteristic(), QByteArray());
}

void TestDeviceInfoService::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    DeviceInfoService service(nullptr);
    QVERIFY(!service.tr("ignored").isEmpty());
}

QTPOKIT_END_NAMESPACE

QTEST_MAIN(QTPOKIT_PREPEND_NAMESPACE(TestDeviceInfoService))
