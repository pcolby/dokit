// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testsetnamecommand.h"

#include "setnamecommand.h"

void TestSetNameCommand::test1_data()
{
    QTest::addColumn<int>("input");
    QTest::addColumn<int>("expected");

    QTest::addRow("example") << 1 << 2;
}

void TestSetNameCommand::test1()
{
    QFETCH(int, input);
    QFETCH(int, expected);
    const int actual = input * 2;
    QCOMPARE(actual, expected);
}

QTEST_MAIN(TestSetNameCommand)
