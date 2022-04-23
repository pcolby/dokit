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

#ifndef QTPOKIT_DEVICECOMMAND_H
#define QTPOKIT_DEVICECOMMAND_H

#include "abstractcommand.h"

#include <QLowEnergyController>

class PokitDevice;

class DeviceCommand : public AbstractCommand
{
public:
    explicit DeviceCommand(QObject * const parent);

    QStringList requiredOptions() const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    PokitDevice * device; ///< Pokit Bluetooth device (if any) this command inerracts with.

protected slots:
    virtual void controllerError(const QLowEnergyController::Error error);
    virtual void serviceError(const QLowEnergyService::ServiceError error);
    virtual void serviceDetailsDiscovered();

};

#endif // QTPOKIT_DEVICECOMMAND_H
