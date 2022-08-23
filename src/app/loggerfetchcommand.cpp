// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "loggerfetchcommand.h"

#include <qtpokit/pokitdevice.h>

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

/*!
 * \class LoggerFetchCommand
 *
 * The LoggerFetchCommand class implements the `logger` CLI command.
 */

/*!
 * Construct a new LoggerFetchCommand object with \a parent.
 */
LoggerFetchCommand::LoggerFetchCommand(QObject * const parent)
    : DeviceCommand(parent), service(nullptr), showCsvHeader(true)
{

}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a DataLoggerService object.
 */
AbstractPokitService * LoggerFetchCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->dataLogger();
        Q_ASSERT(service);
        connect(service, &DataLoggerService::metadataRead, this, &LoggerFetchCommand::metadataRead);
        connect(service, &DataLoggerService::samplesRead, this, &LoggerFetchCommand::outputSamples);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override fetches the current device's status, and outputs it in the selected format.
 */
void LoggerFetchCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    qCInfo(lc).noquote() << tr("Fetching logger samples...");
    service->enableMetadataNotifications();
    service->enableReadingNotifications();
    service->fetchSamples();
}

/*!
 * Invoked when \a metadata has been received from the data logger.
 */
void LoggerFetchCommand::metadataRead(const DataLoggerService::Metadata &metadata)
{
    qCDebug(lc) << "status:" << (int)(metadata.status);
    qCDebug(lc) << "scale:" << metadata.scale;
    qCDebug(lc) << "mode:" << DataLoggerService::toString(metadata.mode) << (quint8)metadata.mode;
    qCDebug(lc) << "range:" << DataLoggerService::toString(metadata.range.voltageRange)
                            << (quint8)metadata.range.voltageRange;
    qCDebug(lc) << "updateInterval:" << (int)metadata.updateInterval;
    qCDebug(lc) << "numberOfSamples:" << metadata.numberOfSamples;
    qCDebug(lc) << "timestamp:" << metadata.timestamp
                                << QDateTime::fromSecsSinceEpoch(metadata.timestamp, Qt::UTC);
    this->metadata = metadata;
    this->samplesToGo = metadata.numberOfSamples;
    this->timestamp = (quint64)metadata.timestamp * (quint64)1000;
    qCInfo(lc).noquote() << tr("Fetching %L1 logger samples...").arg(metadata.numberOfSamples);
}

/*!
 * Outputs logger \a samples in the selected ouput format.
 */
void LoggerFetchCommand::outputSamples(const DataLoggerService::Samples &samples)
{
    QString unit;
    switch (metadata.mode) {
    case DataLoggerService::Mode::DcVoltage: unit = QLatin1String("Vdc"); break;
    case DataLoggerService::Mode::AcVoltage: unit = QLatin1String("Vac"); break;
    case DataLoggerService::Mode::DcCurrent: unit = QLatin1String("Adc"); break;
    case DataLoggerService::Mode::AcCurrent: unit = QLatin1String("Aac"); break;
    default:
        qCDebug(lc).noquote() << tr("No known unit for mode %1 \"%2\".").arg((int)metadata.mode)
            .arg(DataLoggerService::toString(metadata.mode));
    }
    const QString range = DataLoggerService::toString(metadata.range, metadata.mode);

    for (const qint16 &sample: samples) {
        const QString timeString = (metadata.timestamp == 0) ? QString::number(timestamp)
            : QDateTime::fromMSecsSinceEpoch(timestamp, Qt::UTC).toString(Qt::ISODateWithMs);
        const float value = sample * metadata.scale;
        switch (format) {
        case OutputFormat::Csv:
            for (; showCsvHeader; showCsvHeader = false) {
                std::cout << qPrintable(tr("timestamp,value,unit,range\n"));
            }
            std::cout << qPrintable(QString::fromLatin1("%1,%2,%3,%4\n")
                .arg(timeString).arg(value).arg(unit, range));
            break;
        case OutputFormat::Json:
            std::cout << QJsonDocument(QJsonObject{
                    { QLatin1String("timestamp"), timeString },
                    { QLatin1String("value"),     value },
                    { QLatin1String("unit"),      unit },
                    { QLatin1String("range"),     range },
                    { QLatin1String("mode"),      DataLoggerService::toString(metadata.mode) },
                }).toJson().toStdString();
            break;
        case OutputFormat::Text:
            std::cout << qPrintable(tr("%1 %2 %3\n").arg(timeString).arg(value).arg(unit));
            break;
        }
        timestamp += metadata.updateInterval;
        --samplesToGo;
    }
    if (samplesToGo <= 0) {
        qCInfo(lc).noquote() << tr("Finished fetching %L1 samples (with %L2 to remaining).")
            .arg(metadata.numberOfSamples).arg(samplesToGo);
        if (device) disconnect(); // Will exit the application once disconnected.
    }
}
