// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the StatusServicePrivate class.
 */

#ifndef QTPOKIT_STATUSSERVICE_P_H
#define QTPOKIT_STATUSSERVICE_P_H

#include <qtpokit/statusservice.h>

#include "abstractpokitservice_p.h"

#include <optional>

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT StatusServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit StatusServicePrivate(QLowEnergyController * controller, StatusService * const q);

    static StatusService::DeviceCharacteristics parseDeviceCharacteristics(const QByteArray &value);
    static StatusService::Status parseStatus(const QByteArray &value);
    static std::optional<StatusService::TorchStatus> parseTorchStatus(const QByteArray &value);
    static std::optional<StatusService::ButtonStatus> parseButtonPress(const QByteArray &value);

protected:
    void serviceDiscovered(const QBluetoothUuid &newService) override;

    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;

private:
    Q_DECLARE_PUBLIC(StatusService)
    Q_DISABLE_COPY(StatusServicePrivate)
    friend class TestStatusService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_STATUSSERVICE_P_H
