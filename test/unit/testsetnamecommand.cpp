// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testsetnamecommand.h"

#include "setnamecommand.h"

void TestSetNameCommand::requiredOptions() {
    /// \todo Implement requiredOptions test.
}

void TestSetNameCommand::supportedOptions() {
    /// \todo Implement supportedOptions test.
}

void TestSetNameCommand::processOptions() {
    /// \todo Implement processOptions test.
}

void TestSetNameCommand::getService() {
    /// \todo Implement getService test.
}

void TestSetNameCommand::serviceDetailsDiscovered() {
    /// \todo Implement serviceDetailsDiscovered test.
}

void TestSetNameCommand::deivceNameWritten() {
    /// \todo Implement deivceNameWritten test.
}

void TestSetNameCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    SetNameCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestSetNameCommand)
