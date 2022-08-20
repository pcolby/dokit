// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testloggerfetchcommand.h"

#include "loggerfetchcommand.h"

void TestLoggerFetchCommand::getService() {
    // Unable to safely invoke LoggerFetchCommand::getService() without a valid Bluetooth device.
}

void TestLoggerFetchCommand::serviceDetailsDiscovered() {
    // Unable to safely invoke LoggerFetchCommand::serviceDetailsDiscovered() without a valid service.
}

void TestLoggerFetchCommand::metadataRead() {
    /// \todo Implement metadataRead test.
}

void TestLoggerFetchCommand::outputSamples() {
    /// \todo Implement outputSamples test.
}

void TestLoggerFetchCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    LoggerFetchCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestLoggerFetchCommand)
