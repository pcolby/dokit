// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "calibratecommand.h"

#include <qtpokit/calibrationservice.h>
#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

/*!
 * \class CalibrateCommand
 *
 * The CalibrateCommand class implements the `calibrate` CLI command.
 */

/*!
 * Construct a new CalibrateCommand object with \a parent.
 */
CalibrateCommand::CalibrateCommand(QObject * const parent) : DeviceCommand(parent),
    service(nullptr), temperature(std::numeric_limits<float>::quiet_NaN())
{

}

QStringList CalibrateCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::requiredOptions(parser) + QStringList{
        QLatin1String("temperature"),
    };
}

QStringList CalibrateCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return DeviceCommand::supportedOptions(parser);
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList CalibrateCommand::processOptions(const QCommandLineParser &parser)
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
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a CalibrationService object.
 */
AbstractPokitService * CalibrateCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->calibration();
        Q_ASSERT(service);
        connect(service, &CalibrationService::temperatureCalibrated,
                this, &CalibrateCommand::temperatureCalibrated);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override sets the ambient temperature, via the Calibration service.
 */
void CalibrateCommand::serviceDetailsDiscovered()
{
    Q_ASSERT(service);
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.
    qCInfo(lc).noquote() << tr("Calibrating temperature at %1 degrees celcius...").arg(temperature);
    if (!service->calibrateTemperature(0)) {
        QCoreApplication::exit(EXIT_FAILURE);
    }
}

/*!
 * Handles CalibrationService::temperatureCalibrated events, by outputting the result and exiting.
 */
void CalibrateCommand::temperatureCalibrated()
{
    switch (format) {
    case OutputFormat::Csv:
        std::cout << qPrintable(tr("calibration_result\nsuccess\n"));
        break;
    case OutputFormat::Json:
        std::cout << qPrintable(QLatin1String("true\n"));
        break;
    case OutputFormat::Text:
        std::cout << qPrintable(tr("Done.\n"));
        break;
    }
    if (device) disconnect(); // Will exit the application once disconnected.
}
