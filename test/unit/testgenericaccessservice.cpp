// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
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
    QTest::addColumn<QByteArray>("value");
    QTest::addColumn<quint16>("expected");

    QTest::addRow("null") << QByteArray() << std::numeric_limits<quint16>::max();

    QTest::addRow("short") << QByteArray(1, '\x00') << std::numeric_limits<quint16>::max();

    QTest::addRow("0000") << QByteArray("\x00\x00", 2) << (quint16)0;
    QTest::addRow("00FF") << QByteArray("\x00\xFF", 2) << (quint16)65280;
    QTest::addRow("FF00") << QByteArray("\xFF\x00", 2) << (quint16)255;
    QTest::addRow("ABCD") << QByteArray("\xAB\xCD", 2) << (quint16)52651;

    // Extra bytes emit warnings, but are otherwise ignored.
    QTest::addRow("AAAA")  << QByteArray("\xAA\xAA", 2) << (quint16)43690;
    QTest::addRow("extra") << QByteArray(10, '\xAA')    << (quint16)43690;
}

void TestGenericAccessService::parseAppearance()
{
    QFETCH(QByteArray, value);
    QFETCH(quint16, expected);
    if (value.size() < 2) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Appearance requires \\d+ bytes, but only \\d+ present: 0x[a-zA-Z0-9,]*$")));
    }
    if (value.size() > 2) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^Appearance has \\d+ extraneous bytes: 0x[a-zA-Z0-9,]*$")));
    }
    QCOMPARE(GenericAccessServicePrivate::parseAppearance(value), expected);
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

void TestGenericAccessService::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    GenericAccessService service(nullptr);
    QVERIFY(!service.tr("ignored").isEmpty());
}

QTEST_MAIN(TestGenericAccessService)
