// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdevicecommand.h"

#include "devicecommand.h"

class MockDeviceCommand : public DeviceCommand
{
public:
    MockDeviceCommand() : DeviceCommand(nullptr)
    {

    }

    AbstractPokitService * getService() override
    {
        return nullptr;
    }
};

void TestDeviceCommand::start()
{
    MockDeviceCommand command;

    QVERIFY(command.deviceToScanFor.isNull());
    QTest::ignoreMessage(QtInfoMsg, "Looking first available Pokit device...");
    QVERIFY(command.start());

    command.deviceToScanFor = QStringLiteral("example");
    QTest::ignoreMessage(QtInfoMsg, "Looking for device \"example\"...");
    QVERIFY(command.start());
}

void TestDeviceCommand::disconnect()
{
    // Cannot test DeviceCommand::disconnect() without a valid device controller.
}

void TestDeviceCommand::controllerError()
{
    MockDeviceCommand command;

    QTest::ignoreMessage(QtWarningMsg, "Bluetooth controller error: QLowEnergyController::NetworkError");
    command.controllerError(QLowEnergyController::Error::NetworkError);

    QTest::ignoreMessage(QtWarningMsg, "Bluetooth controller error: QLowEnergyController::UnknownError");
    command.controllerError(QLowEnergyController::Error::UnknownError);
}

void TestDeviceCommand::deviceDisconnected()
{
    MockDeviceCommand command;
    command.deviceDisconnected(); // Just logs a debug message, and exits.
}

void TestDeviceCommand::serviceError()
{
    MockDeviceCommand command;

    QTest::ignoreMessage(QtWarningMsg, "Bluetooth service error: QLowEnergyService::OperationError");
    command.serviceError(QLowEnergyService::ServiceError::OperationError);

    QTest::ignoreMessage(QtWarningMsg, "Bluetooth service error: QLowEnergyService::UnknownError");
    command.serviceError(QLowEnergyService::ServiceError::UnknownError);
}

void TestDeviceCommand::serviceDetailsDiscovered()
{
    MockDeviceCommand command;
    command.serviceDetailsDiscovered(); // Just logs a debug message.
}

void TestDeviceCommand::deviceDiscovered()
{
    MockDeviceCommand command;
    command.deviceToScanFor = QStringLiteral("example");
    command.deviceDiscovered(QBluetoothDeviceInfo()); // Just logs a debug message, and ignores.
}

void TestDeviceCommand::deviceDiscoveryFinished()
{
    MockDeviceCommand command;

    QVERIFY(command.deviceToScanFor.isNull());
    QTest::ignoreMessage(QtWarningMsg, "Failed to find any Pokit device.");
    command.deviceDiscoveryFinished();

    command.deviceToScanFor = QStringLiteral("example");
    QTest::ignoreMessage(QtWarningMsg, "Failed to find device \"example\".");
    command.deviceDiscoveryFinished();
}

QTEST_MAIN(TestDeviceCommand)
