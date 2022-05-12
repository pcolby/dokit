/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "loggerfetchcommand.h"

#include <qtpokit/pokitdevice.h>

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

/*!
 * \class LoggerFetchCommand
 *
 * The LoggerFetchCommand class implements the `logger` CLI command.
 */

/*!
 * Construct a new LoggerFetchCommand object with \a parent.
 */
LoggerFetchCommand::LoggerFetchCommand(QObject * const parent)
    : DeviceCommand(parent), service(nullptr)
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
//        connect(service, &DataLoggerService::settingsWritten,
//                this, &LoggerFetchCommand::settingsWritten);
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

//    service->beginMetadata();
//    service->beginSampling();
    service->fetchSamples();

//    for (int i = 1000; i < 20000; i+=5000) {
//        QTimer::singleShot(i, this, [this](){
//            service->fetchSamples();
//        });
//    }
}

/*!
 * Invoked when the data logger settings have been written, to begin reading samples.
 */
//void LoggerFetchCommand::settingsWritten()
//{
//    qCDebug(lc).noquote() << tr("Settings written; starting data logger...");
//    connect(service, &DataLoggerService::metadataRead, this, &LoggerFetchCommand::metadataRead);
//    connect(service, &DataLoggerService::samplesRead, this, &LoggerFetchCommand::outputSamples);
//    service->beginMetadata();
//    service->beginSampling();
//}

/*!
 * Invoked when \a metadata has been received from the data logger.
 *
 * \todo Presumably we need to at least record the metadata.scale here. Probably also need the
 * metadata.numberOfSamples. Of course, log and check for errors etc. too.
 */
void LoggerFetchCommand::metadataRead(const DataLoggerService::Metadata &metadata)
{
    /// \todo Remember the metadata.
    Q_UNUSED(metadata);
    qCDebug(lc) << "status:" << (int)(metadata.status);
    qCDebug(lc) << "scale:" << metadata.scale;
    qCDebug(lc) << "mode:" << DataLoggerService::toString(metadata.mode) << (quint8)metadata.mode;
    qCDebug(lc) << "range:" << DataLoggerService::toString(metadata.range.voltageRange)
                            << (quint8)metadata.range.voltageRange;
    qCDebug(lc) << "updateInterval:" << (int)metadata.updateInterval;
    qCDebug(lc) << "numberOfSamples:" << metadata.numberOfSamples;
    qCDebug(lc) << "timestamp:" << metadata.timestamp
                                << QDateTime::fromSecsSinceEpoch(metadata.timestamp);
}

/*!
 * Outputs logger \a samples in the selected ouput format.
 */
void LoggerFetchCommand::outputSamples(const DataLoggerService::Samples &samples)
{
    /// \todo Output the samples.
    Q_UNUSED(samples);
    qCDebug(lc) << "samplesRead";
}
