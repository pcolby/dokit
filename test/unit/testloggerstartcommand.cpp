// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testloggerstartcommand.h"

#include "loggerstartcommand.h"

void TestLoggerStartCommand::requiredOptions() {
    /// \todo Implement requiredOptions test.
}

void TestLoggerStartCommand::supportedOptions() {
    /// \todo Implement supportedOptions test.
}

void TestLoggerStartCommand::processOptions() {
    /// \todo Implement processOptions test.
}

void TestLoggerStartCommand::getService() {
    /// \todo Implement getService test.
}

void TestLoggerStartCommand::serviceDetailsDiscovered() {
    /// \todo Implement serviceDetailsDiscovered test.
}

void TestLoggerStartCommand::lowestRange_data()
{
    QTest::addColumn<DataLoggerService::Mode>("mode");
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<DataLoggerService::Range>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mVdc") \
            << DataLoggerService::Mode::DcVoltage << (quint32)value \
            << DataLoggerService::Range(DataLoggerService::VoltageRange::range); \
        QTest::addRow(#value "mVac") \
            << DataLoggerService::Mode::AcVoltage << (quint32)value \
            << DataLoggerService::Range(DataLoggerService::VoltageRange::range)
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
    QTPOKIT_ADD_TEST_ROW(99999, _30V_to_60V); // DataLogger has no 'auto', so use the max range available.
    /// \todo Pokit Pro supports up to 600V.
    #undef QTPOKIT_ADD_TEST_ROW

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mAdc") \
            << DataLoggerService::Mode::DcCurrent << (quint32)value \
            << DataLoggerService::Range(DataLoggerService::CurrentRange::range); \
        QTest::addRow(#value "mAac") \
            << DataLoggerService::Mode::AcCurrent << (quint32)value \
            << DataLoggerService::Range(DataLoggerService::CurrentRange::range)
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

void TestLoggerStartCommand::lowestRange()
{
    QFETCH(DataLoggerService::Mode, mode);
    QFETCH(quint32, desiredMax);
    QFETCH(DataLoggerService::Range, expected);
    QCOMPARE(LoggerStartCommand::lowestRange(mode, desiredMax), expected);
}

void TestLoggerStartCommand::lowestCurrentRange_data()
{
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<DataLoggerService::CurrentRange>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mA") << (quint32)value << DataLoggerService::CurrentRange::range
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

void TestLoggerStartCommand::lowestCurrentRange()
{
    QFETCH(quint32, desiredMax);
    QFETCH(DataLoggerService::CurrentRange, expected);
    QCOMPARE(LoggerStartCommand::lowestCurrentRange(desiredMax), expected);
}

void TestLoggerStartCommand::lowestVoltageRange_data()
{
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<DataLoggerService::VoltageRange>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mV") << (quint32)value << DataLoggerService::VoltageRange::range
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
    QTPOKIT_ADD_TEST_ROW(99999, _30V_to_60V); // DataLogger has no 'auto', so use the max range available.
    /// \todo Pokit Pro supports up to 600V.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestLoggerStartCommand::lowestVoltageRange()
{
    QFETCH(quint32, desiredMax);
    QFETCH(DataLoggerService::VoltageRange, expected);
    QCOMPARE(LoggerStartCommand::lowestVoltageRange(desiredMax), expected);
}

void TestLoggerStartCommand::settingsWritten() {
    /// \todo Implement settingsWritten test.
}

void TestLoggerStartCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    LoggerStartCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestLoggerStartCommand)
