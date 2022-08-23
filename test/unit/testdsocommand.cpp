// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdsocommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

#include "dsocommand.h"

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(DsoService::Mode);
Q_DECLARE_METATYPE(DsoService::VoltageRange);
Q_DECLARE_METATYPE(DsoService::CurrentRange);
Q_DECLARE_METATYPE(DsoService::Range);
Q_DECLARE_METATYPE(DsoService::Settings);
Q_DECLARE_METATYPE(DsoService::Metadata);

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
    QTest::addColumn<DsoService::Settings>("expected");
    QTest::addColumn<QStringList>("errors");

    QTest::addRow("missing-required-options")
        << QStringList{}
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_30V_to_60V, 1000*1000, 1000}
        << QStringList{
            QStringLiteral("Missing required option: mode"),
            QStringLiteral("Missing required option: range") };

    QTest::addRow("missing-required-mode")
        << QStringList{
           QStringLiteral("--range"), QStringLiteral("100")}
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_30V_to_60V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Missing required option: mode") };

    QTest::addRow("missing-required-range")
        << QStringList{
           QStringLiteral("--mode"), QStringLiteral("Vdc") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_30V_to_60V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Missing required option: range") };

    QTest::addRow("Vdc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("Vac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vac"),
           QStringLiteral("--range"), QStringLiteral("5V") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::AcVoltage,
            DsoService::VoltageRange::_2V_to_6V, 1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("Adc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Adc"),
           QStringLiteral("--range"), QStringLiteral("100mA") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcCurrent,
            DsoService::CurrentRange::_30mA_to_150mA, 1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("Aac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Aac"),
           QStringLiteral("--range"), QStringLiteral("2A") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::AcCurrent,
            DsoService::CurrentRange::_300mA_to_3A, 1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("invalid-mode")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("invalid"),
           QStringLiteral("--range"), QStringLiteral("123") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_30V_to_60V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Unknown DSO mode: invalid") };

    QTest::addRow("invalid-range")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("invalid") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_30V_to_60V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Invalid range value: invalid") };

    QTest::addRow("trigger-level-requires-trigger-mode")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.123f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ QStringLiteral(
            "If either option is provided, then both must be: trigger-level, trigger-mode") };

    QTest::addRow("trigger-mode-requires-trigger-level")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-mode"), QStringLiteral("free") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
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
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("rise")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("rise") }
        << DsoService::Settings{
            DsoService::Command::RisingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("rising")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("rising") }
        << DsoService::Settings{
            DsoService::Command::RisingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("fall")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("fall") }
        << DsoService::Settings{
            DsoService::Command::FallingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("falling")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("falling") }
        << DsoService::Settings{
            DsoService::Command::FallingEdgeTrigger, 0.123f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("invalid-trigger-level")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("invalid"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("free") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Invalid trigger-level value: invalid") };

    QTest::addRow("invalid-trigger-mode")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--trigger-level"), QStringLiteral("123mV"),
           QStringLiteral("--trigger-mode"),  QStringLiteral("invalid") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.123f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Unknown trigger mode: invalid") };

    QTest::addRow("interval:100") // Defaults to 100 seconds (ie 100,000,000us).
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("100") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 100*1000*1000, 1000}
        << QStringList{ };

    QTest::addRow("interval:499999") // Defaults to 499.999 seconds.
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("499999") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 499999000, 1000}
        << QStringList{ };

    QTest::addRow("interval:500000") // Defaults to 0.5 seconds (ie 500,000us).
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("500000") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 500*1000, 1000}
        << QStringList{ };

    QTest::addRow("invalid-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("invalid") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Invalid interval value: invalid") };

    QTest::addRow("negative-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("-123") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Invalid interval value: -123") };

    QTest::addRow("samples:100")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--samples"), QStringLiteral("100") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 100}
        << QStringList{ };

    QTest::addRow("invald-samples")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--samples"), QStringLiteral("invalid") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Invalid samples value: invalid") };

    QTest::addRow("negative-samples")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--samples"), QStringLiteral("-123") }
        << DsoService::Settings{
            DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
            DsoService::VoltageRange::_300mV_to_2V, 1000*1000, 1000}
        << QStringList{ QStringLiteral("Invalid samples value: -123") };
}

void TestDsoCommand::processOptions()
{
    QFETCH(QStringList, arguments);
    QFETCH(DsoService::Settings, expected);
    QFETCH(QStringList, errors);

    arguments.prepend(QStringLiteral("pokit")); // The first argument is always the app name.

    QCommandLineParser parser;
    parser.addOption({QStringLiteral("mode"), QStringLiteral("description"), QStringLiteral("mode")});
    parser.addOption({QStringLiteral("range"), QStringLiteral("description"), QStringLiteral("range")});
    parser.addOption({QStringLiteral("trigger-level"), QStringLiteral("description"), QStringLiteral("trigger-level")});
    parser.addOption({QStringLiteral("trigger-mode"), QStringLiteral("description"), QStringLiteral("trigger-mode")});
    parser.addOption({QStringLiteral("interval"), QStringLiteral("description"), QStringLiteral("interval")});
    parser.addOption({QStringLiteral("samples"), QStringLiteral("description"), QStringLiteral("samples")});
    parser.process(arguments);

    DsoCommand command(this);
    QCOMPARE(command.processOptions(parser),   errors);
    QCOMPARE(command.settings.command,         expected.command);
    QCOMPARE(command.settings.triggerLevel,    expected.triggerLevel);
    QCOMPARE(command.settings.mode,            expected.mode);
    QCOMPARE(command.settings.range,           expected.range);
    QCOMPARE(command.settings.samplingWindow,  expected.samplingWindow);
    QCOMPARE(command.settings.numberOfSamples, expected.numberOfSamples);
}

void TestDsoCommand::getService()
{
    // Unable to safely invoke DsoCommand::getService() without a valid Bluetooth device.
}

void TestDsoCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke DsoCommand::serviceDetailsDiscovered() without a valid service.
}

void TestDsoCommand::lowestRange_data()
{
    QTest::addColumn<DsoService::Mode>("mode");
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<DsoService::Range>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mVdc") \
            << DsoService::Mode::DcVoltage << (quint32)value \
            << DsoService::Range(DsoService::VoltageRange::range); \
        QTest::addRow(#value "mVac") \
            << DsoService::Mode::AcVoltage << (quint32)value \
            << DsoService::Range(DsoService::VoltageRange::range)
    QTPOKIT_ADD_TEST_ROW(    0, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  299, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  300, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  301, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 1999, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 2000, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 2001, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 5999, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 6000, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 6001, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(11999, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(12000, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(12001, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(29999, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(30000, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(30001, _30V_to_60V);
    QTPOKIT_ADD_TEST_ROW(99999, _30V_to_60V); // DSO has no 'auto', so use the max range available.
    /// \todo Pokit Pro supports up to 600V.
    #undef QTPOKIT_ADD_TEST_ROW

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mAdc") \
            << DsoService::Mode::DcCurrent << (quint32)value \
            << DsoService::Range(DsoService::CurrentRange::range); \
        QTest::addRow(#value "mAac") \
            << DsoService::Mode::AcCurrent << (quint32)value \
            << DsoService::Range(DsoService::CurrentRange::range)
    QTPOKIT_ADD_TEST_ROW(   0, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(   9, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(  10, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(  11, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  29, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  30, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  31, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 149, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 150, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 151, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 299, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 300, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 301, _300mA_to_3A);
    QTPOKIT_ADD_TEST_ROW(9999, _300mA_to_3A);
    /// \todo Pokit Pro supports up to 10A.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestDsoCommand::lowestRange()
{
    QFETCH(DsoService::Mode, mode);
    QFETCH(quint32, desiredMax);
    QFETCH(DsoService::Range, expected);
    QCOMPARE(DsoCommand::lowestRange(mode, desiredMax), expected);
}

void TestDsoCommand::lowestCurrentRange_data()
{
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<DsoService::CurrentRange>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mA") << (quint32)value << DsoService::CurrentRange::range
    QTPOKIT_ADD_TEST_ROW(   0, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(   9, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(  10, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(  11, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  29, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  30, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  31, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 149, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 150, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 151, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 299, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 300, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 301, _300mA_to_3A);
    QTPOKIT_ADD_TEST_ROW(9999, _300mA_to_3A);
    /// \todo Pokit Pro supports up to 10A.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestDsoCommand::lowestCurrentRange()
{
    QFETCH(quint32, desiredMax);
    QFETCH(DsoService::CurrentRange, expected);
    QCOMPARE(DsoCommand::lowestCurrentRange(desiredMax), expected);
}

void TestDsoCommand::lowestVoltageRange_data()
{
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<DsoService::VoltageRange>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mV") << (quint32)value << DsoService::VoltageRange::range
    QTPOKIT_ADD_TEST_ROW(    0, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  299, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  300, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  301, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 1999, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 2000, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 2001, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 5999, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 6000, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 6001, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(11999, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(12000, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(12001, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(29999, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(30000, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(30001, _30V_to_60V);
    QTPOKIT_ADD_TEST_ROW(99999, _30V_to_60V); // DSO has no 'auto', so use the max range available.
    /// \todo Pokit Pro supports up to 600V.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestDsoCommand::lowestVoltageRange()
{
    QFETCH(quint32, desiredMax);
    QFETCH(DsoService::VoltageRange, expected);
    QCOMPARE(DsoCommand::lowestVoltageRange(desiredMax), expected);
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
        DsoService::Range(DsoService::VoltageRange::_300mV_to_2V),
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
                    DsoService::VoltageRange::_300mV_to_2V, 1000, 0, 1 },
        { DsoService::DsoStatus::Done, -1.0f, DsoService::Mode::AcVoltage,
                    DsoService::VoltageRange::_300mV_to_2V, 500, 0, 1000 },
        { DsoService::DsoStatus::Error, 0.5f, DsoService::Mode::DcCurrent,
                    DsoService::CurrentRange::_30mA_to_150mA, 100, 0, 1000000 },
        { DsoService::DsoStatus::Sampling, 0.1f, DsoService::Mode::AcCurrent,
                    DsoService::CurrentRange::_300mA_to_3A, 5000, 0, 1000000000 },
    };

    const QList<DsoService::Samples> samplesList{
        {0},
        {1,2,3,4,6,7,8,9,0},
        {2,-32767,0,32767}
    };

    #define QTPOKIT_ADD_TEST_ROW(name, metadata, list) \
        QTest::newRow(qPrintable(name + QStringLiteral(".csv"))) \
            << metadata << list << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(qPrintable(name + QStringLiteral(".json"))) \
            << metadata << list << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(qPrintable(name + QStringLiteral(".txt"))) \
            << metadata << list << AbstractCommand::OutputFormat::Text

    for (const DsoService::Metadata &metadata: metadatas) {
        const QString namePrefix = DsoService::toString(metadata.mode)
            .replace(QLatin1Char(' '), QLatin1Char('-'));
        QTPOKIT_ADD_TEST_ROW(namePrefix + QStringLiteral("-null"),
                             metadata, QList<DsoService::Samples>{ });
        for (const DsoService::Samples &samples: samplesList) {
            QTPOKIT_ADD_TEST_ROW(namePrefix + QString::number(samples.front()), metadata,
                                 QList<DsoService::Samples>{ samples });
        }
        QTPOKIT_ADD_TEST_ROW(namePrefix + QStringLiteral("-all"), metadata, samplesList);
    }
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestDsoCommand::outputSamples()
{
    QFETCH(DsoService::Metadata, metadata);
    QFETCH(QList<DsoService::Samples>, samplesList);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    const OutputStreamCapture capture(&std::cout);
    DsoCommand command(nullptr);
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
    DsoCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestDsoCommand)
