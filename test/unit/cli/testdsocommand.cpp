// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdsocommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

#include "dsocommand.h"

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(DsoService::Mode)
Q_DECLARE_METATYPE(DsoService::Settings)
Q_DECLARE_METATYPE(DsoService::Metadata)

typedef quint8 (* minRangeFunc)(const PokitProduct product, const quint32 maxValue);
Q_DECLARE_METATYPE(minRangeFunc)

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

void TestDsoCommand::requiredOptions()
{
    DsoCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) +
        QStringList{ QStringLiteral("mode"), QStringLiteral("range") };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestDsoCommand::supportedOptions()
{
    DsoCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser) +
        QStringList{ QStringLiteral("interval"),      QStringLiteral("samples"),
                     QStringLiteral("trigger-level"), QStringLiteral("trigger-mode") };
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestDsoCommand::processOptions_data()
{
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<DsoService::Settings>("expectedSettings");
    QTest::addColumn<minRangeFunc>("expectedMinRangeFunc");
    QTest::addColumn<quint32>("expectedRangeOptionValue");
    QTest::addColumn<QStringList>("expectedErrors");

    QTest::addRow("missing-required-options")
        << QStringList{}
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 1000*1000, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << QStringList{
            QStringLiteral("Missing required option: mode"),
            QStringLiteral("Missing required option: range") };

    QTest::addRow("missing-required-mode")
        << QStringList{
           QStringLiteral("--range"), QStringLiteral("100")}
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 1000*1000, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << QStringList{ QStringLiteral("Missing required option: mode") };

    QTest::addRow("missing-required-range")
        << QStringList{
           QStringLiteral("--mode"), QStringLiteral("Vdc") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 1000*1000, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << QStringList{ QStringLiteral("Missing required option: range") };

    QTest::addRow("Vdc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("Vac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vac"),
           QStringLiteral("--range"), QStringLiteral("5V") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::AcVoltage,
            +PokitMeter::VoltageRange::_6V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 5000u
        << QStringList{ };

    QTest::addRow("Adc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Adc"),
           QStringLiteral("--range"), QStringLiteral("100mA") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcCurrent,
            +PokitMeter::CurrentRange::_150mA, 1000*1000, 1000}
        << &DsoCommand::minCurrentRange << 100u
        << QStringList{ };

    QTest::addRow("Aac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Aac"),
           QStringLiteral("--range"), QStringLiteral("2A") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::AcCurrent,
            +PokitMeter::CurrentRange::_2A, 1000*1000, 1000}
        << &DsoCommand::minCurrentRange << 2000u
        << QStringList{ };

    QTest::addRow("invalid-mode")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("invalid"),
           QStringLiteral("--range"), QStringLiteral("123") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 1000*1000, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << QStringList{ QStringLiteral("Unknown DSO mode: invalid") };

    QTest::addRow("invalid-range")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("invalid") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 0u
        << QStringList{ QStringLiteral("Invalid range value: invalid") };

    QTest::addRow("trigger-level-requires-trigger-mode")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ QStringLiteral(
            "If either option is provided, then both must be: trigger-level, trigger-mode") };

    QTest::addRow("trigger-mode-requires-trigger-level")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-mode"), QStringLiteral("free") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ QStringLiteral(
            "If either option is provided, then both must be: trigger-level, trigger-mode") };

    QTest::addRow("free-running")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("free") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("rise")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("rise") }
        << DsoService::Settings{
            DsoService::Command::RisingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("rising")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("rising") }
        << DsoService::Settings{
            DsoService::Command::RisingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("fall")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("fall") }
        << DsoService::Settings{
            DsoService::Command::FallingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("falling")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("falling") }
        << DsoService::Settings{
            DsoService::Command::FallingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("invalid-trigger-level")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("invalid"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("free") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ QStringLiteral("Invalid trigger-level value: invalid") };

    QTest::addRow("invalid-trigger-mode")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("invalid") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ QStringLiteral("Unknown trigger mode: invalid") };

    QTest::addRow("interval:100") // Defaults to 100 seconds (ie 100,000,000us).
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("100") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100*1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("interval:499999") // Defaults to 499.999 seconds.
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("499999") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 499999000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("interval:500000") // Defaults to 0.5 seconds (ie 500,000us).
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("500000") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 500*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("invalid-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("invalid") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ QStringLiteral("Invalid interval value: invalid") };

    QTest::addRow("negative-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("-123") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ QStringLiteral("Invalid interval value: -123") };

    QTest::addRow("samples:100")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--samples"), QStringLiteral("100") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 100}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("invalid-samples")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--samples"), QStringLiteral("invalid") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ QStringLiteral("Invalid samples value: invalid") };

    QTest::addRow("too-big-samples")
        << QStringList{
            QStringLiteral("--mode"),  QStringLiteral("Vdc"),
            QStringLiteral("--range"), QStringLiteral("1000mV"),
            QStringLiteral("--samples"), QStringLiteral("65536") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ QStringLiteral("Samples value (65536) must be no greater than 65535") };

    QTest::addRow("possibly-too-big-samples")
        << QStringList{
            QStringLiteral("--mode"),  QStringLiteral("Vdc"),
            QStringLiteral("--range"), QStringLiteral("1000mV"),
            QStringLiteral("--samples"), QStringLiteral("8193") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 8193}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ };

    QTest::addRow("negative-samples")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--samples"), QStringLiteral("-123") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 1000*1000, 1000}
        << &DsoCommand::minVoltageRange << 1000u
        << QStringList{ QStringLiteral("Invalid samples value: -123") };
}

void TestDsoCommand::processOptions()
{
    QFETCH(QStringList, arguments);
    QFETCH(DsoService::Settings, expectedSettings);
    QFETCH(minRangeFunc, expectedMinRangeFunc);
    QFETCH(quint32, expectedRangeOptionValue);
    QFETCH(QStringList, expectedErrors);

    arguments.prepend(QStringLiteral("dokit")); // The first argument is always the app name.

    QCommandLineParser parser;
    parser.addOption({QStringLiteral("mode"), QStringLiteral("description"), QStringLiteral("mode")});
    parser.addOption({QStringLiteral("range"), QStringLiteral("description"), QStringLiteral("range")});
    parser.addOption({QStringLiteral("trigger-level"), QStringLiteral("description"), QStringLiteral("trigger-level")});
    parser.addOption({QStringLiteral("trigger-mode"), QStringLiteral("description"), QStringLiteral("trigger-mode")});
    parser.addOption({QStringLiteral("interval"), QStringLiteral("description"), QStringLiteral("interval")});
    parser.addOption({QStringLiteral("samples"), QStringLiteral("description"), QStringLiteral("samples")});
    parser.process(arguments);

    if (expectedSettings.numberOfSamples > 8192) {
        QTest::ignoreMessage(QtWarningMsg, "Pokit devices do not officially support great than 8192 samples");
    }

    DsoCommand command(this);
    command.service = new DsoService(QLowEnergyController::createCentral(QBluetoothDeviceInfo()));
    command.service->setPokitProduct(PokitProduct::PokitMeter);
    QCOMPARE(command.processOptions(parser),   expectedErrors);
    QCOMPARE(command.settings.command,         expectedSettings.command);
    QCOMPARE(command.settings.triggerLevel,    expectedSettings.triggerLevel);
    QCOMPARE(command.settings.mode,            expectedSettings.mode);
    QCOMPARE(command.settings.range,           (quint8)255); // Always 255, because range is not set until services discovered.
    QCOMPARE(command.settings.samplingWindow,  expectedSettings.samplingWindow);
    QCOMPARE(command.settings.numberOfSamples, expectedSettings.numberOfSamples);
    QVERIFY (command.minRangeFunc       ==     expectedMinRangeFunc);
    QCOMPARE(command.rangeOptionValue,         expectedRangeOptionValue);
}

void TestDsoCommand::getService()
{
    // Unable to safely invoke DsoCommand::getService() without a valid Bluetooth device.
}

void TestDsoCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke DsoCommand::serviceDetailsDiscovered() without a valid service.
}

void TestDsoCommand::settingsWritten()
{
    // Unable to safely invoke DsoCommand::settingsWritten() without a valid Bluetooth service.
}

void TestDsoCommand::metadataRead()
{
    DsoCommand command(this);
    DsoService::Metadata metadata{
        DsoService::DsoStatus::Sampling,
        123.456f,
        DsoService::Mode::DcVoltage,
        +PokitMeter::VoltageRange::_2V,
        123456,
        789,
        1234
    };
    command.metadataRead(metadata);
    QCOMPARE(command.metadata.status,             metadata.status);
    QCOMPARE(command.metadata.scale,              metadata.scale);
    QCOMPARE(command.metadata.mode,               metadata.mode);
    QCOMPARE(command.metadata.range,              metadata.range);
    QCOMPARE(command.metadata.samplingWindow,     metadata.samplingWindow);
    QCOMPARE(command.metadata.numberOfSamples,    metadata.numberOfSamples);
    QCOMPARE(command.metadata.samplingRate,       metadata.samplingRate);
    QCOMPARE(command.samplesToGo,         (qint32)metadata.numberOfSamples);
}

void TestDsoCommand::outputSamples_data()
{
    QTest::addColumn<DsoService::Metadata>("metadata");
    QTest::addColumn<QList<DsoService::Samples>>("samplesList");
    QTest::addColumn<AbstractCommand::OutputFormat>("format");

    const QList<DsoService::Metadata> metadatas{
        { DsoService::DsoStatus::Sampling, 1.0f, DsoService::Mode::DcVoltage,
                    +PokitMeter::VoltageRange::_2V, 1000, 0, 1 },
        { DsoService::DsoStatus::Done, -1.0f, DsoService::Mode::AcVoltage,
                    +PokitMeter::VoltageRange::_2V, 500, 0, 1000 },
        { DsoService::DsoStatus::Error, 0.5f, DsoService::Mode::DcCurrent,
                    +PokitMeter::CurrentRange::_150mA, 100, 0, 1000000 },
        { DsoService::DsoStatus::Sampling, 0.1f, DsoService::Mode::AcCurrent,
                    +PokitMeter::CurrentRange::_2A, 5000, 0, 1000000000 },
    };

    const QList<DsoService::Samples> samplesList{
        {0},
        {1,2,3,4,6,7,8,9,0},
        {2,-32767,0,32767}
    };

    #define DOKIT_ADD_TEST_ROW(name, metadata, list) \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".csv"))) \
            << metadata << list << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".json"))) \
            << metadata << list << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".txt"))) \
            << metadata << list << AbstractCommand::OutputFormat::Text

    for (const DsoService::Metadata &metadata: metadatas) {
        const QString namePrefix = DsoService::toString(metadata.mode)
            .replace(QLatin1Char(' '), QLatin1Char('-'));
        DOKIT_ADD_TEST_ROW(namePrefix + QStringLiteral("-null"),
                             metadata, QList<DsoService::Samples>{ });
        for (const DsoService::Samples &samples: samplesList) {
            DOKIT_ADD_TEST_ROW(namePrefix + QString::number(samples.front()), metadata,
                                 QList<DsoService::Samples>{ samples });
        }
        DOKIT_ADD_TEST_ROW(namePrefix + QStringLiteral("-all"), metadata, samplesList);
    }
    #undef DOKIT_ADD_TEST_ROW
}

void TestDsoCommand::outputSamples()
{
    QFETCH(DsoService::Metadata, metadata);
    QFETCH(QList<DsoService::Samples>, samplesList);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    const OutputStreamCapture capture(&std::cout);
    DsoCommand command;
    command.service = new DsoService(QLowEnergyController::createCentral(QBluetoothDeviceInfo()));
    command.service->setPokitProduct(PokitProduct::PokitMeter);
    command.metadataRead(metadata);
    command.format = format;
    for (const DsoService::Samples &samples: samplesList) {
        command.outputSamples(samples);
    }
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestDsoCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    DsoCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestDsoCommand)
