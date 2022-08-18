// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdsocommand.h"

#include "dsocommand.h"

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

void TestDsoCommand::processOptions()
{
    /// \todo Implement processOptions test.
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
            << DsoService::Range{ .voltageRange = DsoService::VoltageRange::range }; \
        QTest::addRow(#value "mVac") \
            << DsoService::Mode::AcVoltage << (quint32)value \
            << DsoService::Range{ .voltageRange = DsoService::VoltageRange::range }
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
    /// \todo Pokit Pro supports up to 600V.
    #undef QTPOKIT_ADD_TEST_ROW

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mAdc") \
            << DsoService::Mode::DcCurrent << (quint32)value \
            << DsoService::Range{ .currentRange = DsoService::CurrentRange::range }; \
        QTest::addRow(#value "mAac") \
            << DsoService::Mode::AcCurrent << (quint32)value \
            << DsoService::Range{ .currentRange = DsoService::CurrentRange::range }
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
    /// \todo Pokit Pro supports up to 10A.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestDsoCommand::lowestRange()
{
    QFETCH(DsoService::Mode, mode);
    QFETCH(quint32, desiredMax);
    QFETCH(DsoService::Range, expected);
    const DsoService::Range actual = DsoCommand::lowestRange(mode, desiredMax);
    switch (mode) {
        case DsoService::Mode::Idle:
            QSKIP("Invoking DsoCommand::lowestRange with DsoService::Mode::Idle is invalid");
            break;
        case DsoService::Mode::DcVoltage:
        case DsoService::Mode::AcVoltage:
            QCOMPARE(actual.voltageRange, expected.voltageRange);
            break;
        case DsoService::Mode::DcCurrent:
        case DsoService::Mode::AcCurrent:
            QCOMPARE(actual.currentRange, expected.currentRange);
            break;
    }
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
        DsoService::Range{ .voltageRange = DsoService::VoltageRange::_300mV_to_2V },
        123456,
        789,
        1234
    };
    command.metadataRead(metadata);
    QCOMPARE(command.metadata.status,             metadata.status);
    QCOMPARE(command.metadata.scale,              metadata.scale);
    QCOMPARE(command.metadata.mode,               metadata.mode);
    QCOMPARE(command.metadata.range.voltageRange, metadata.range.voltageRange);
    QCOMPARE(command.metadata.samplingWindow,     metadata.samplingWindow);
    QCOMPARE(command.metadata.numberOfSamples,    metadata.numberOfSamples);
    QCOMPARE(command.metadata.samplingRate,       metadata.samplingRate);
    QCOMPARE(command.samplesToGo,                 metadata.numberOfSamples);

}

void TestDsoCommand::outputSamples()
{
    /// \todo Verify the output format.
}

QTEST_MAIN(TestDsoCommand)
