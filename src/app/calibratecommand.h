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

class CalibrationService;

class CalibrationCommand : public DeviceCommand
{
public:
    explicit CalibrationCommand(QObject * const parent);

    QStringList requiredOptions() const override;
    QStringList supportedOptions() const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    CalibrationService * service; ///< Bluetooth service this command interracts with.
    float temperature; ///< Ambient temperature from the CLI options.

private slots:
    void temperatureCalibrated();

    friend class TestCalibrateCommand;
};
