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
    quint64 timestamp; ///< Current sample's epoch milliseconds timestamp.

private slots:
    void metadataRead(const DataLoggerService::Metadata &metadata);
    void outputSamples(const DataLoggerService::Samples &samples);

    friend class TestLoggerFetchCommand;
};
