// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the DsoServicePrivate class.
 */

#ifndef QTPOKIT_DSOSERVICE_P_H
#define QTPOKIT_DSOSERVICE_P_H

#include <qtpokit/dsoservice.h>

#include "abstractpokitservice_p.h"

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT DsoServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit DsoServicePrivate(QLowEnergyController * controller, DsoService * const q);

    static QByteArray encodeSettings(const DsoService::Settings &settings);

    static DsoService::Metadata parseMetadata(const QByteArray &value);
    static DsoService::Samples parseSamples(const QByteArray &value);

protected:
    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;

private:
    Q_DECLARE_PUBLIC(DsoService)
    Q_DISABLE_COPY(DsoServicePrivate)
    QTPOKIT_BEFRIEND_TEST(DsoService)
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_DSOSERVICE_P_H
