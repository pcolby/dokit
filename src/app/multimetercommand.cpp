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

#include "multimetercommand.h"

#include <qtpokit/pokitdevice.h>

#include <QJsonDocument>
#include <QJsonObject>

/*!
 * \class MultimeterCommand
 *
 * The MultimeterCommand class implements the `meter` CLI command.
 */

/*!
 * Construct a new MultimeterCommand object with \a parent.
 */
MultimeterCommand::MultimeterCommand(QObject * const parent) : DeviceCommand(parent), service(nullptr)
{

}

QStringList MultimeterCommand::requiredOptions() const
{
    return DeviceCommand::requiredOptions() + QStringList{
    };
}

QStringList MultimeterCommand::supportedOptions() const
{
    return DeviceCommand::supportedOptions();
}

/*!
 * \copybrief DeviceCommand::processOptions
 *
 * This implementation extends DeviceCommand::processOptions to process additional CLI options
 * supported (or required) by this command.
 */
QStringList MultimeterCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = DeviceCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    return errors;
}

/*!
 * \copybrief DeviceCommand::getService
 *
 * This override returns a pointer to a MultimeterService object.
 */
AbstractPokitService * MultimeterCommand::getService()
{
    Q_ASSERT(device);
    if (!service) {
        service = device->multimeter();
        Q_ASSERT(service);
        connect(service, &MultimeterService::settingsWritten,
                this, &MultimeterCommand::settingsWritten);
        connect(service, &MultimeterService::readingRead,
                this, &MultimeterCommand::outputReading);
    }
    return service;
}

/*!
 * \copybrief DeviceCommand::serviceDetailsDiscovered
 *
 * This override fetches the current device's status, and outputs it in the selected format.
 */
void MultimeterCommand::serviceDetailsDiscovered()
{
    DeviceCommand::serviceDetailsDiscovered(); // Just logs consistently.

    /// \todo
    MultimeterService::Settings settings{
        MultimeterService::MultimeterMode::MultimeterDcVoltage,
        { MultimeterService::VoltageRange::AutoRange },
        1000
    };

    qCInfo(lc).noquote() << tr("Configuring multimeter settings...");
    service->setSettings(settings);
}

/*!
 * Invoked when the multimeter settings have been written, to begin reading the meter values.
 */
void MultimeterCommand::settingsWritten()
{
    qCDebug(lc).noquote() << tr("Settings written; starting meter readings...");
    service->beginClientReadings();
}

/*!
 * Outputs meter \a reading in the selected ouput format.
 */
void MultimeterCommand::outputReading(const MultimeterService::Reading &reading)
{
    /// \todo
    qCInfo(lc) << reading.status << reading.value << (int)reading.mode << (int)reading.range.voltageRange;
}
