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

class LoggerCommand : public DeviceCommand
{
public:
    explicit LoggerCommand(QObject * const parent);

    QStringList requiredOptions() const override;
    QStringList supportedOptions() const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    DataLoggerService * service; ///< Bluetooth service this command interracts with.
    DataLoggerService::Settings settings; ///< Settings for the Pokit device's data logger mode.
    int numberOfSamplesToRead; ///< Number of samples to read, if specified on the CLI.

    DataLoggerService::Range lowestRange(const DataLoggerService::Mode mode, const quint32 desiredMax);
    static DataLoggerService::CurrentRange lowestCurrentRange(const quint32 desiredMax);
    static DataLoggerService::VoltageRange lowestVoltageRange(const quint32 desiredMax);

private slots:
    void settingsWritten();
    void metadataRead(const DataLoggerService::Metadata &metadata);
    void outputSamples(const DataLoggerService::Samples &samples);

    friend class TestLoggerCommand;
};
