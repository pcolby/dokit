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
    const DataLoggerService::Metadata metadata{
        DataLoggerService::LoggerStatus::Sampling,
        0.123f,
        DataLoggerService::Mode::DcCurrent,
        DataLoggerService::CurrentRange::_300mA_to_3A,
        1000, 1234, (quint32)QDateTime::currentSecsSinceEpoch()
    };
    LoggerFetchCommand command(this);
    QTest::ignoreMessage(QtInfoMsg, "Fetching 1,234 logger samples...");
    command.metadataRead(metadata);
    QCOMPARE(command.metadata.status,          metadata.status);
    QCOMPARE(command.metadata.scale,           metadata.scale);
    QCOMPARE(command.metadata.mode,            metadata.mode);
    QCOMPARE(command.metadata.range,           metadata.range);
    QCOMPARE(command.metadata.updateInterval,  metadata.updateInterval);
    QCOMPARE(command.metadata.numberOfSamples, metadata.numberOfSamples);
    QCOMPARE(command.metadata.timestamp,       metadata.timestamp);
    QCOMPARE(command.samplesToGo,              metadata.numberOfSamples);
    QCOMPARE(command.timestamp,                (quint64)metadata.timestamp * (quint64)1000);
}

void TestLoggerFetchCommand::outputSamples() {
    /// \todo Verify the output format.
}

void TestLoggerFetchCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    LoggerFetchCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestLoggerFetchCommand)
