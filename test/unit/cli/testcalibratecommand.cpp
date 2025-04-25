// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testcalibratecommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"
#include "../stringliterals_p.h"

#include "calibratecommand.h"

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)

DOKIT_USE_STRINGLITERALS

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

void TestCalibrateCommand::requiredOptions()
{
    CalibrateCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) + QStringList{ u"temperature"_s };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestCalibrateCommand::supportedOptions()
{
    CalibrateCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser);
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestCalibrateCommand::processOptions_data()
{
    QTest::addColumn<QString>("argument");
    QTest::addColumn<float>("expected");
    QTest::addColumn<QStringList>("errors");

    QTest::addRow("missing")
        << QString()
        << std::numeric_limits<float>::quiet_NaN()
        << QStringList{ u"Missing required option: temperature"_s};

    QTest::addRow("1")   << u"1"_s   <<   1.0f << QStringList{};
    QTest::addRow("1.0") << u"1.0"_s <<   1.0f << QStringList{};
    QTest::addRow("123") << u"123"_s << 123.0f << QStringList{};
    QTest::addRow("-12") << u"-12"_s << -12.0f << QStringList{};

    QTest::addRow("0")     << u"0"_s     << 0.0f   << QStringList{};
    QTest::addRow("0.0")   << u"0.0"_s   << 0.0f   << QStringList{};
    QTest::addRow("0.000") << u"0.000"_s << 0.0f   << QStringList{};
    QTest::addRow("0.001") << u"0.001"_s << 0.001f << QStringList{};

    QTest::addRow("empty")
        << u""_s
        << std::numeric_limits<float>::quiet_NaN()
        << QStringList{ u"Unrecognised temperature format: "_s};

    QTest::addRow("extraneous")
        << u"1.0extraneous"_s
        << std::numeric_limits<float>::quiet_NaN()
        << QStringList{ u"Unrecognised temperature format: 1.0extraneous"_s};

    QTest::addRow("invalid")
        << u"invalid"_s
        << std::numeric_limits<float>::quiet_NaN()
        << QStringList{ u"Unrecognised temperature format: invalid"_s};
}

void TestCalibrateCommand::processOptions()
{
    QFETCH(QString, argument);
    QFETCH(float, expected);
    QFETCH(QStringList, errors);

    QStringList arguments{ u"dokit"_s }; // The first argument is always the app name.
    if (!argument.isNull()) {
        arguments += QStringList{ u"--temperature"_s, argument };
    }

    QCommandLineParser parser;
    parser.addOption({u"temperature"_s, u"description"_s, u"degrees"_s});
    parser.process(arguments);

    CalibrateCommand command(this);
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

void TestCalibrateCommand::temperatureCalibrated_data()
{
    QTest::addColumn<AbstractCommand::OutputFormat>("format");
    QTest::newRow("1.csv") << AbstractCommand::OutputFormat::Csv;
    QTest::newRow("1.json") << AbstractCommand::OutputFormat::Json;
    QTest::newRow("1.txt") << AbstractCommand::OutputFormat::Text;
}

void TestCalibrateCommand::temperatureCalibrated()
{
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    OutputStreamCapture capture(&std::cout);
    CalibrateCommand command;
    command.format = format;
    command.temperatureCalibrated();
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestCalibrateCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    CalibrateCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestCalibrateCommand)
