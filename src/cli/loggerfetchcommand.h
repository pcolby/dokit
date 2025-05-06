// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/dataloggerservice.h>

class LoggerFetchCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit LoggerFetchCommand(QObject * const parent = nullptr);

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    DataLoggerService * service { nullptr }; ///< Bluetooth service this command interacts with.
    DataLoggerService::Metadata metadata;    ///< Most recent data logging metadata.
    qint32 samplesToGo { 0 };    ///< Number of samples we're still expecting to receive.
    quint64 timestamp { 0 };     ///< Current sample's epoch milliseconds timestamp.
    bool showCsvHeader { true }; ///< Whether or not to show a header as the first line of CSV output.

private slots:
    void metadataRead(const DataLoggerService::Metadata &data);
    void outputSamples(const DataLoggerService::Samples &samples);

    QTPOKIT_BEFRIEND_TEST(LoggerFetchCommand)
};
