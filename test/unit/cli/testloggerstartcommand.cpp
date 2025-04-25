// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testloggerstartcommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"
#include "../stringliterals_p.h"

#include "loggerstartcommand.h"

#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(DataLoggerService::Mode)
Q_DECLARE_METATYPE(DataLoggerService::Settings)

typedef quint8 (* minRangeFunc)(const PokitProduct product, const quint32 maxValue);
Q_DECLARE_METATYPE(minRangeFunc)

DOKIT_USE_STRINGLITERALS

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

void TestLoggerStartCommand::requiredOptions()
{
    LoggerStartCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) +
        QStringList{ u"mode"_s };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestLoggerStartCommand::supportedOptions()
{
    LoggerStartCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser) +
        QStringList{ u"interval"_s, u"range"_s, u"timestamp"_s};
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestLoggerStartCommand::processOptions_data()
{
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<DataLoggerService::Settings>("expectedSettings");
    QTest::addColumn<minRangeFunc>("expectedMinRangeFunc");
    QTest::addColumn<quint32>("expectedRangeOptionValue");
    QTest::addColumn<bool>("expectNowish");
    QTest::addColumn<QStringList>("expectedErrors");

    QTest::addRow("missing-required-options")
        << QStringList{}
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::AutoRange, 60000, 0}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << false
        << QStringList{ u"Missing required option: mode"_s };

    QTest::addRow("missing-required-mode")
        << QStringList{
           u"--range"_s, u"100"_s}
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::AutoRange, 60000, 0}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << false
        << QStringList{ u"Missing required option: mode"_s };

    QTest::addRow("missing-required-range")
        << QStringList{
           u"--mode"_s, u"Vdc"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::AutoRange, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 0u
        << true
        << QStringList{ u"Missing required option for logger mode 'Vdc': range"_s };

    QTest::addRow("Vdc")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s }
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << true
        << QStringList{ };

    QTest::addRow("Vac")
        << QStringList{
           u"--mode"_s,  u"Vac"_s,
           u"--range"_s, u"5V"_s }
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::AcVoltage,
            +PokitMeter::VoltageRange::_6V, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 5000u
        << true
        << QStringList{ };

    QTest::addRow("Adc")
        << QStringList{
           u"--mode"_s,  u"Adc"_s,
           u"--range"_s, u"100mA"_s }
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcCurrent,
            +PokitMeter::CurrentRange::_150mA, 60000, 0}
        << &LoggerStartCommand::minCurrentRange << 100u
        << true
        << QStringList{ };

    QTest::addRow("Aac")
        << QStringList{
           u"--mode"_s,  u"Aac"_s,
           u"--range"_s, u"2A"_s }
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::AcCurrent,
            +PokitMeter::CurrentRange::_2A, 60000, 0}
        << &LoggerStartCommand::minCurrentRange << 2000u
        << true
        << QStringList{ };

    QTest::addRow("Temperature")
        << QStringList{
           u"--mode"_s,  u"temp"_s,
           u"--range"_s, u"2A"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::Temperature,
           +PokitMeter::VoltageRange::_60V, 60000, 0}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << true
        << QStringList{ };

    QTest::addRow("unnecessary-range")
        << QStringList{
           u"--mode"_s,  u"temp"_s,
           u"--range"_s, u"10"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::Temperature,
           +PokitMeter::VoltageRange::_60V, 60000, 0}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << true
        << QStringList{ };

    QTest::addRow("invalid-mode")
        << QStringList{
           u"--mode"_s,  u"invalid"_s,
           u"--range"_s, u"123"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::AutoRange, 60000, 0}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << false
        << QStringList{ u"Unknown logger mode: invalid"_s };

    QTest::addRow("invalid-range")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"invalid"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::AutoRange, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 0u
        << true
        << QStringList{ u"Invalid range value: invalid"_s };

    QTest::addRow("interval:100") // Defaults to 100 seconds (ie 100,000ms).
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 100000, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << true
        << QStringList{ };

    QTest::addRow("interval:499") // Defaults to 499 seconds.
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"499"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 499000, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << true
        << QStringList{ };

    QTest::addRow("interval:500") // Defaults to 500ms.
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"500"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 500, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << true
        << QStringList{ };

    QTest::addRow("invalid-interval")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"invalid"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << true
        << QStringList{ u"Invalid interval value: invalid"_s };

    QTest::addRow("negative-interval")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"-123"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << true
        << QStringList{ u"Invalid interval value: -123"_s };

    QTest::addRow("timestamp:0")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--timestamp"_s, u"0"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << false
        << QStringList{ };

    QTest::addRow("timestamp:1")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--timestamp"_s, u"1"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 60000, 1}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << false
        << QStringList{ };

    QTest::addRow("timestamp:now")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--timestamp"_s, QString::number(QDateTime::currentSecsSinceEpoch()) }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << true
        << QStringList{ };

    QTest::addRow("invalid-timestamp")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--timestamp"_s, u"invalid"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << true
        << QStringList{ u"Invalid timestamp value: invalid"_s };

    QTest::addRow("negative-timestamp")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--timestamp"_s, u"-123"_s }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           +PokitMeter::VoltageRange::_2V, 60000, 0}
        << &LoggerStartCommand::minVoltageRange << 1000u
        << true
        << QStringList{ u"Invalid timestamp value: -123"_s };
}

void TestLoggerStartCommand::processOptions()
{
    QFETCH(QStringList, arguments);
    QFETCH(DataLoggerService::Settings, expectedSettings);
    QFETCH(minRangeFunc, expectedMinRangeFunc);
    QFETCH(quint32, expectedRangeOptionValue);
    QFETCH(bool, expectNowish);
    QFETCH(QStringList, expectedErrors);

    arguments.prepend(u"dokit"_s); // The first argument is always the app name.

    QCommandLineParser parser;
    parser.addOption({u"mode"_s, u"description"_s, u"mode"_s});
    parser.addOption({u"range"_s, u"description"_s, u"range"_s});
    parser.addOption({u"interval"_s, u"description"_s, u"interval"_s});
    parser.addOption({u"timestamp"_s, u"description"_s, u"timestamp"_s});
    parser.process(arguments);

    LoggerStartCommand command(this);
    const quint32 now = (quint32)QDateTime::currentSecsSinceEpoch();
    QCOMPARE(command.processOptions(parser),  expectedErrors);
    QCOMPARE(command.settings.command,        expectedSettings.command);
    QCOMPARE(command.settings.arguments,      expectedSettings.arguments);
    QCOMPARE(command.settings.mode,           expectedSettings.mode);
    QCOMPARE(command.settings.range,          (quint8)255); // Always 255, because range is not set until services discovered.
    QCOMPARE(command.settings.updateInterval, expectedSettings.updateInterval);
    QVERIFY (command.minRangeFunc      ==     expectedMinRangeFunc);
    QCOMPARE(command.rangeOptionValue,        expectedRangeOptionValue);

    // Recoginise if/when LoggerStartCommand::processOptions automatically uses 'now' for timestamp.
    if (expectNowish) {
        Q_ASSERT(expectedSettings.timestamp == 0);
        // Fuzzy compare, with up 3 seconds tolerance. Using qMin and qMax here instead of qAbs to
        // avoid negative wrap-around, since both values are unsigned.
        QVERIFY((qMax(now, command.settings.timestamp) -
                 qMin(now, command.settings.timestamp)) <= 3);
    } else {
        QCOMPARE(command.settings.timestamp,  expectedSettings.timestamp);
    }
}

void TestLoggerStartCommand::getService()
{
    // Unable to safely invoke LoggerStartCommand::getService() without a valid Bluetooth device.
}

void TestLoggerStartCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke LoggerStartCommand::serviceDetailsDiscovered() without a valid service.
}

void TestLoggerStartCommand::settingsWritten_data()
{
    QTest::addColumn<AbstractCommand::OutputFormat>("format");
    QTest::newRow("1.csv") << AbstractCommand::OutputFormat::Csv;
    QTest::newRow("1.json") << AbstractCommand::OutputFormat::Json;
    QTest::newRow("1.txt") << AbstractCommand::OutputFormat::Text;
}

void TestLoggerStartCommand::settingsWritten()
{
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    OutputStreamCapture capture(&std::cout);
    LoggerStartCommand command;
    command.format = format;
    command.settingsWritten();
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestLoggerStartCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    LoggerStartCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestLoggerStartCommand)
