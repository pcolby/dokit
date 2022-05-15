/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "testcalibrationservice.h"

#include <qtpokit/calibrationservice.h>
#include "calibrationservice_p.h"

#include <QLowEnergyController>

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
    service.d_func()->characteristicWritten(QLowEnergyCharacteristic(), QByteArray());
}

QTEST_MAIN(TestCalibrationService)