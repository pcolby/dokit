// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testmetercommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

#include "metercommand.h"

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(MultimeterService::Mode);
Q_DECLARE_METATYPE(MultimeterService::VoltageRange);
Q_DECLARE_METATYPE(MultimeterService::CurrentRange);
Q_DECLARE_METATYPE(MultimeterService::ResistanceRange);
Q_DECLARE_METATYPE(MultimeterService::Range);
Q_DECLARE_METATYPE(MultimeterService::Reading);
Q_DECLARE_METATYPE(MultimeterService::Settings);

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

void TestMeterCommand::requiredOptions()
{
    MeterCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) +
        QStringList{ QStringLiteral("mode") };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestMeterCommand::supportedOptions()
{
    MeterCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser) +
        QStringList{ QStringLiteral("interval"), QStringLiteral("range"), QStringLiteral("samples") };
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestMeterCommand::processOptions_data()
{
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<MultimeterService::Settings>("expected");
    QTest::addColumn<int>("expectedSamples");
    QTest::addColumn<QStringList>("errors");

    QTest::addRow("missing-required-mode")
        << QStringList{
           QStringLiteral("--range"), QStringLiteral("100")}
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ QStringLiteral("Missing required option: mode") };

    QTest::addRow("Vdc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::_300mV_to_2V, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("Vac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vac"),
           QStringLiteral("--range"), QStringLiteral("5V") }
        << MultimeterService::Settings{
            MultimeterService::Mode::AcVoltage, MultimeterService::VoltageRange::_2V_to_6V, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("Adc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Adc"),
           QStringLiteral("--range"), QStringLiteral("100mA") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcCurrent, MultimeterService::CurrentRange::_30mA_to_150mA, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("Aac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Aac"),
           QStringLiteral("--range"), QStringLiteral("2A") }
        << MultimeterService::Settings{
            MultimeterService::Mode::AcCurrent, MultimeterService::CurrentRange::_300mA_to_3A, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("Resistance")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("res") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Resistance, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("Diode")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("dio") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Diode, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("Continuity")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("cont") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Continuity, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("Temperature")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("temp") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Temperature, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ };

    /// \todo Pokit Pro supports capacitance too.

    QTest::addRow("invalid-mode")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("invalid") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ QStringLiteral("Unknown meter mode: invalid") };

    QTest::addRow("interval:100") // Defaults to 100 seconds (ie 100,000ms).
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("100") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 100000}
        << -1
        << QStringList{ };

    QTest::addRow("interval:499") // Defaults to 499 seconds.
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("499") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 499000}
        << -1
        << QStringList{ };

    QTest::addRow("interval:500") // Defaults to 500ms.
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("500") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 500}
        << -1
        << QStringList{ };

    QTest::addRow("invalid-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("invalid") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ QStringLiteral("Invalid interval value: invalid") };

    QTest::addRow("negative-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("-123") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ QStringLiteral("Invalid interval value: -123") };

    QTest::addRow("auto-range-voltage")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("auto") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("auto-range-current")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Adc"),
           QStringLiteral("--range"), QStringLiteral("auto") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcCurrent, MultimeterService::CurrentRange::AutoRange, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("auto-range-resistance")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("resis"),
           QStringLiteral("--range"), QStringLiteral("auto") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Resistance, MultimeterService::ResistanceRange::AutoRange, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("invalid-range")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("invalid") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ QStringLiteral("Invalid range value: invalid") };

    QTest::addRow("rangeless-range")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("diode"),
           QStringLiteral("--range"), QStringLiteral("1000") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Diode, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ };

    QTest::addRow("samples:100")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--samples"), QStringLiteral("100") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 1000}
        << 100
        << QStringList{ };

    QTest::addRow("invald-samples")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--samples"), QStringLiteral("invalid") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ QStringLiteral("Invalid samples value: invalid") };

    QTest::addRow("negative-samples")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--samples"), QStringLiteral("-123") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, MultimeterService::VoltageRange::AutoRange, 1000}
        << -1
        << QStringList{ QStringLiteral("Invalid samples value: -123") };
}

void TestMeterCommand::processOptions()
{
    QFETCH(QStringList, arguments);
    QFETCH(MultimeterService::Settings, expected);
    QFETCH(int, expectedSamples);
    QFETCH(QStringList, errors);

    arguments.prepend(QStringLiteral("pokit")); // The first argument is always the app name.

    QCommandLineParser parser;
    parser.addOption({QStringLiteral("mode"), QStringLiteral("description"), QStringLiteral("mode")});
    parser.addOption({QStringLiteral("range"), QStringLiteral("description"), QStringLiteral("range")});
    parser.addOption({QStringLiteral("interval"), QStringLiteral("description"), QStringLiteral("interval")});
    parser.addOption({QStringLiteral("samples"), QStringLiteral("description"), QStringLiteral("samples")});
    parser.process(arguments);

    MeterCommand command(this);
    QCOMPARE(command.processOptions(parser),  errors);
    QCOMPARE(command.settings.mode,           expected.mode);
    QCOMPARE(command.settings.range,          expected.range);
    QCOMPARE(command.settings.updateInterval, expected.updateInterval);
    QCOMPARE(command.samplesToGo,             expectedSamples);
}

void TestMeterCommand::getService()
{
    // Unable to safely invoke MeterCommand::getService() without a valid Bluetooth device.
}

void TestMeterCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke MeterCommand::serviceDetailsDiscovered() without a valid service.
}

void TestMeterCommand::lowestRange_data()
{
    QTest::addColumn<MultimeterService::Mode>("mode");
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<MultimeterService::Range>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mVdc") \
            << MultimeterService::Mode::DcVoltage << (quint32)value \
            << MultimeterService::Range(MultimeterService::VoltageRange::range); \
        QTest::addRow(#value "mVac") \
            << MultimeterService::Mode::AcVoltage << (quint32)value \
            << MultimeterService::Range(MultimeterService::VoltageRange::range)
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
    QTPOKIT_ADD_TEST_ROW(59000, _30V_to_60V);
    QTPOKIT_ADD_TEST_ROW(60000, _30V_to_60V);
    /// \todo Pokit Pro supports up to 600V.
    QTPOKIT_ADD_TEST_ROW(60001, AutoRange);
    QTPOKIT_ADD_TEST_ROW(99999, AutoRange);
    #undef QTPOKIT_ADD_TEST_ROW

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mAdc") \
            << MultimeterService::Mode::DcCurrent << (quint32)value \
            << MultimeterService::Range(MultimeterService::CurrentRange::range); \
        QTest::addRow(#value "mAac") \
            << MultimeterService::Mode::AcCurrent << (quint32)value \
            << MultimeterService::Range(MultimeterService::CurrentRange::range)
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
    QTPOKIT_ADD_TEST_ROW(2999, _300mA_to_3A);
    QTPOKIT_ADD_TEST_ROW(3000, _300mA_to_3A);
    /// \todo Pokit Pro supports up to 10A.
    QTPOKIT_ADD_TEST_ROW(3001, AutoRange);
    QTPOKIT_ADD_TEST_ROW(9999, AutoRange);
    #undef QTPOKIT_ADD_TEST_ROW

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
    QTest::addRow(#value "ohms") \
        << MultimeterService::Mode::Resistance << (quint32)value \
        << MultimeterService::Range(MultimeterService::ResistanceRange::range)
    QTPOKIT_ADD_TEST_ROW(      0, _0_to_160);
    QTPOKIT_ADD_TEST_ROW(    159, _0_to_160);
    QTPOKIT_ADD_TEST_ROW(    160, _0_to_160);
    QTPOKIT_ADD_TEST_ROW(    161, _160_to_330);
    QTPOKIT_ADD_TEST_ROW(    329, _160_to_330);
    QTPOKIT_ADD_TEST_ROW(    330, _160_to_330);
    QTPOKIT_ADD_TEST_ROW(    331, _330_to_890);
    QTPOKIT_ADD_TEST_ROW(    889, _330_to_890);
    QTPOKIT_ADD_TEST_ROW(    890, _330_to_890);
    QTPOKIT_ADD_TEST_ROW(    891, _890_to_1K5);
    QTPOKIT_ADD_TEST_ROW(   1499, _890_to_1K5);
    QTPOKIT_ADD_TEST_ROW(   1500, _890_to_1K5);
    QTPOKIT_ADD_TEST_ROW(   1501, _1K5_to_10K);
    QTPOKIT_ADD_TEST_ROW(   9999, _1K5_to_10K);
    QTPOKIT_ADD_TEST_ROW(  10000, _1K5_to_10K);
    QTPOKIT_ADD_TEST_ROW(  10001, _10K_to_100K);
    QTPOKIT_ADD_TEST_ROW(  99999, _10K_to_100K);
    QTPOKIT_ADD_TEST_ROW( 100000, _10K_to_100K);
    QTPOKIT_ADD_TEST_ROW( 100001, _100K_to_470K);
    QTPOKIT_ADD_TEST_ROW( 469999, _100K_to_470K);
    QTPOKIT_ADD_TEST_ROW( 470000, _100K_to_470K);
    QTPOKIT_ADD_TEST_ROW( 470001, _470K_to_1M);
    QTPOKIT_ADD_TEST_ROW( 999999, _470K_to_1M);
    QTPOKIT_ADD_TEST_ROW(1000000, _470K_to_1M);
    /// \todo Pokit Pro supports up to 3M ohms.
    QTPOKIT_ADD_TEST_ROW(1000001, AutoRange);
    QTPOKIT_ADD_TEST_ROW(9999999, AutoRange);
    #undef QTPOKIT_ADD_TEST_ROW

    // The remaining modes are all unsupported (so always return *::AutoRange, aka 255).
    #define QTPOKIT_ADD_TEST_ROW(mode) \
        QTest::addRow(#mode) \
            << MultimeterService::Mode::mode << (quint32)0 \
            << MultimeterService::Range(MultimeterService::VoltageRange::AutoRange)
    QTPOKIT_ADD_TEST_ROW(Idle);
    QTPOKIT_ADD_TEST_ROW(Diode);
    QTPOKIT_ADD_TEST_ROW(Continuity);
    QTPOKIT_ADD_TEST_ROW(Temperature);
    /// \todo Pokit Pro supports capacitance too.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestMeterCommand::lowestRange()
{
    QFETCH(MultimeterService::Mode, mode);
    QFETCH(quint32, desiredMax);
    QFETCH(MultimeterService::Range, expected);
    if ((mode != MultimeterService::Mode::DcVoltage) &&
        (mode != MultimeterService::Mode::AcVoltage) &&
        (mode != MultimeterService::Mode::DcCurrent) &&
        (mode != MultimeterService::Mode::AcCurrent) &&
        (mode != MultimeterService::Mode::Resistance))
    {
        QTest::ignoreMessage(QtWarningMsg, "Mode does not support range.");
    }
    QCOMPARE(MeterCommand::lowestRange(mode, desiredMax), expected);
}

void TestMeterCommand::lowestCurrentRange_data()
{
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<MultimeterService::CurrentRange>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mA") << (quint32)value << MultimeterService::CurrentRange::range
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
    QTPOKIT_ADD_TEST_ROW(2999, _300mA_to_3A);
    QTPOKIT_ADD_TEST_ROW(3000, _300mA_to_3A);
    /// \todo Pokit Pro supports up to 10A.
    QTPOKIT_ADD_TEST_ROW(3001, AutoRange);
    QTPOKIT_ADD_TEST_ROW(9999, AutoRange);
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestMeterCommand::lowestCurrentRange()
{
    QFETCH(quint32, desiredMax);
    QFETCH(MultimeterService::CurrentRange, expected);
    QCOMPARE(MeterCommand::lowestCurrentRange(desiredMax), expected);
}

void TestMeterCommand::lowestResistanceRange_data()
{
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<MultimeterService::ResistanceRange>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "ohms") << (quint32)value << MultimeterService::ResistanceRange::range
    QTPOKIT_ADD_TEST_ROW(      0, _0_to_160);
    QTPOKIT_ADD_TEST_ROW(    159, _0_to_160);
    QTPOKIT_ADD_TEST_ROW(    160, _0_to_160);
    QTPOKIT_ADD_TEST_ROW(    161, _160_to_330);
    QTPOKIT_ADD_TEST_ROW(    329, _160_to_330);
    QTPOKIT_ADD_TEST_ROW(    330, _160_to_330);
    QTPOKIT_ADD_TEST_ROW(    331, _330_to_890);
    QTPOKIT_ADD_TEST_ROW(    889, _330_to_890);
    QTPOKIT_ADD_TEST_ROW(    890, _330_to_890);
    QTPOKIT_ADD_TEST_ROW(    891, _890_to_1K5);
    QTPOKIT_ADD_TEST_ROW(   1499, _890_to_1K5);
    QTPOKIT_ADD_TEST_ROW(   1500, _890_to_1K5);
    QTPOKIT_ADD_TEST_ROW(   1501, _1K5_to_10K);
    QTPOKIT_ADD_TEST_ROW(   9999, _1K5_to_10K);
    QTPOKIT_ADD_TEST_ROW(  10000, _1K5_to_10K);
    QTPOKIT_ADD_TEST_ROW(  10001, _10K_to_100K);
    QTPOKIT_ADD_TEST_ROW(  99999, _10K_to_100K);
    QTPOKIT_ADD_TEST_ROW( 100000, _10K_to_100K);
    QTPOKIT_ADD_TEST_ROW( 100001, _100K_to_470K);
    QTPOKIT_ADD_TEST_ROW( 469999, _100K_to_470K);
    QTPOKIT_ADD_TEST_ROW( 470000, _100K_to_470K);
    QTPOKIT_ADD_TEST_ROW( 470001, _470K_to_1M);
    QTPOKIT_ADD_TEST_ROW( 999999, _470K_to_1M);
    QTPOKIT_ADD_TEST_ROW(1000000, _470K_to_1M);
    /// \todo Pokit Pro supports up to 3M ohms.
    QTPOKIT_ADD_TEST_ROW(1000001, AutoRange);
    QTPOKIT_ADD_TEST_ROW(9999999, AutoRange);
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestMeterCommand::lowestResistanceRange()
{
    QFETCH(quint32, desiredMax);
    QFETCH(MultimeterService::ResistanceRange, expected);
    QCOMPARE(MeterCommand::lowestResistanceRange(desiredMax), expected);
}

void TestMeterCommand::lowestVoltageRange_data()
{
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<MultimeterService::VoltageRange>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mV") << (quint32)value << MultimeterService::VoltageRange::range
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
    QTPOKIT_ADD_TEST_ROW(59000, _30V_to_60V);
    QTPOKIT_ADD_TEST_ROW(60000, _30V_to_60V);
    /// \todo Pokit Pro supports up to 600V.
    QTPOKIT_ADD_TEST_ROW(60001, AutoRange);
    QTPOKIT_ADD_TEST_ROW(99999, AutoRange);
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestMeterCommand::lowestVoltageRange()
{
    QFETCH(quint32, desiredMax);
    QFETCH(MultimeterService::VoltageRange, expected);
    QCOMPARE(MeterCommand::lowestVoltageRange(desiredMax), expected);
}

void TestMeterCommand::settingsWritten()
{
    // Unable to safely invoke MeterCommand::settingsWritten() without a valid Bluetooth service.
}

void TestMeterCommand::outputReading_data()
{
    QTest::addColumn<QList<MultimeterService::Reading>>("readings");
    QTest::addColumn<AbstractCommand::OutputFormat>("format");

    const QList<MultimeterService::Reading> readings{
        { MultimeterService::MeterStatus::Ok, 0.0f, MultimeterService::Mode::Idle,
              MultimeterService::VoltageRange::AutoRange },
        { MultimeterService::MeterStatus::AutoRangeOn, 1.0f, MultimeterService::Mode::DcVoltage,
              MultimeterService::VoltageRange::_300mV_to_2V },
        { MultimeterService::MeterStatus::AutoRangeOff, -1.0f, MultimeterService::Mode::AcVoltage,
              MultimeterService::VoltageRange::_300mV_to_2V },
        { MultimeterService::MeterStatus::AutoRangeOn, 0.5f, MultimeterService::Mode::DcCurrent,
              MultimeterService::CurrentRange::_30mA_to_150mA },
        { MultimeterService::MeterStatus::AutoRangeOff, 0.1f, MultimeterService::Mode::AcCurrent,
              MultimeterService::CurrentRange::_300mA_to_3A },
        { MultimeterService::MeterStatus::AutoRangeOff, 123.4f, MultimeterService::Mode::Resistance,
              MultimeterService::CurrentRange::_300mA_to_3A },
        { MultimeterService::MeterStatus::AutoRangeOff, 576.89f, MultimeterService::Mode::Diode,
              MultimeterService::CurrentRange::_300mA_to_3A },
        { MultimeterService::MeterStatus::Continuity, 0.0f, MultimeterService::Mode::Continuity,
              MultimeterService::CurrentRange::AutoRange },
        { MultimeterService::MeterStatus::NoContinuity, 0.0f, MultimeterService::Mode::Continuity,
              MultimeterService::CurrentRange::AutoRange },
        { MultimeterService::MeterStatus::AutoRangeOff, 32.0f, MultimeterService::Mode::Temperature,
              MultimeterService::CurrentRange::AutoRange },
        /// \todo Pokit Pro supports capacitance too.
        { MultimeterService::MeterStatus::Error, 0.0f, MultimeterService::Mode::Idle,
              MultimeterService::VoltageRange::AutoRange },
    };

    #define QTPOKIT_ADD_TEST_ROW(name, readings) \
        QTest::newRow(qPrintable(name + QStringLiteral(".csv"))) \
            << readings << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(qPrintable(name + QStringLiteral(".json"))) \
            << readings << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(qPrintable(name + QStringLiteral(".txt"))) \
            << readings << AbstractCommand::OutputFormat::Text

    QTPOKIT_ADD_TEST_ROW(QStringLiteral("null"),
                         QList<MultimeterService::Reading>{ MultimeterService::Reading() });

    for (const MultimeterService::Reading &reading: readings) {
        QString name = MultimeterService::toString(reading.mode)
            .replace(QLatin1Char(' '), QLatin1Char('-'));
        if ((reading.mode == MultimeterService::Mode::Continuity) &&
            (!((quint8)reading.status & (quint8)MultimeterService::MeterStatus::Continuity))) {
            name.prepend(QStringLiteral("No")); // ie NoContinuity.
        }
        if (reading.status == MultimeterService::MeterStatus::Error) {
            name = QStringLiteral("Error");
        }
        QTPOKIT_ADD_TEST_ROW(name, QList<MultimeterService::Reading>{ reading });
    }

    QTPOKIT_ADD_TEST_ROW(QStringLiteral("all"), readings);
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestMeterCommand::outputReading()
{
    QFETCH(QList<MultimeterService::Reading>, readings);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    const OutputStreamCapture capture(&std::cout);
    MeterCommand command(nullptr);
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
    MeterCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestMeterCommand)
