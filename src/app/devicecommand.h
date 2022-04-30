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

class AbstractPokitService;
class PokitDevice;

class DeviceCommand : public AbstractCommand
{
public:
    explicit DeviceCommand(QObject * const parent);

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;
    bool start() override;

protected:
    PokitDevice * device; ///< Pokit Bluetooth device (if any) this command inerracts with.
    virtual AbstractPokitService * getService() = 0;

protected slots:
    virtual void controllerError(const QLowEnergyController::Error error);
    virtual void serviceError(const QLowEnergyService::ServiceError error);
    virtual void serviceDetailsDiscovered();

private slots:
    // These are protected in the base class, but hidden (private) for our descendents.
    void deviceDiscovered(const QBluetoothDeviceInfo &info) override;
    void deviceDiscoveryFinished() override;

    friend class TestDeviceCommand;
};

#endif // QTPOKIT_DEVICECOMMAND_H
