// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testinfocommand.h"

#include "infocommand.h"

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

void TestInfoCommand::requiredOptions()
{
    InfoCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser);
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestInfoCommand::supportedOptions()
{
    InfoCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser);
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestInfoCommand::processOptions()
{
    InfoCommand command(this);
    QCommandLineParser parser;
    QCOMPARE(command.processOptions(parser), QStringList());
}

void TestInfoCommand::getService()
{
    // Unable to safely invoke InfoCommand::getService() without a valid Bluetooth device.
}

void TestInfoCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke InfoCommand::serviceDetailsDiscovered() without a valid service.
}

void TestInfoCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    InfoCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestInfoCommand)
