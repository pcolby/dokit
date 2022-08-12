// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testcalibratecommand.h"

#include "calibratecommand.h"

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

void TestCalibrateCommand::requiredOptions()
{
    CalibrateCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    QCOMPARE(command.requiredOptions(parser),
             mock.requiredOptions(parser) + QStringList{ QStringLiteral("temperature") });
}

void TestCalibrateCommand::supportedOptions()
{
    CalibrateCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    QCOMPARE(command.supportedOptions(parser),
             QStringList{ QStringLiteral("temperature") } + mock.supportedOptions(parser));
}

void TestCalibrateCommand::processOptions_data()
{
    QTest::addColumn<QString>("argument");
    QTest::addColumn<float>("expected");
    QTest::addColumn<QStringList>("errors");

    QTest::addRow("missing")
        << QString()
        << std::numeric_limits<float>::quiet_NaN()
        << QStringList{ QStringLiteral("Missing required option: temperature")};

    QTest::addRow("1")   << QStringLiteral("1")   <<   1.0f << QStringList{};
    QTest::addRow("1.0") << QStringLiteral("1.0") <<   1.0f << QStringList{};
    QTest::addRow("123") << QStringLiteral("123") << 123.0f << QStringList{};
    QTest::addRow("-12") << QStringLiteral("-12") << -12.0f << QStringList{};

    QTest::addRow("0")     << QStringLiteral("0")     << 0.0f   << QStringList{};
    QTest::addRow("0.0")   << QStringLiteral("0.0")   << 0.0f   << QStringList{};
    QTest::addRow("0.000") << QStringLiteral("0.000") << 0.0f   << QStringList{};
    QTest::addRow("0.001") << QStringLiteral("0.001") << 0.001f << QStringList{};

    QTest::addRow("empty")
        << QStringLiteral("")
        << std::numeric_limits<float>::quiet_NaN()
        << QStringList{ QStringLiteral("Unrecognised temperature format: ")};

    QTest::addRow("extraneous")
        << QStringLiteral("1.0extraneous")
        << std::numeric_limits<float>::quiet_NaN()
        << QStringList{ QStringLiteral("Unrecognised temperature format: 1.0extraneous")};

    QTest::addRow("invalid")
        << QStringLiteral("invalid")
        << std::numeric_limits<float>::quiet_NaN()
        << QStringList{ QStringLiteral("Unrecognised temperature format: invalid")};
}

void TestCalibrateCommand::processOptions()
{
    QFETCH(QString, argument);
    QFETCH(float, expected);
    QFETCH(QStringList, errors);

    QStringList arguments{ QStringLiteral("pokit") }; // The first argument is always the app name.
    if (!argument.isNull()) {
        arguments += QStringList{ QStringLiteral("--temperature"), argument };
    }

    QCommandLineParser parser;
    parser.addOption({QStringLiteral("temperature"), QStringLiteral("description"), QStringLiteral("degrees")});
    parser.process(arguments);

    CalibrateCommand command(this);
    qInfo() << command.processOptions(parser);
    QCOMPARE(command.processOptions(parser), errors);
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
    if (qIsNaN(expected)) {
        QCOMPARE(qIsNaN(command.temperature), qIsNaN(expected));
    } else // Pre Qt 5.12, QCOMPARE would see two NaN's as not matching.
#endif
    QCOMPARE(command.temperature, expected);
}

void TestCalibrateCommand::getService()
{
    // Unable to safely invoke CalibrateCommand::getService() without a valid Bluetooth device.
}

void TestCalibrateCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke CalibrateCommand::serviceDetailsDiscovered() without a valid service.
}

void TestCalibrateCommand::temperatureCalibrated()
{
    /// \todo Verify the output format.
}

QTEST_MAIN(TestCalibrateCommand)
