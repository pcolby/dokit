// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testsetnamecommand.h"

#include "setnamecommand.h"

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

void TestSetNameCommand::requiredOptions() {
    SetNameCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) +
        QStringList{ QStringLiteral("new-name") };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestSetNameCommand::supportedOptions() {
    SetNameCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser);
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestSetNameCommand::processOptions() {
    /// \todo Implement processOptions test.
}

void TestSetNameCommand::getService() {
    // Unable to safely invoke SetNameCommand::getService() without a valid Bluetooth device.
}

void TestSetNameCommand::serviceDetailsDiscovered() {
    // Unable to safely invoke SetNameCommand::serviceDetailsDiscovered() without a valid service.
}

void TestSetNameCommand::deivceNameWritten() {
    /// \todo Implement deivceNameWritten test.
}

void TestSetNameCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    SetNameCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestSetNameCommand)
