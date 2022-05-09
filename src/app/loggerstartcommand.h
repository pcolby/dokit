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

class LoggerStartCommand : public DeviceCommand
{
public:
    explicit LoggerStartCommand(QObject * const parent);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    DataLoggerService * service; ///< Bluetooth service this command interracts with.
    DataLoggerService::Settings settings; ///< Settings for the Pokit device's data logger mode.

    static DataLoggerService::Range lowestRange(const DataLoggerService::Mode mode,
                                                const quint32 desiredMax);
    static DataLoggerService::CurrentRange lowestCurrentRange(const quint32 desiredMax);
    static DataLoggerService::VoltageRange lowestVoltageRange(const quint32 desiredMax);

private slots:
    void settingsWritten();

    friend class TestLoggerStartCommand;
};
