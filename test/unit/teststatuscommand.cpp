// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "teststatuscommand.h"

#include "statuscommand.h"

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

void TestStatusCommand::requiredOptions()
{
    StatusCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser);
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestStatusCommand::supportedOptions()
{
    StatusCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser);
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestStatusCommand::processOptions()
{
    StatusCommand command(this);
    QCommandLineParser parser;
    QCOMPARE(command.processOptions(parser), QStringList());
}

void TestStatusCommand::getService()
{
    // Unable to safely invoke StatusCommand::getService() without a valid Bluetooth device.
}

void TestStatusCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke StatusCommand::serviceDetailsDiscovered() without a valid service.
}

void TestStatusCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    StatusCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestStatusCommand)
