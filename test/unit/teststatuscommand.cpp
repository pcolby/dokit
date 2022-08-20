// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "teststatuscommand.h"

#include "statuscommand.h"

void TestStatusCommand::requiredOptions() {
    /// \todo Implement requiredOptions test.
}

void TestStatusCommand::supportedOptions() {
    /// \todo Implement supportedOptions test.
}

void TestStatusCommand::processOptions() {
    /// \todo Implement processOptions test.
}

void TestStatusCommand::getService() {
    /// \todo Implement getService test.
}

void TestStatusCommand::serviceDetailsDiscovered() {
    /// \todo Implement serviceDetailsDiscovered test.
}

void TestStatusCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    StatusCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestStatusCommand)
