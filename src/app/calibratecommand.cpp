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

#include "calibratecommand.h"

#include <qtpokit/calibrationservice.h>
#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

/*!
 * \class CalibrationCommand
 *
 * The CalibrationCommand class implements the `calibrate` CLI command.
 */

/*!
 * Construct a new CalibrationCommand object with \a parent.
 */
CalibrationCommand::CalibrationCommand(QObject * const parent) : DeviceCommand(parent),
    service(nullptr), temperature(std::numeric_limits<float>::quiet_NaN())
{

}

QStringList CalibrationCommand::requiredOptions() const
{
    return DeviceCommand::requiredOptions() + QStringList{
        QLatin1String("temperature"),
    };
}

QStringList CalibrationCommand::supportedOptions() const
{
    return DeviceCommand::supportedOptions();
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList CalibrationCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = DeviceCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    const QString temperatureString = parser.value(QLatin1String("temperature"));
    bool ok;
    const float temperatureFloat = temperatureString.toFloat(&ok);
    if (ok) {
        temperature = temperatureFloat;
    } else {
        errors.append(tr("Unrecognised temperature format: %1").arg(temperatureString));
    }
    return errors;
}

/*!
 * Begins scanning for Pokit devices.
 */
bool CalibrationCommand::start()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->calibration();
        Q_ASSERT(service);
        connect(service, &CalibrationService::serviceDetailsDiscovered,
                this, &CalibrationCommand::serviceDetailsDiscovered);
        connect(service, &CalibrationService::serviceErrorOccurred,
                this, &CalibrationCommand::serviceError);
        connect(service, &CalibrationService::temperatureCalibrated,
                this, &CalibrationCommand::temperatureCalibrated);
    }
    qCInfo(lc).noquote() << tr("Connecting to device...");
    device->controller()->connectToDevice();
    return true;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override sets the ambient temperature, via the Calibration service.
 */
void CalibrationCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    qCInfo(lc).noquote() << tr("Calibrating temperature at %1 degrees celcius...").arg(temperature);
    if (!service->calibrateTemperature(0)) {
        QCoreApplication::exit(EXIT_FAILURE);
    }
}

/*!
 * Handles CalibrationService::temperatureCalibrated events, by outputting the result and exiting.
 */
void CalibrationCommand::temperatureCalibrated()
{
    switch (format) {
    case OutputFormat::Csv:
        fputs(qPrintable(tr("calibration_result\nsuccess\n")), stdout);
        break;
    case OutputFormat::Json:
        fputs(qPrintable(QLatin1String("true\n")), stdout);
        break;
    case OutputFormat::Text:
        fputs(qPrintable(tr("Done.\n")), stdout);
        break;
    }
    QCoreApplication::quit();
}
