// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testsettorchcommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

#include "settorchcommand.h"

#include <iostream>
#include <sstream>

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(StatusService::TorchStatus)

// Serialiser for QCOMPARE to output optional TorchStatus values on test failures.
char *toString(const StatusService::TorchStatus &status)
{
    return qstrdup(StatusService::toString(status).toLocal8Bit().constData());
}

class MockDeviceCommand : public DeviceCommand
{
public:
    MockDeviceCommand(QObject * const parent = nullptr) : DeviceCommand(parent)
    {

    }

    AbstractPokitService * getService() override
    {
        return nullptr;
    }
};

void TestSetTorchCommand::requiredOptions()
{
    SetTorchCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) +
        QStringList{ QStringLiteral("mode") };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestSetTorchCommand::supportedOptions()
{
    SetTorchCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser);
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestSetTorchCommand::processOptions_data()
{
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<StatusService::TorchStatus>("expected");
    QTest::addColumn<QStringList>("errors");

    QTest::addRow("on")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("on") }
        << StatusService::TorchStatus::On
        << QStringList{};

    QTest::addRow("off")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("off") }
        << StatusService::TorchStatus::Off
        << QStringList{};

    QTest::addRow("invalid")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("invalid") }
        << StatusService::TorchStatus{}
        << QStringList{ QStringLiteral("Invalid status value: invalid") };

    QTest::addRow("missing")
        << QStringList{}
        << StatusService::TorchStatus{}
        << QStringList{ QStringLiteral("Missing required option: mode") };

    QTest::addRow("empty")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("") }
        << StatusService::TorchStatus{}
        << QStringList{ QStringLiteral("Invalid status value: ") };
}

void TestSetTorchCommand::processOptions()
{
    QFETCH(QStringList, arguments);
    QFETCH(StatusService::TorchStatus, expected);
    QFETCH(QStringList, errors);

    arguments.prepend(QStringLiteral("dokit")); // The first argument is always the app name.

    QCommandLineParser parser;
    parser.addOption({QStringLiteral("mode"), QStringLiteral("description"), QStringLiteral("name")});
    parser.process(arguments);

    SetTorchCommand command(this);
    QCOMPARE(command.processOptions(parser), errors);
    QCOMPARE(command.newStatus, expected);
}

void TestSetTorchCommand::getService()
{
    // Unable to safely invoke SetTorchCommand::getService() without a valid Bluetooth device.
}

void TestSetTorchCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke SetTorchCommand::serviceDetailsDiscovered() without a valid service.
}

void TestSetTorchCommand::torchStatusWritten_data()
{
    QTest::addColumn<AbstractCommand::OutputFormat>("format");
    QTest::newRow("1.csv") << AbstractCommand::OutputFormat::Csv;
    QTest::newRow("1.json") << AbstractCommand::OutputFormat::Json;
    QTest::newRow("1.txt") << AbstractCommand::OutputFormat::Text;
}

void TestSetTorchCommand::torchStatusWritten()
{
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    OutputStreamCapture capture(&std::cout);
    SetTorchCommand command;
    command.format = format;
    command.torchStatusWritten();
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestSetTorchCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    SetTorchCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestSetTorchCommand)
