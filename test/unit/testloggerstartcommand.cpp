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

void TestLoggerStartCommand::lowestRange() {
    /// \todo Implement lowestRange test.
}

void TestLoggerStartCommand::lowestCurrentRange() {
    /// \todo Implement lowestCurrentRange test.
}

void TestLoggerStartCommand::lowestVoltageRange() {
    /// \todo Implement lowestVoltageRange test.
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
