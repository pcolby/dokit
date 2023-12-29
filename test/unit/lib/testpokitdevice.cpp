// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testpokitdevice.h"

#include <qtpokit/pokitdevice.h>
#include "pokitdevice_p.h"

#include <qtpokit/calibrationservice.h>
#include <qtpokit/dataloggerservice.h>
#include <qtpokit/deviceinfoservice.h>
#include <qtpokit/dsoservice.h>
#include <qtpokit/multimeterservice.h>
#include <qtpokit/statusservice.h>

void TestPokitDevice::controller()
{
    PokitDevice device(nullptr);
    QCOMPARE(device.controller(), nullptr);
}

void TestPokitDevice::controller_const()
{
    const PokitDevice device(nullptr);
    QCOMPARE(device.controller(), nullptr);
}

void TestPokitDevice::calibration()
{
    PokitDevice device(nullptr);
    const CalibrationService * service = device.calibration();
    QVERIFY(service != nullptr);
    QCOMPARE(device.calibration(), service); // Once created, the PokitDevice instance will always
    QCOMPARE(device.calibration(), service); // return the same service class instances (in a thread
    QCOMPARE(device.calibration(), service); // safe manner, too).
}

void TestPokitDevice::dataLogger()
{
    PokitDevice device(nullptr);
    const DataLoggerService * service = device.dataLogger();
    QVERIFY(service != nullptr);
    QCOMPARE(device.dataLogger(), service); // Once created, the PokitDevice instance will always
    QCOMPARE(device.dataLogger(), service); // return the same service class instances (in a thread
    QCOMPARE(device.dataLogger(), service); // safe manner, too).
}

void TestPokitDevice::deviceInformation()
{
    PokitDevice device(nullptr);
    const DeviceInfoService * service = device.deviceInformation();
    QVERIFY(service != nullptr);
    QCOMPARE(device.deviceInformation(), service); // Once created, the PokitDevice instance will always
    QCOMPARE(device.deviceInformation(), service); // return the same service class instances (in a thread
    QCOMPARE(device.deviceInformation(), service); // safe manner, too).
}

void TestPokitDevice::dso()
{
    PokitDevice device(nullptr);
    const DsoService * service = device.dso();
    QVERIFY(service != nullptr);
    QCOMPARE(device.dso(), service); // Once created, the PokitDevice instance will always
    QCOMPARE(device.dso(), service); // return the same service class instances (in a thread
    QCOMPARE(device.dso(), service); // safe manner, too).
}

void TestPokitDevice::multimeter()
{
    PokitDevice device(nullptr);
    const MultimeterService * service = device.multimeter();
    QVERIFY(service != nullptr);
    QCOMPARE(device.multimeter(), service); // Once created, the PokitDevice instance will always
    QCOMPARE(device.multimeter(), service); // return the same service class instances (in a thread
    QCOMPARE(device.multimeter(), service); // safe manner, too).
}

void TestPokitDevice::status()
{
    PokitDevice device(nullptr);
    const StatusService * service = device.status();
    QVERIFY(service != nullptr);
    QCOMPARE(device.status(), service); // Once created, the PokitDevice instance will always
    QCOMPARE(device.status(), service); // return the same service class instances (in a thread
    QCOMPARE(device.status(), service); // safe manner, too).
}

void TestPokitDevice::serviceToString_data()
{
    QTest::addColumn<QBluetoothUuid>("uuid");
    QTest::addColumn<QString>("expected");

    #define DOKIT_ADD_TEST_ROW(service, expected) \
        QTest::addRow(#service) << service::serviceUuid << QStringLiteral(expected)
    DOKIT_ADD_TEST_ROW(CalibrationService, "Calibration");
    DOKIT_ADD_TEST_ROW(DataLoggerService,  "Data Logger");
    DOKIT_ADD_TEST_ROW(DsoService,         "DSO");
    DOKIT_ADD_TEST_ROW(MultimeterService,  "Multimeter");
    #undef DOKIT_ADD_TEST_ROW

    QTest::addRow("Status (Pokit Meter)")
        << StatusService::ServiceUuids::pokitMeter << "Status (Pokit Meter)";
    QTest::addRow("Status (Pokit Pro)")
        << StatusService::ServiceUuids::pokitPro << "Status (Pokit Pro)";

    QTest::addRow("DeviceInfoService") << DeviceInfoService::serviceUuid
        << QBluetoothUuid::serviceClassToString(QBluetoothUuid::ServiceClassUuid::DeviceInformation);

    QTest::addRow("GenericAccessService") << QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::GenericAccess)
        << QBluetoothUuid::serviceClassToString(QBluetoothUuid::ServiceClassUuid::GenericAccess);

    QTest::addRow("GenericAttribute") << QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::GenericAttribute)
        << QBluetoothUuid::serviceClassToString(QBluetoothUuid::ServiceClassUuid::GenericAttribute);

    // Bonus, unsupported service (we're just aware of it).
    QTest::addRow("OTA Service") << QBluetoothUuid(QStringLiteral("1d14d6ee-fd63-4fa1-bfa4-8f47b42119f0"))
        << QStringLiteral("OTA Firmware Update");
}

void TestPokitDevice::serviceToString()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(QString, expected);
    QCOMPARE(PokitDevice::serviceToString(uuid), expected);
}

void TestPokitDevice::charcteristicToString_data()
{
    QTest::addColumn<QBluetoothUuid>("uuid");
    QTest::addColumn<QString>("expected");

    #define DOKIT_ADD_TEST_ROW(service, characteristic, expected) \
        QTest::addRow(#service "::" #characteristic) \
            << service::CharacteristicUuids::characteristic << QStringLiteral(expected)
    DOKIT_ADD_TEST_ROW(CalibrationService, temperature,           "Temperature");
    DOKIT_ADD_TEST_ROW(CalibrationService, getParam,              "Get Param");
    DOKIT_ADD_TEST_ROW(CalibrationService, setParam,              "Set Param");
    DOKIT_ADD_TEST_ROW(DataLoggerService,  metadata,              "Metadata");
    DOKIT_ADD_TEST_ROW(DataLoggerService,  reading,               "Reading");
    DOKIT_ADD_TEST_ROW(DataLoggerService,  settings,              "Settings");
    DOKIT_ADD_TEST_ROW(DsoService,         metadata,              "Metadata");
    DOKIT_ADD_TEST_ROW(DsoService,         reading,               "Reading");
    DOKIT_ADD_TEST_ROW(DsoService,         settings,              "Settings");
    DOKIT_ADD_TEST_ROW(MultimeterService,  reading,               "Reading");
    DOKIT_ADD_TEST_ROW(MultimeterService,  settings,              "Settings");
    DOKIT_ADD_TEST_ROW(StatusService,      deviceCharacteristics, "Device Characteristics");
    DOKIT_ADD_TEST_ROW(StatusService,      flashLed,              "Flash LED");
    DOKIT_ADD_TEST_ROW(StatusService,      name,                  "Name");
    DOKIT_ADD_TEST_ROW(StatusService,      status,                "Status");
    #undef DOKIT_ADD_TEST_ROW

    #define DOKIT_ADD_TEST_ROW(service, characteristic, expected) \
        QTest::addRow(#service "::" #characteristic) \
            << service::CharacteristicUuids::characteristic \
            << QBluetoothUuid::characteristicToString(QBluetoothUuid::CharacteristicType::expected)
    DOKIT_ADD_TEST_ROW(DeviceInfoService,    firmwareRevision, FirmwareRevisionString);
    DOKIT_ADD_TEST_ROW(DeviceInfoService,    hardwareRevision, HardwareRevisionString);
    DOKIT_ADD_TEST_ROW(DeviceInfoService,    manufacturerName, ManufacturerNameString);
    DOKIT_ADD_TEST_ROW(DeviceInfoService,    modelNumber,      ModelNumberString);
    DOKIT_ADD_TEST_ROW(DeviceInfoService,    softwareRevision, SoftwareRevisionString);
    #undef DOKIT_ADD_TEST_ROW

    #define DOKIT_ADD_TEST_ROW(uuid, expected) \
        QTest::addRow(expected) << QBluetoothUuid(QStringLiteral(uuid)) << QStringLiteral(expected)
    DOKIT_ADD_TEST_ROW("f7bf3564-fb6d-4e53-88a4-5e37e0326063", "OTA Control");
    DOKIT_ADD_TEST_ROW("984227f3-34fc-4045-a5d0-2c581f81a153", "OTA Data Transfer");
    #undef DOKIT_ADD_TEST_ROW
}

void TestPokitDevice::charcteristicToString()
{
    QFETCH(QBluetoothUuid, uuid);
    QFETCH(QString, expected);
    QCOMPARE(PokitDevice::charcteristicToString(uuid), expected);
}

void TestPokitDevice::setController()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDevice device(nullptr);
    QCOMPARE(device.controller(), nullptr);

    QLowEnergyController * const controller1 =
        QLowEnergyController::createCentral(QBluetoothDeviceInfo());
    device.d_func()->setController(controller1);
    QCOMPARE(device.controller(), controller1);

    QLowEnergyController * const controller2 =
        QLowEnergyController::createCentral(QBluetoothDeviceInfo());
    device.d_func()->setController(controller2);
    QCOMPARE(device.controller(), controller2);

    // Safe to set the same controller.
    device.d_func()->setController(controller2);
    QCOMPARE(device.controller(), controller2);
    device.d_func()->setController(controller2);
    QCOMPARE(device.controller(), controller2);

    // Safe to "unset" the controller.
    device.d_func()->setController(nullptr);
    QCOMPARE(device.controller(), nullptr);

    // Safe to clean-up now.
    delete controller1;
    delete controller2;
}

void TestPokitDevice::connected()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDevice device(nullptr);
    QTest::ignoreMessage(QtCriticalMsg,
        "PokitDevicePrivate::connected slot invoked without a controller.");
    device.d_func()->connected();

    QLowEnergyController * const tempController =
        QLowEnergyController::createCentral(QBluetoothDeviceInfo());
    device.d_func()->setController(tempController);
    device.d_func()->connected();

    device.d_func()->setController(nullptr);
    QTest::ignoreMessage(QtCriticalMsg,
        "PokitDevicePrivate::connected slot invoked without a controller.");
    device.d_func()->connected();

    delete tempController;
}

void TestPokitDevice::connectionUpdated()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDevice device(nullptr);
    device.d_func()->connectionUpdated(QLowEnergyConnectionParameters());
}

void TestPokitDevice::disconnected()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDevice device(nullptr);
    device.d_func()->disconnected();
}

void TestPokitDevice::discoveryFinished()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDevice device(nullptr);
    device.d_func()->discoveryFinished();
}

void TestPokitDevice::errorOccurred()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDevice device(nullptr);
    device.d_func()->errorOccurred(QLowEnergyController::Error::UnknownError);
}

void TestPokitDevice::serviceDiscovered()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDevice device(nullptr);
    device.d_func()->serviceDiscovered(QBluetoothUuid::createUuid());
}

void TestPokitDevice::stateChanged()
{
    // Verify safe error handling (can't do much else without a Bluetooth device).
    PokitDevice device(nullptr);
    device.d_func()->stateChanged(QLowEnergyController::ControllerState::ClosingState);
}

void TestPokitDevice::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    PokitDevice device(nullptr);
    QVERIFY(!device.tr("ignored").isEmpty());
    QVERIFY(!device.d_ptr->tr("ignored").isEmpty());
}

QTEST_MAIN(TestPokitDevice)
