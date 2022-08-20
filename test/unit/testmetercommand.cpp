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

void TestMeterCommand::lowestRange() {
    /// \todo Implement lowestRange test.
}

void TestMeterCommand::lowestCurrentRange() {
    /// \todo Implement lowestCurrentRange test.
}

void TestMeterCommand::lowestResistanceRange() {
    /// \todo Implement lowestResistanceRange test.
}

void TestMeterCommand::lowestVoltageRange() {
    /// \todo Implement lowestVoltageRange test.
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
