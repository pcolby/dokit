// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testmetercommand.h"

#include "metercommand.h"

void TestMeterCommand::requiredOptions() {
    /// \todo Implement requiredOptions test.
}

void TestMeterCommand::supportedOptions() {
    /// \todo Implement supportedOptions test.
}

void TestMeterCommand::processOptions() {
    /// \todo Implement processOptions test.
}

void TestMeterCommand::getService() {
    /// \todo Implement getService test.
}

void TestMeterCommand::serviceDetailsDiscovered() {
    /// \todo Implement serviceDetailsDiscovered test.
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

void TestMeterCommand::settingsWritten() {
    /// \todo Implement settingsWritten test.
}

void TestMeterCommand::outputReading() {
    /// \todo Implement outputReading test.
}

void TestMeterCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    MeterCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestMeterCommand)
