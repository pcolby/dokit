// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
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

// Mock device command class that does nothing (but log) if real devices are actually discovered. That is, if testing
// while real (physical) Pokit devices are withing BLE range. This never happes in CI environments of course.
class StartableDeviceCommand : public MockDeviceCommand
{
protected slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &info) override
    {
        qWarning() << "Ignoring discovered Pokit device:" << info.name() << info.deviceUuid() << info.address();
    }
};

void TestDeviceCommand::start()
{
    StartableDeviceCommand command;

    QVERIFY(command.deviceToScanFor.isNull());
    QTest::ignoreMessage(QtInfoMsg, "Looking for first available Pokit device...");
    QVERIFY(command.start());

    command.deviceToScanFor = QStringLiteral("example");
    QTest::ignoreMessage(QtInfoMsg, R"(Looking for device "example"...)");
    QVERIFY(command.start());
}

void TestDeviceCommand::disconnect()
{
    // Cannot test DeviceCommand::disconnect() without a valid device controller.
}

void TestDeviceCommand::controllerError()
{
    MockDeviceCommand command;

    QTest::ignoreMessage(QtWarningMsg,
    #if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
        "Bluetooth controller error: QLowEnergyController::Error(NetworkError)");
    #else
        "Bluetooth controller error: QLowEnergyController::NetworkError");
    #endif
    command.controllerError(QLowEnergyController::Error::NetworkError);

    QTest::ignoreMessage(QtWarningMsg,
    #if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
         "Bluetooth controller error: QLowEnergyController::Error(UnknownError)");
    #else
         "Bluetooth controller error: QLowEnergyController::UnknownError");
    #endif
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

    QTest::ignoreMessage(QtWarningMsg,
    #if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
         "Bluetooth service error: QLowEnergyService::ServiceError(OperationError)");
    #else
         "Bluetooth service error: QLowEnergyService::OperationError");
    #endif
    command.serviceError(QLowEnergyService::ServiceError::OperationError);

    QTest::ignoreMessage(QtWarningMsg,
    #if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
         "Bluetooth service error: QLowEnergyService::ServiceError(UnknownError)");
    #else
         "Bluetooth service error: QLowEnergyService::UnknownError");
    #endif
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
    QTest::ignoreMessage(QtWarningMsg, R"(Failed to find device "example".)");
    command.deviceDiscoveryFinished();
}

void TestDeviceCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    MockDeviceCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestDeviceCommand)
