// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "loggerfetchcommand.h"

#include <qtpokit/pokitdevice.h>

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

// Qt 6.5.0 added new QDateTime::fromSecsSinceEpoch() and fromMSecsSinceEpoch()
// overloads, then Qt 6.6.0 deprectated some of of the older ones.
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
    #define DOKIT_QT_UTC Qt::UTC
#else
    #include <QTimeZone>
    #define DOKIT_QT_UTC QTimeZone::UTC
#endif

/*!
 * \class LoggerFetchCommand
 *
 * The LoggerFetchCommand class implements the `logger` CLI command.
 */

/*!
 * Construct a new LoggerFetchCommand object with \a parent.
 */
LoggerFetchCommand::LoggerFetchCommand(QObject * const parent) : DeviceCommand(parent)
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
void LoggerFetchCommand::metadataRead(const DataLoggerService::Metadata &data)
{
    qCDebug(lc) << "status:"          << (int)(data.status);
    qCDebug(lc) << "scale:"           << data.scale;
    qCDebug(lc) << "mode:"            << DataLoggerService::toString(data.mode) << (quint8)data.mode;
    qCDebug(lc) << "range:"           << service->toString(data.range, data.mode) << data.range;
    qCDebug(lc) << "updateInterval:"  << (int)data.updateInterval;
    qCDebug(lc) << "numberOfSamples:" << data.numberOfSamples;
    qCDebug(lc) << "timestamp:"       << data.timestamp << QDateTime::fromSecsSinceEpoch(data.timestamp, DOKIT_QT_UTC);
    this->metadata = data;
    this->samplesToGo = data.numberOfSamples;
    this->timestamp = (quint64)data.timestamp * (quint64)1000;
    qCInfo(lc).noquote() << tr("Fetching %Ln logger sample/s...", nullptr, data.numberOfSamples);
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
    case DataLoggerService::Mode::Temperature: unit = QString::fromUtf8("°C"); break;
    default:
        qCDebug(lc).noquote() << tr(R"(No known unit for mode %1 "%2".)").arg((int)metadata.mode)
            .arg(DataLoggerService::toString(metadata.mode));
    }
    const QString range = service->toString(metadata.range, metadata.mode);

    for (const qint16 &sample: samples) {
        const QString timeString = (metadata.timestamp == 0) ? QString::number(timestamp)
            : QDateTime::fromMSecsSinceEpoch(timestamp, DOKIT_QT_UTC).toString(Qt::ISODateWithMs);
        const float value = sample * metadata.scale;
        switch (format) {
        case OutputFormat::Csv:
            for (; showCsvHeader; showCsvHeader = false) {
                std::cout << qUtf8Printable(tr("timestamp,value,unit,range\n"));
            }
            std::cout << qUtf8Printable(QString::fromLatin1("%1,%2,%3,%4\n")
                .arg(timeString).arg(value).arg(unit, range));
            break;
        case OutputFormat::Json: {
            QJsonObject object{
                { QLatin1String("timestamp"), timeString },
                { QLatin1String("value"),     value },
                { QLatin1String("unit"),      unit },
                { QLatin1String("mode"),      DataLoggerService::toString(metadata.mode) },
            };
            if (!range.isEmpty()) {
                object.insert(QLatin1String("range"), range);
            }
            std::cout << QJsonDocument(object).toJson().toStdString();
        }   break;
        case OutputFormat::Text:
            std::cout << qUtf8Printable(tr("%1 %2 %3\n").arg(timeString).arg(value).arg(unit));
            break;
        }
        timestamp += metadata.updateInterval;
        --samplesToGo;
    }
    if (samplesToGo <= 0) {
        qCInfo(lc).noquote() << tr("Finished fetching %Ln sample/s (with %L1 remaining).",
            nullptr, metadata.numberOfSamples).arg(samplesToGo);
        if (device) disconnect(); // Will exit the application once disconnected.
    }
}
