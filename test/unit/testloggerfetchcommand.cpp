// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testloggerfetchcommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

#include "loggerfetchcommand.h"

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(DataLoggerService::Metadata)

void TestLoggerFetchCommand::getService()
{
    // Unable to safely invoke LoggerFetchCommand::getService() without a valid Bluetooth device.
}

void TestLoggerFetchCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke LoggerFetchCommand::serviceDetailsDiscovered() without a valid service.
}

void TestLoggerFetchCommand::metadataRead()
{
    const DataLoggerService::Metadata metadata{
        DataLoggerService::LoggerStatus::Sampling,
        0.123f,
        DataLoggerService::Mode::DcCurrent,
        DataLoggerService::CurrentRange::_300mA_to_3A,
        1000, 1234, (quint32)QDateTime::currentSecsSinceEpoch()
    };
    LoggerFetchCommand command(this);
    QTest::ignoreMessage(QtInfoMsg, "Fetching 1,234 logger sample/s...");
    command.metadataRead(metadata);
    QCOMPARE(command.metadata.status,          metadata.status);
    QCOMPARE(command.metadata.scale,           metadata.scale);
    QCOMPARE(command.metadata.mode,            metadata.mode);
    QCOMPARE(command.metadata.range,           metadata.range);
    QCOMPARE(command.metadata.updateInterval,  metadata.updateInterval);
    QCOMPARE(command.metadata.numberOfSamples, metadata.numberOfSamples);
    QCOMPARE(command.metadata.timestamp,       metadata.timestamp);
    QCOMPARE(command.samplesToGo,              (qint32)metadata.numberOfSamples);
    QCOMPARE(command.timestamp,                (quint64)metadata.timestamp * (quint64)1000);
}

void TestLoggerFetchCommand::outputSamples_data()
{
    QTest::addColumn<DataLoggerService::Metadata>("metadata");
    QTest::addColumn<QList<DataLoggerService::Samples>>("samplesList");
    QTest::addColumn<AbstractCommand::OutputFormat>("format");

    const QList<DataLoggerService::Metadata> metadatas{
        { DataLoggerService::LoggerStatus::Sampling, 1.0f, DataLoggerService::Mode::DcVoltage,
                    DataLoggerService::VoltageRange::_300mV_to_2V, 1000, 0, 1661235278 },
        { DataLoggerService::LoggerStatus::Done, -1.0f, DataLoggerService::Mode::AcVoltage,
                    DataLoggerService::VoltageRange::_300mV_to_2V, 500, 0, 1661235439 },
        { DataLoggerService::LoggerStatus::Error, 0.5f, DataLoggerService::Mode::DcCurrent,
                    DataLoggerService::CurrentRange::_30mA_to_150mA, 100, 0, 0 },
        { DataLoggerService::LoggerStatus::BufferFull, 0.1f, DataLoggerService::Mode::AcCurrent,
                    DataLoggerService::CurrentRange::_300mA_to_3A, 5000, 0, 1661235439 },
    };

    const QList<DataLoggerService::Samples> samplesList{
        {0},
        {1,2,3,4,6,7,8,9,0},
        {2,-32767,0,32767}
    };

    #define DOKIT_ADD_TEST_ROW(name, metadata, list) \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".csv"))) \
            << metadata << list << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".json"))) \
            << metadata << list << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".txt"))) \
            << metadata << list << AbstractCommand::OutputFormat::Text

    for (const DataLoggerService::Metadata &metadata: metadatas) {
        const QString namePrefix = DataLoggerService::toString(metadata.mode)
            .replace(QLatin1Char(' '), QLatin1Char('-'));
        DOKIT_ADD_TEST_ROW(namePrefix + QStringLiteral("-null"),
                             metadata, QList<DataLoggerService::Samples>{ });
        for (const DataLoggerService::Samples &samples: samplesList) {
            DOKIT_ADD_TEST_ROW(namePrefix + QString::number(samples.front()), metadata,
                                 QList<DataLoggerService::Samples>{ samples });
        }
        DOKIT_ADD_TEST_ROW(namePrefix + QStringLiteral("-all"), metadata, samplesList);
    }
    #undef DOKIT_ADD_TEST_ROW
}

void TestLoggerFetchCommand::outputSamples()
{
    QFETCH(DataLoggerService::Metadata, metadata);
    QFETCH(QList<DataLoggerService::Samples>, samplesList);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    const OutputStreamCapture capture(&std::cout);
    LoggerFetchCommand command(nullptr);
    command.metadataRead(metadata);
    command.format = format;
    for (const DataLoggerService::Samples &samples: samplesList) {
        command.outputSamples(samples);
    }
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestLoggerFetchCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    LoggerFetchCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestLoggerFetchCommand)
