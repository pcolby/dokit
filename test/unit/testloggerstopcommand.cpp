// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testloggerstopcommand.h"

#include "loggerstopcommand.h"

void TestLoggerStopCommand::getService() {
    /// \todo Implement getService test.
}

void TestLoggerStopCommand::serviceDetailsDiscovered() {
    /// \todo Implement serviceDetailsDiscovered test.
}

void TestLoggerStopCommand::settingsWritten() {
    /// \todo Implement settingsWritten test.
}

void TestLoggerStopCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    LoggerStopCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestLoggerStopCommand)
