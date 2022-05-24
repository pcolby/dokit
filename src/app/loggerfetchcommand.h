// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/dataloggerservice.h>

class LoggerFetchCommand : public DeviceCommand
{
public:
    explicit LoggerFetchCommand(QObject * const parent);

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    DataLoggerService * service; ///< Bluetooth service this command interracts with.
    DataLoggerService::Metadata metadata; ///< Most recent data logging metadata.
    qint32 samplesToGo; ///< Number of samples we're still expecting to receive.
    quint64 timestamp; ///< Current sample's epoch milliseconds timestamp.

private slots:
    void metadataRead(const DataLoggerService::Metadata &metadata);
    void outputSamples(const DataLoggerService::Samples &samples);

    friend class TestLoggerFetchCommand;
};
