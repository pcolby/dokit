// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the MultimeterServicePrivate class.
 */

#ifndef QTPOKIT_MULTIMETERSERVICE_P_H
#define QTPOKIT_MULTIMETERSERVICE_P_H

#include <qtpokit/multimeterservice.h>

#include "abstractpokitservice_p.h"

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT MultimeterServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit MultimeterServicePrivate(QLowEnergyController * controller, MultimeterService * const q);

    static QByteArray encodeSettings(const MultimeterService::Settings &settings);

    static MultimeterService::Reading parseReading(const QByteArray &value);

protected:
    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;

private:
    Q_DECLARE_PUBLIC(MultimeterService)
    Q_DISABLE_COPY(MultimeterServicePrivate)
    QTPOKIT_BEFRIEND_TEST(MultimeterService)
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_MULTIMETERSERVICE_P_H
