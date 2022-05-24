// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the GenericAccessServicePrivate class.
 */

#ifndef QTPOKIT_GENERICACCESSSERVICE_P_H
#define QTPOKIT_GENERICACCESSSERVICE_P_H

#include <qtpokit/genericaccessservice.h>

#include "abstractpokitservice_p.h"

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT GenericAccessServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit GenericAccessServicePrivate(QLowEnergyController * controller,
                                         GenericAccessService * const q);

    static quint16 parseAppearance(const QByteArray &value);

protected:
    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;

private:
    Q_DECLARE_PUBLIC(GenericAccessService)
    Q_DISABLE_COPY(GenericAccessServicePrivate)
    friend class TestGenericAccessService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_GENERICACCESSSERVICE_P_H
