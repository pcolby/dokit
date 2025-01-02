// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the DeviceInfoServicePrivate class.
 */

#ifndef QTPOKIT_INFOSERVICE_P_H
#define QTPOKIT_INFOSERVICE_P_H

#include <qtpokit/deviceinfoservice.h>

#include "abstractpokitservice_p.h"

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT DeviceInfoServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit DeviceInfoServicePrivate(QLowEnergyController * controller, DeviceInfoService * const q);

protected:
    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;

private:
    Q_DECLARE_PUBLIC(DeviceInfoService)
    Q_DISABLE_COPY(DeviceInfoServicePrivate)
    QTPOKIT_BEFRIEND_TEST(DeviceInfoService)
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_INFOSERVICE_P_H
