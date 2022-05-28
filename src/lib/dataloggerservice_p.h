// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the DataLoggerServicePrivate class.
 */

#ifndef QTPOKIT_DATALOGGERSERVICE_P_H
#define QTPOKIT_DATALOGGERSERVICE_P_H

#include <qtpokit/dataloggerservice.h>

#include "abstractpokitservice_p.h"

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT DataLoggerServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit DataLoggerServicePrivate(QLowEnergyController * controller, DataLoggerService * const q);

    static QByteArray encodeSettings(const DataLoggerService::Settings &settings, const bool pokitPro);

    static DataLoggerService::Metadata parseMetadata(const QByteArray &value);
    static DataLoggerService::Samples parseSamples(const QByteArray &value);

protected:
    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;

private:
    Q_DECLARE_PUBLIC(DataLoggerService)
    Q_DISABLE_COPY(DataLoggerServicePrivate)
    friend class TestDataLoggerService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_DATALOGGERSERVICE_P_H
