// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testcalibrationservice.h"
#include "../github.h"

#include <qtpokit/calibrationservice.h>
#include "calibrationservice_p.h"

#include <QLowEnergyController>
#include <QRegularExpression>

QTPOKIT_BEGIN_NAMESPACE

void TestCalibrationService::readCharacteristics()
{
    if (gitHubActionsRunnerOsVersion() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 14)) {
        QSKIP("BLE controller operations hang on GitHub Actions's macOS 14 runners");
    }

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

void TestCalibrationService::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    CalibrationService service(nullptr);
    QVERIFY(!service.tr("ignored").isEmpty());
}

QTPOKIT_END_NAMESPACE

QTEST_MAIN(QTPOKIT_PREPEND_NAMESPACE(TestCalibrationService))
