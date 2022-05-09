/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    friend class TestDsoService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_DSOSERVICE_P_H
