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
 * Declares the InfoServicePrivate class.
 */

#ifndef QTPOKIT_INFOSERVICE_P_H
#define QTPOKIT_INFOSERVICE_P_H

#include <qtpokit/infoservice.h>

#include "abstractpokitservice_p.h"

QTPOKIT_BEGIN_NAMESPACE

class QTPOKIT_EXPORT InfoServicePrivate : public AbstractPokitServicePrivate
{
    Q_OBJECT

public:
    explicit InfoServicePrivate(QLowEnergyController * controller, InfoService * const q);

protected:
    void characteristicRead(const QLowEnergyCharacteristic &characteristic,
                            const QByteArray &value) override;
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                               const QByteArray &newValue) override;

private:
    Q_DECLARE_PUBLIC(InfoService)
    Q_DISABLE_COPY(InfoServicePrivate)
    friend class TestInfoService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_INFOSERVICE_P_H
