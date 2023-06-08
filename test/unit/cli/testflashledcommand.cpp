// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testflashledcommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

#include "flashledcommand.h"

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)

class MockDeviceCommand : public DeviceCommand
{
public:
    explicit MockDeviceCommand(QObject * const parent = nullptr) : DeviceCommand(parent)
    {

    }

    AbstractPokitService * getService() override
    {
        return nullptr;
    }
};

void TestFlashLedCommand::requiredOptions()
{
    FlashLedCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser);
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestFlashLedCommand::supportedOptions()
{
    FlashLedCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser);
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestFlashLedCommand::processOptions()
{
    FlashLedCommand command(this);
    QCommandLineParser parser;
    QCOMPARE(command.processOptions(parser), QStringList());
}

void TestFlashLedCommand::getService()
{
    // Unable to safely invoke FlashLedCommand::getService() without a valid Bluetooth device.
}

void TestFlashLedCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke FlashLedCommand::serviceDetailsDiscovered() without a valid service.
}

void TestFlashLedCommand::deviceLedFlashed_data()
{
    QTest::addColumn<AbstractCommand::OutputFormat>("format");
    QTest::newRow("1.csv") << AbstractCommand::OutputFormat::Csv;
    QTest::newRow("1.json") << AbstractCommand::OutputFormat::Json;
    QTest::newRow("1.txt") << AbstractCommand::OutputFormat::Text;
}

void TestFlashLedCommand::deviceLedFlashed()
{
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    OutputStreamCapture capture(&std::cout);
    FlashLedCommand command;
    command.format = format;
    command.deviceLedFlashed();
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestFlashLedCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    FlashLedCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestFlashLedCommand)
