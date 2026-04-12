// SPDX-FileCopyrightText: 2022-2026 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdsocommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"
#include "../github.h"
#include "../stringliterals_p.h"

#include "dsocommand.h"

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(DsoService::Mode)
Q_DECLARE_METATYPE(DsoService::Settings)
Q_DECLARE_METATYPE(DsoService::Metadata)
Q_DECLARE_METATYPE(PokitProduct)

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

void TestDsoCommand::requiredOptions()
{
    DsoCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) +
        QStringList{ u"mode"_s, u"range"_s };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestDsoCommand::supportedOptions()
{
    DsoCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser) +
        QStringList{ u"interval"_s,      u"samples"_s, u"sample-rate"_s,
                     u"trigger-level"_s, u"trigger-mode"_s, u"window-size"_s };
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestDsoCommand::processOptions_data()
{
    if (gitHubActionsRunnerOsVersion() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 14)) {
        QSKIP("BLE controller operations hang on GitHub Actions's macOS 14 runners");
    }

    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<DsoService::Settings>("expectedSettings");
    QTest::addColumn<minRangeFunc>("expectedMinRangeFunc");
    QTest::addColumn<quint32>("expectedRangeOptionValue");
    QTest::addColumn<quint32>("expectedSampleRateValue");
    QTest::addColumn<QStringList>("expectedErrors");

    QTest::addRow("missing-required-options")
        << QStringList{}
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 0, 0}
        << static_cast<minRangeFunc>(nullptr) << 0u << 0u
        << QStringList{
            u"Missing required option: mode"_s,
            u"Missing required option: range"_s };

    QTest::addRow("missing-required-mode")
        << QStringList{
           u"--range"_s, u"100"_s}
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 0, 0}
        << static_cast<minRangeFunc>(nullptr) << 0u << 0u
        << QStringList{ u"Missing required option: mode"_s };

    QTest::addRow("missing-required-range")
        << QStringList{
           u"--mode"_s, u"Vdc"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 0, 0}
        << static_cast<minRangeFunc>(nullptr) << 0u << 0u
        << QStringList{ u"Missing required option: range"_s };

    QTest::addRow("missing-required-samplate-rate")
        << QStringList{
            u"--mode"_s,  u"Vdc"_s,
            u"--range"_s, u"1000mV"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0}
        << &DsoCommand::minVoltageRange << 1000u << 0u
        << QStringList{ u"Missing required option/s: either sample-rate, or both interval and window-size"_s };

    QTest::addRow("Vdc")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0}
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("Vac")
        << QStringList{
           u"--mode"_s,  u"Vac"_s,
           u"--range"_s, u"5V"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::AcVoltage,
            +PokitMeter::VoltageRange::_6V, 0, 0 }
        << &DsoCommand::minVoltageRange << 5000u << 500'000u
        << QStringList{ };

    QTest::addRow("Adc")
        << QStringList{
           u"--mode"_s,  u"Adc"_s,
           u"--range"_s, u"100mA"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcCurrent,
            +PokitMeter::CurrentRange::_150mA, 0, 0 }
        << &DsoCommand::minCurrentRange << 100u << 500'000u
        << QStringList{ };

    QTest::addRow("Aac")
        << QStringList{
           u"--mode"_s,  u"Aac"_s,
           u"--range"_s, u"2A"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::AcCurrent,
            +PokitMeter::CurrentRange::_2A, 0, 0 }
        << &DsoCommand::minCurrentRange << 2000u << 500'000u
        << QStringList{ };

    QTest::addRow("invalid-mode")
        << QStringList{
           u"--mode"_s,  u"invalid"_s,
           u"--range"_s, u"123"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 0, 0 }
        << static_cast<minRangeFunc>(nullptr) << 0u << 0u
        << QStringList{ u"Unknown DSO mode: invalid"_s };

    QTest::addRow("invalid-range")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"invalid"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            255, 0, 0 }
        << &DsoCommand::minVoltageRange << 0u << 500'000u
        << QStringList{ u"Invalid range value: invalid"_s };

    QTest::addRow("trigger-level-requires-trigger-mode")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--sample-rate"_s, u"500kHz"_s,
           u"--trigger-level"_s, u"123mV"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"If either option is provided, then both must be: trigger-level, trigger-mode"_s };

    QTest::addRow("trigger-mode-requires-trigger-level")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--trigger-mode"_s, u"free"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"If either option is provided, then both must be: trigger-level, trigger-mode"_s };

    QTest::addRow("free-running")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--trigger-level"_s, u"123mV"_s,
           u"--trigger-mode"_s,  u"free"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("rise")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--trigger-level"_s, u"123mV"_s,
           u"--trigger-mode"_s,  u"rise"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::RisingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("rising")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--trigger-level"_s, u"123mV"_s,
           u"--trigger-mode"_s,  u"rising"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::RisingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("fall")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--trigger-level"_s, u"123mV"_s,
           u"--trigger-mode"_s,  u"fall"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FallingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("falling")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--trigger-level"_s, u"123mV"_s,
           u"--trigger-mode"_s,  u"falling"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FallingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("invalid-trigger-level")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--trigger-level"_s, u"invalid"_s,
           u"--trigger-mode"_s,  u"free"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"Invalid trigger-level value: invalid"_s };

    QTest::addRow("invalid-trigger-mode")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--trigger-level"_s, u"123mV"_s,
           u"--trigger-mode"_s,  u"invalid"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.123f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"Unknown trigger mode: invalid"_s };

    QTest::addRow("sample-rate:100") // Defaults to 100kHz.
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100"_s,
           u"--sample-rate"_s, u"100"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100*1000*1000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 100'000u
        << QStringList{ };

    QTest::addRow("sample-rate:499999") // Defaults to 100kHz.
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100"_s,
           u"--sample-rate"_s, u"499999"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100*1000*1000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 499'999u
        << QStringList{ };

    QTest::addRow("sample-rate:500000") // Defaults to 100kHz.
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100"_s,
           u"--sample-rate"_s, u"500000"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100*1000*1000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("sample-rate:123kHz")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100"_s,
           u"--sample-rate"_s, u"123"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100*1000*1000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 123'000u
        << QStringList{ };

    QTest::addRow("sample-rate:9.9M")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100"_s,
           u"--sample-rate"_s, u"9.9M"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100*1000*1000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 9'900'000u
        << QStringList{ };

    QTest::addRow("invalid-sample-rate")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100"_s,
           u"--sample-rate"_s, u"invalid"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100*1000*1000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 0u
        << QStringList{ u"Invalid sample-rate value: invalid"_s };

    QTest::addRow("negative-sample-rate")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100"_s,
           u"--sample-rate"_s, u"-123"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100*1000*1000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 0u
        << QStringList{ u"Invalid sample-rate value: -123"_s };

    QTest::addRow("interval:100") // Defaults to 100 seconds (ie 100,000,000us).
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100*1000*1000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("interval:499999") // Defaults to 499.999 seconds.
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"499999"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 499999000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("interval:500000") // Defaults to 0.5 seconds (ie 500,000us).
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"500000"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 500*1000, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("invalid-interval")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"invalid"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0}
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"Invalid interval value: invalid"_s };

    QTest::addRow("negative-interval")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"-123"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"Invalid interval value: -123"_s };

    QTest::addRow("window-size:100")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--sample-rate"_s, u"500kHz"_s,
           u"--window-size"_s, u"100"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 100 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("window-size:1000")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--sample-rate"_s, u"500kHz"_s,
           u"--window-size"_s, u"1000"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 1000 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("invalid-window-size")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--sample-rate"_s, u"500kHz"_s,
           u"--window-size"_s, u"invalid"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"Invalid window-size value: invalid"_s };

    QTest::addRow("negative-window-size")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--sample-rate"_s, u"500kHz"_s,
           u"--window-size"_s, u"-123"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"Invalid window-size value: -123"_s };

    QTest::addRow("too-big-window-size")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--sample-rate"_s, u"500kHz"_s,
           u"--window-size"_s, u"16385"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 16385 }
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    // Sample-rate is not required, if both interval and window-size are provided.
    QTest::addRow("interval-and-window-size")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--interval"_s, u"100ms"_s,
           u"--window-size"_s, u"8k"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100'000, 8000 }
        << &DsoCommand::minVoltageRange << 1000u << 0u
        << QStringList{ };

    // But if all three are present, the must match.
    QTest::addRow("sample-rate-interval-and-window-size")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--sample-rate"_s, u"80kHz"_s,
           u"--interval"_s, u"100ms"_s,
           u"--window-size"_s, u"8k"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100'000, 8000 }
        << &DsoCommand::minVoltageRange << 1000u << 80'000u
        << QStringList{ };

    QTest::addRow("sample-rate-interval-and-window-size:mismatch")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--sample-rate"_s, u"250kHz"_s,
           u"--interval"_s, u"100ms"_s,
           u"--window-size"_s, u"8k"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 100'000, 8000 }
        << &DsoCommand::minVoltageRange << 1000u << 250'000u
        << QStringList{ u"Windows size (8000 samples) and interval (100000ns) yield a sample rate of 80000Hz, which "
            "does not match the supplied sample-rate (250000Hz). Tip: leave one option unset to have dokit calculate "
            "the remaining option."_s };

    QTest::addRow("samples:100")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--samples"_s, u"100"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 100}
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("invalid-samples")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--samples"_s, u"invalid"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0}
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"Invalid samples value: invalid"_s };

    /// \todo This limit will be removed soon.
    QTest::addRow("too-big-samples")
        << QStringList{
            u"--mode"_s,  u"Vdc"_s,
            u"--range"_s, u"1000mV"_s,
            u"--samples"_s, u"65536"_s,
            u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0}
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"Samples value (65536) must be no greater than 65535"_s };

    QTest::addRow("possibly-too-big-samples")
        << QStringList{
            u"--mode"_s,  u"Vdc"_s,
            u"--range"_s, u"1000mV"_s,
            u"--samples"_s, u"16385"_s,
            u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 16385}
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ };

    QTest::addRow("negative-samples")
        << QStringList{
           u"--mode"_s,  u"Vdc"_s,
           u"--range"_s, u"1000mV"_s,
           u"--samples"_s, u"-123"_s,
           u"--sample-rate"_s, u"500kHz"_s }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            +PokitMeter::VoltageRange::_2V, 0, 0}
        << &DsoCommand::minVoltageRange << 1000u << 500'000u
        << QStringList{ u"Invalid samples value: -123"_s };
}

void TestDsoCommand::processOptions()
{
    QFETCH(QStringList, arguments);
    QFETCH(DsoService::Settings, expectedSettings);
    QFETCH(minRangeFunc, expectedMinRangeFunc);
    QFETCH(quint32, expectedRangeOptionValue);
    QFETCH(quint32, expectedSampleRateValue);
    QFETCH(QStringList, expectedErrors);

    arguments.prepend(u"dokit"_s); // The first argument is always the app name.

    QCommandLineParser parser;
    parser.addOption({u"mode"_s, u"description"_s, u"mode"_s});
    parser.addOption({u"range"_s, u"description"_s, u"range"_s});
    parser.addOption({u"trigger-level"_s, u"description"_s, u"trigger-level"_s});
    parser.addOption({u"trigger-mode"_s, u"description"_s, u"trigger-mode"_s});
    parser.addOption({u"interval"_s, u"description"_s, u"interval"_s});
    parser.addOption({u"samples"_s, u"description"_s, u"samples"_s});
    parser.addOption({u"sample-rate"_s, u"description"_s, u"rate"_s});
    parser.addOption({u"window-size"_s, u"description"_s, u"samples"_s});
    parser.process(arguments);

    if (expectedSettings.numberOfSamples > 16384) {
        QTest::ignoreMessage(QtWarningMsg, "Pokit devices do not officially support windows greater than 16,384 samples");
    }

    DsoCommand command(this);
    command.service = new DsoService(QLowEnergyController::createCentral(QBluetoothDeviceInfo()));
    command.service->setPokitProduct(PokitProduct::PokitMeter);
    const auto errors = command.processOptions(parser);
    if (errors != expectedErrors) {
        qDebug() << errors;
        qDebug() << expectedErrors;
    }
    QCOMPARE(command.processOptions(parser),   expectedErrors);
    QCOMPARE(command.settings.command,         expectedSettings.command);
    QCOMPARE(command.settings.triggerLevel,    expectedSettings.triggerLevel);
    QCOMPARE(command.settings.mode,            expectedSettings.mode);
    QCOMPARE(command.settings.range,           (quint8)255); // Always 255, because range is not set until services discovered.
    QCOMPARE(command.settings.samplingWindow,  expectedSettings.samplingWindow);
    QCOMPARE(command.settings.numberOfSamples, expectedSettings.numberOfSamples);
    QVERIFY (command.minRangeFunc       ==     expectedMinRangeFunc);
    QCOMPARE(command.rangeOptionValue,         expectedRangeOptionValue);
    QCOMPARE(command.sampleRateValue,          expectedSampleRateValue);
}

void TestDsoCommand::getService()
{
    // Unable to safely invoke DsoCommand::getService() without a valid Bluetooth device.
}

void TestDsoCommand::maxWindowSize_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint16>("expected");

    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter << (quint16)8'191;
    QTest::addRow("Pokit Pro")   << PokitProduct::PokitPro << (quint16)16'384;
}

void TestDsoCommand::maxWindowSize()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint16, expected);
    QCOMPARE(DsoCommand::maxWindowSize(product), expected);
}

void TestDsoCommand::configureWindow_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addColumn<quint32>("sampleRate");
    QTest::addColumn<quint32>("samplingWindow");
    QTest::addColumn<quint16>("numberOfSamples");
    QTest::addColumn<bool>("expectSuccess");
    QTest::addColumn<quint32>("expectedSamplingWindow");
    QTest::addColumn<quint16>("expectedNumberOfSamples");

    // Sample-rates only.
    QTest::addRow("PokitMeter:1Hz")
        << PokitProduct::PokitMeter << (quint32)1 << (quint32)0 << (quint16)0
        << true << (quint32)1'000'000 << (quint16)1;
    QTest::addRow("PokitMeter:123kHz")
        << PokitProduct::PokitMeter << (quint32)123'000 << (quint32)0 << (quint16)0
        << true << (quint32)66'000 << (quint16)8'118;
    QTest::addRow("PokitMeter:1MHz")
        << PokitProduct::PokitMeter << (quint32)1'000'000 << (quint32)0 << (quint16)0
        << true << (quint32)8'191 << (quint16)8'191;
    QTest::addRow("PokitPro:1Hz")
        << PokitProduct::PokitPro << (quint32)1 << (quint32)0 << (quint16)0
        << true << (quint32)1'000'000 << (quint16)1;
    QTest::addRow("PokitPro:123kHz")
        << PokitProduct::PokitPro << (quint32)123'000 << (quint32)0 << (quint16)0
        << true << (quint32)133'000 << (quint16)16'359;
    QTest::addRow("PokitPro:1MHz")
        << PokitProduct::PokitPro << (quint32)1'000'000 << (quint32)0 << (quint16)0
        << true << (quint32)16'384 << (quint16)16'384;

    // Sample rates, plus sampling windows.
    QTest::addRow("PokitMeter:1Hz,500ms")
        << PokitProduct::PokitMeter << (quint32)1 << (quint32)500'000 << (quint16)0
        << false << (quint32)500'000 << (quint16)0;
    QTest::addRow("PokitMeter:1Hz,2s")
        << PokitProduct::PokitMeter << (quint32)1 << (quint32)2'000'000 << (quint16)0
        << true << (quint32)2'000'000 << (quint16)2;
    QTest::addRow("PokitMeter:123kHz,456us")
        << PokitProduct::PokitMeter << (quint32)123'000 << (quint32)456 << (quint16)0
        << true << (quint32)456 << (quint16)56;
    QTest::addRow("PokitMeter:1MHz,789us")
        << PokitProduct::PokitMeter << (quint32)1'000'000 << (quint32)789 << (quint16)0
        << true << (quint32)789 << (quint16)789;
    QTest::addRow("PokitPro:1Hz,10ms")
        << PokitProduct::PokitPro << (quint32)1 << (quint32)10 << (quint16)0
        << false << (quint32)10 << (quint16)0;
    QTest::addRow("PokitPro:1Hz,2s")
        << PokitProduct::PokitPro << (quint32)1 << (quint32)2'000'000 << (quint16)0
        << true << (quint32)2'000'000 << (quint16)2;
    QTest::addRow("PokitPro:123kHz,456us")
        << PokitProduct::PokitPro << (quint32)123'000 << (quint32)456 << (quint16)0
        << true << (quint32)456 << (quint16)56;
    QTest::addRow("PokitPro:1MHz,789us")
        << PokitProduct::PokitPro << (quint32)1'000'000 << (quint32)789 << (quint16)0
        << true << (quint32)789 << (quint16)789;

    // Sample rates, plus sampling windows.
    QTest::addRow("PokitMeter:1Hz,123x")
        << PokitProduct::PokitMeter << (quint32)1 << (quint32)0 << (quint16)123
        << true << (quint32)123'000'000 << (quint16)123;
    QTest::addRow("PokitMeter:123kHz,456x")
        << PokitProduct::PokitMeter << (quint32)123'000 << (quint32)0 << (quint16)456
        << true << (quint32)3707 << (quint16)456;
    QTest::addRow("PokitMeter:1MHz,789x")
        << PokitProduct::PokitMeter << (quint32)1'000'000 << (quint32)789 << (quint16)789
        << true << (quint32)789 << (quint16)789;
    QTest::addRow("PokitPro:1Hz,1234x")
        << PokitProduct::PokitPro << (quint32)1 << (quint32)1234 << (quint16)1234
        << true << (quint32)1234 << (quint16)1234;
    QTest::addRow("PokitPro:123kHz,5678x")
        << PokitProduct::PokitPro << (quint32)123'000 << (quint32)5678 << (quint16)5678
        << true << (quint32)5678 << (quint16)5678;
    QTest::addRow("PokitPro:1MHz,9012x")
        << PokitProduct::PokitPro << (quint32)1'000'000 << (quint32)9012 << (quint16)9012
        << true << (quint32)9012 << (quint16)9012;
}

void TestDsoCommand::configureWindow()
{
    QFETCH(PokitProduct, product);
    QFETCH(quint32, sampleRate);
    QFETCH(quint32, samplingWindow);
    QFETCH(quint16, numberOfSamples);
    QFETCH(bool, expectSuccess);
    QFETCH(quint32, expectedSamplingWindow);
    QFETCH(quint16, expectedNumberOfSamples);

    DsoService::Settings settings {
        DsoService::Command::ResendData, 123.45f, DsoService::Mode::AcVoltage,
        +PokitMeter::VoltageRange::_6V, samplingWindow, numberOfSamples
    };
    QCOMPARE(DsoCommand::configureWindow(product, sampleRate, settings), expectSuccess);
    QCOMPARE(settings.command, DsoService::Command::ResendData); // ie not touched.
    QCOMPARE(settings.triggerLevel, 123.45f);                    // ie not touched.
    QCOMPARE(settings.mode, DsoService::Mode::AcVoltage);        // ie not touched.
    QCOMPARE(settings.range, +PokitMeter::VoltageRange::_6V);    // ie not touched.
    QCOMPARE(settings.samplingWindow, expectedSamplingWindow);
    QCOMPARE(settings.numberOfSamples, expectedNumberOfSamples);
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
    if (gitHubActionsRunnerOsVersion() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 14)) {
        QSKIP("BLE controller operations hang on GitHub Actions's macOS 14 runners");
    }

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
        QTest::newRow(qUtf8Printable(name + u".csv"_s)) \
            << metadata << list << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(qUtf8Printable(name + u".json"_s)) \
            << metadata << list << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(qUtf8Printable(name + u".txt"_s)) \
            << metadata << list << AbstractCommand::OutputFormat::Text

    for (const DsoService::Metadata &metadata: metadatas) {
        const QString namePrefix = DsoService::toString(metadata.mode).replace(' '_L1, '-'_L1);
        DOKIT_ADD_TEST_ROW(namePrefix + u"-null"_s,
                             metadata, QList<DsoService::Samples>{ });
        for (const DsoService::Samples &samples: samplesList) {
            DOKIT_ADD_TEST_ROW(namePrefix + QString::number(samples.front()), metadata,
                                 QList<DsoService::Samples>{ samples });
        }
        DOKIT_ADD_TEST_ROW(namePrefix + u"-all"_s, metadata, samplesList);
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
