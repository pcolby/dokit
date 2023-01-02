// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testloggerstopcommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

#include "loggerstopcommand.h"

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)

void TestLoggerStopCommand::getService()
{
    // Unable to safely invoke LoggerStopCommand::getService() without a valid Bluetooth device.
}

void TestLoggerStopCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke LoggerStopCommand::serviceDetailsDiscovered() without a valid service.
}

void TestLoggerStopCommand::settingsWritten_data()
{
    QTest::addColumn<AbstractCommand::OutputFormat>("format");
    QTest::newRow("1.csv") << AbstractCommand::OutputFormat::Csv;
    QTest::newRow("1.json") << AbstractCommand::OutputFormat::Json;
    QTest::newRow("1.txt") << AbstractCommand::OutputFormat::Text;
}

void TestLoggerStopCommand::settingsWritten()
{
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    OutputStreamCapture capture(&std::cout);
    LoggerStopCommand command(nullptr);
    command.format = format;
    command.settingsWritten();
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestLoggerStopCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    LoggerStopCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestLoggerStopCommand)
