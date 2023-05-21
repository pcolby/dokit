// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DOKIT_DEVICECOMMAND_H
#define DOKIT_DEVICECOMMAND_H

#include "abstractcommand.h"
#include "qtpokit/pokitproducts.h"

#include <QLowEnergyController>

class AbstractPokitService;
class PokitDevice;

class DeviceCommand : public AbstractCommand
{
    Q_OBJECT

public:
    explicit DeviceCommand(QObject * const parent = nullptr);

public slots:
    bool start() override;

protected:
    PokitDevice * device { nullptr }; ///< Pokit Bluetooth device (if any) this command inerracts with.
    int exitCodeOnDisconnect { EXIT_FAILURE }; ///< Exit code to return on device disconnection.

    void disconnect(int exitCode=EXIT_SUCCESS);
    virtual AbstractPokitService * getService() = 0;

    template<typename T> static T minRange(const quint32 maxValue);
    static quint8 minCapacitanceRange(const PokitProduct product, const quint32 maxValue);
    static quint8 minCurrentRange(const PokitProduct product, const quint32 maxValue);
    static quint8 minResistanceRange(const PokitProduct product, const quint32 maxValue);
    static quint8 minVoltageRange(const PokitProduct product, const quint32 maxValue);

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

#endif // DOKIT_DEVICECOMMAND_H
