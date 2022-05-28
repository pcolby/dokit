// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testcalibrationservice.h"

#include <qtpokit/calibrationservice.h>
#include "calibrationservice_p.h"

#include <QLowEnergyController>
#include <QRegularExpression>

void TestCalibrationService::readCharacteristics()
{
    // readCharacteristics always returns true, since the service has no readable characterstics.
    CalibrationService service(QLowEnergyController::createCentral(QBluetoothDeviceInfo()));
    QVERIFY(service.readCharacteristics());
}

void TestCalibrationService::calibrateTemperature()
{
    // Verify safe error handling.
    CalibrationService service(nullptr);
    QVERIFY(!service.calibrateTemperature(0.0f));
    QVERIFY(!service.calibrateTemperature(123.4f));
}

void TestCalibrationService::encodeTemperature_data()
{
    QTest::addColumn<float>("value");
    QTest::addColumn<QByteArray>("expected");
    QTest::addRow("-123456.0"  ) << -123456.0f   << QByteArray("\x00\x20\xf1\xc7", 4);
    QTest::addRow(   "-123.456") <<    -123.456f << QByteArray("\x79\xe9\xf6\xc2", 4);
    QTest::addRow(      "0.0"  ) <<       0.0f   << QByteArray("\x00\x00\x00\x00", 4);
    QTest::addRow(    "123.456") <<     123.456f << QByteArray("\x79\xe9\xf6\x42", 4);
    QTest::addRow( "123456.0"  ) <<   12456.0f   << QByteArray("\x00\xa0\x42\x46", 4);
}

void TestCalibrationService::encodeTemperature()
{
    QFETCH(float, value);
    QFETCH(QByteArray, expected);
    QCOMPARE(CalibrationServicePrivate::encodeTemperature(value), expected);
}

void TestCalibrationService::characteristicWritten()
{
    // Verify safe error handling.
    CalibrationService service(nullptr);
    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
        "^Unknown characteristic written for Calibration service .*$")));
    service.d_func()->characteristicWritten(QLowEnergyCharacteristic(), QByteArray());
}

QTEST_MAIN(TestCalibrationService)
