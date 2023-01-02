// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    bool start() override;

protected:
    PokitDevice * device; ///< Pokit Bluetooth device (if any) this command inerracts with.
    int exitCodeOnDisconnect; ///< Exit code to return on device disconnection.

    void disconnect(int exitCode=EXIT_SUCCESS);
    virtual AbstractPokitService * getService() = 0;

protected slots:
    virtual void controllerError(const QLowEnergyController::Error error);
    virtual void deviceDisconnected();
    virtual void serviceError(const QLowEnergyService::ServiceError error);
    virtual void serviceDetailsDiscovered();

private slots:
    // These are protected in the base class, but hidden (private) for our descendents.
    void deviceDiscovered(const QBluetoothDeviceInfo &info) override;
    void deviceDiscoveryFinished() override;

    friend class TestDeviceCommand;
};

#endif // QTPOKIT_DEVICECOMMAND_H
