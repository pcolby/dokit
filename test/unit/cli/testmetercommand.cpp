// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testmetercommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"
#include "../github.h"
#include "../stringliterals_p.h"

#include "metercommand.h"

#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(MultimeterService::Mode)
Q_DECLARE_METATYPE(MultimeterService::Reading)
Q_DECLARE_METATYPE(MultimeterService::Settings)

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

void TestMeterCommand::requiredOptions()
{
    MeterCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) +
        QStringList{ u"mode"_s };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestMeterCommand::supportedOptions()
{
    MeterCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser) +
        QStringList{ u"interval"_s, u"range"_s, u"samples"_s };
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestMeterCommand::processOptions_data()
{
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<MultimeterService::Settings>("expectedSettings");
    QTest::addColumn<minRangeFunc>("expectedMinRangeFunc");
    QTest::addColumn<quint32>("expectedRangeOptionValue");
    QTest::addColumn<int>("expectedSamples");
    QTest::addColumn<QStringList>("expectedErrors");

    QTest::addRow("missing-required-mode")
        << QStringList{
           u"--range"_s, u"100"_s}
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ u"Missing required option: mode"_s };

    QTest::addRow("Vdc")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::_2V, 1000}
        << &MeterCommand::minVoltageRange << 1000u
        << -1
        << QStringList{ };

    QTest::addRow("Vac")
        << QStringList{
           u"--mode"_s,  u"Vac"_s,
           u"--range"_s, u"5V"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::AcVoltage, +PokitMeter::VoltageRange::_6V, 1000}
        << &MeterCommand::minVoltageRange << 5000u
        << -1
        << QStringList{ };

    QTest::addRow("Adc")
        << QStringList{
           u"--mode"_s,  u"Adc"_s,
           u"--range"_s, u"100mA"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcCurrent, +PokitMeter::CurrentRange::_150mA, 1000}
        << &MeterCommand::minCurrentRange << 100u
        << -1
        << QStringList{ };

    QTest::addRow("Aac")
        << QStringList{
           u"--mode"_s,  u"Aac"_s,
           u"--range"_s, u"2A"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::AcCurrent, +PokitMeter::CurrentRange::_2A, 1000}
        << &MeterCommand::minCurrentRange << 2000u
        << -1
        << QStringList{ };

    QTest::addRow("Resistance")
        << QStringList{ u"--mode"_s,  u"res"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::Resistance, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minResistanceRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("Diode")
        << QStringList{ u"--mode"_s,  u"dio"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::Diode, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ };

    QTest::addRow("Continuity")
        << QStringList{ u"--mode"_s,  u"cont"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::Continuity, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ };

    QTest::addRow("Temperature")
        << QStringList{ u"--mode"_s,  u"temp"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::Temperature, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ };

    QTest::addRow("Capacitance")
        << QStringList{
           u"--mode"_s,  u"cap"_s,
           u"--range"_s, u"500uF"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::Capacitance, +PokitPro::CapacitanceRange::_100nF, 1000}
        << &MeterCommand::minCapacitanceRange << 500000u
        << -1
        << QStringList{ };

    QTest::addRow("invalid-mode")
        << QStringList{ u"--mode"_s,  u"invalid"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ u"Unknown meter mode: invalid"_s };

    QTest::addRow("interval:100") // Defaults to 100 seconds (ie 100,000ms).
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--interval"_s, u"100"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 100000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("interval:499") // Defaults to 499 seconds.
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--interval"_s, u"499"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 499000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("interval:500") // Defaults to 500ms.
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--interval"_s, u"500"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 500}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("invalid-interval")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--interval"_s, u"invalid"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ u"Invalid interval value: invalid"_s };

    QTest::addRow("negative-interval")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--interval"_s, u"-123"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ u"Invalid interval value: -123"_s };

    QTest::addRow("auto-range-voltage")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"auto"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("auto-range-current")
        << QStringList{
           u"--mode"_s,  u"Adc"_s,
           u"--range"_s, u"auto"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcCurrent, +PokitMeter::CurrentRange::AutoRange, 1000}
        << &MeterCommand::minCurrentRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("auto-range-resistance")
        << QStringList{
           u"--mode"_s,  u"resis"_s,
           u"--range"_s, u"auto"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::Resistance, +PokitMeter::ResistanceRange::AutoRange, 1000}
        << &MeterCommand::minResistanceRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("auto-range-capacitance")
        << QStringList{
                       u"--mode"_s,  u"capac"_s,
                       u"--range"_s, u"auto"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::Capacitance, +PokitPro::CapacitanceRange::AutoRange, 1000}
        << &MeterCommand::minCapacitanceRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("invalid-range")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"invalid"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ u"Invalid range value: invalid"_s };

    QTest::addRow("rangeless-range")
        << QStringList{
           u"--mode"_s,  u"diode"_s,
           u"--range"_s, u"1000"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::Diode, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ };

    QTest::addRow("samples:100")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--samples"_s, u"100"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << 100
        << QStringList{ };

    QTest::addRow("invalid-samples")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--samples"_s, u"invalid"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ u"Invalid samples value: invalid"_s };

    QTest::addRow("negative-samples")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--samples"_s, u"-123"_s }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ u"Invalid samples value: -123"_s };
}

void TestMeterCommand::processOptions()
{
    QFETCH(QStringList, arguments);
    QFETCH(MultimeterService::Settings, expectedSettings);
    QFETCH(minRangeFunc, expectedMinRangeFunc);
    QFETCH(quint32, expectedRangeOptionValue);
    QFETCH(int, expectedSamples);
    QFETCH(QStringList, expectedErrors);

    arguments.prepend(u"dokit"_s); // The first argument is always the app name.

    QCommandLineParser parser;
    parser.addOption({u"mode"_s, u"description"_s, u"mode"_s});
    parser.addOption({u"range"_s, u"description"_s, u"range"_s});
    parser.addOption({u"interval"_s, u"description"_s, u"interval"_s});
    parser.addOption({u"samples"_s, u"description"_s, u"samples"_s});
    parser.process(arguments);

    MeterCommand command(this);
    QCOMPARE(command.processOptions(parser),  expectedErrors);
    QCOMPARE(command.settings.mode,           expectedSettings.mode);
    QCOMPARE(command.settings.range,          (quint8)255); // Always 255, because range is not set until services discovered.
    QCOMPARE(command.settings.updateInterval, expectedSettings.updateInterval);
    QCOMPARE(command.samplesToGo,             expectedSamples);
    QVERIFY (command.minRangeFunc      ==     expectedMinRangeFunc);
    QCOMPARE(command.rangeOptionValue,        expectedRangeOptionValue);
}

void TestMeterCommand::getService()
{
    // Unable to safely invoke MeterCommand::getService() without a valid Bluetooth device.
}

void TestMeterCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke MeterCommand::serviceDetailsDiscovered() without a valid service.
}

void TestMeterCommand::settingsWritten()
{
    // Unable to safely invoke MeterCommand::settingsWritten() without a valid Bluetooth service.
}

void TestMeterCommand::outputReading_data()
{
    if (gitHubActionsRunnerOsVersion() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 14)) {
        QSKIP("BLE controller operations hang on GitHub Actions's macOS 14 runners");
    }

    QTest::addColumn<QList<MultimeterService::Reading>>("readings");
    QTest::addColumn<AbstractCommand::OutputFormat>("format");

    const QList<MultimeterService::Reading> readings{
        { MultimeterService::MeterStatus::Ok, 0.0f, MultimeterService::Mode::Idle,
              +PokitMeter::VoltageRange::AutoRange },
        { MultimeterService::MeterStatus::AutoRangeOn, 1.0f, MultimeterService::Mode::DcVoltage,
              +PokitMeter::VoltageRange::_2V },
        { MultimeterService::MeterStatus::AutoRangeOff, -1.0f, MultimeterService::Mode::AcVoltage,
              +PokitMeter::VoltageRange::_2V },
        { MultimeterService::MeterStatus::AutoRangeOn, 0.5f, MultimeterService::Mode::DcCurrent,
              +PokitMeter::CurrentRange::_150mA },
        { MultimeterService::MeterStatus::AutoRangeOff, 0.1f, MultimeterService::Mode::AcCurrent,
              +PokitMeter::CurrentRange::_2A },
        { MultimeterService::MeterStatus::AutoRangeOff, 123.4f, MultimeterService::Mode::Resistance,
              +PokitMeter::CurrentRange::_2A },
        { MultimeterService::MeterStatus::AutoRangeOff, 576.89f, MultimeterService::Mode::Diode,
              +PokitMeter::CurrentRange::_2A },
        { MultimeterService::MeterStatus::Continuity, 0.0f, MultimeterService::Mode::Continuity,
              +PokitMeter::CurrentRange::AutoRange },
        { MultimeterService::MeterStatus::NoContinuity, 0.0f, MultimeterService::Mode::Continuity,
              +PokitMeter::CurrentRange::AutoRange },
        { MultimeterService::MeterStatus::AutoRangeOff, 32.0f, MultimeterService::Mode::Temperature,
              +PokitMeter::CurrentRange::AutoRange },
        { MultimeterService::MeterStatus::AutoRangeOff, 0.0005f, MultimeterService::Mode::Capacitance,
              +PokitPro::CapacitanceRange::_1mF },
        { MultimeterService::MeterStatus::Error, 0.0f, MultimeterService::Mode::Idle,
              +PokitMeter::VoltageRange::AutoRange },
    };

    #define DOKIT_ADD_TEST_ROW(name, readings) \
        QTest::newRow(qUtf8Printable(name + u".csv"_s)) \
            << readings << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(qUtf8Printable(name + u".json"_s)) \
            << readings << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(qUtf8Printable(name + u".txt"_s)) \
            << readings << AbstractCommand::OutputFormat::Text

    DOKIT_ADD_TEST_ROW(u"null"_s,
                         QList<MultimeterService::Reading>{ MultimeterService::Reading() });

    for (const MultimeterService::Reading &reading: readings) {
        QString name = MultimeterService::toString(reading.mode).replace(' '_L1, '-'_L1);
        if ((reading.mode == MultimeterService::Mode::Continuity) &&
            (!((quint8)reading.status & (quint8)MultimeterService::MeterStatus::Continuity))) {
            name.prepend(u"No"_s); // ie NoContinuity.
        }
        if (reading.status == MultimeterService::MeterStatus::Error) {
            name = u"Error"_s;
        }
        DOKIT_ADD_TEST_ROW(name, QList<MultimeterService::Reading>{ reading });
    }

    DOKIT_ADD_TEST_ROW(u"all"_s, readings);
    #undef DOKIT_ADD_TEST_ROW
}

void TestMeterCommand::outputReading()
{
    QFETCH(QList<MultimeterService::Reading>, readings);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    const OutputStreamCapture capture(&std::cout);
    MeterCommand command;
    command.service = new MultimeterService(QLowEnergyController::createCentral(QBluetoothDeviceInfo()));
    command.service->setPokitProduct((readings.first().mode == MultimeterService::Mode::Capacitance)
        ? PokitProduct::PokitPro : PokitProduct::PokitMeter); // Only Pokit Pro supports capacitance.
    command.format = format;
    for (const MultimeterService::Reading &reading: readings) {
        command.outputReading(reading);
    }
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestMeterCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    MeterCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestMeterCommand)
