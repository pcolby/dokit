// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTPOKIT_POKITPRODUCTS_P_H
#define QTPOKIT_POKITPRODUCTS_P_H

#include <qtpokit/pokitproducts.h>

#include <QLowEnergyController>

QTPOKIT_BEGIN_NAMESPACE

QTPOKIT_EXPORT bool isPokitProduct(const QList<QBluetoothUuid> &serviceUuids);
QTPOKIT_EXPORT bool isPokitProduct(const QLowEnergyController &controller);

QTPOKIT_EXPORT PokitProduct pokitProduct(const QList<QBluetoothUuid> &serviceUuids);
QTPOKIT_EXPORT PokitProduct pokitProduct(const QLowEnergyController &controller);

namespace CapacitanceRange {
    QTPOKIT_EXPORT QString toString(const PokitProduct product, const quint8 range);
    QTPOKIT_EXPORT QVariant maxValue(const PokitProduct product, const quint8 range);
}

namespace CurrentRange {
    QTPOKIT_EXPORT QString toString(const PokitProduct product, const quint8 range);
    QTPOKIT_EXPORT QVariant maxValue(const PokitProduct product, const quint8 range);
}

namespace ResistanceRange {
    QTPOKIT_EXPORT QString toString(const PokitProduct product, const quint8 range);
    QTPOKIT_EXPORT QVariant maxValue(const PokitProduct product, const quint8 range);
}

namespace VoltageRange {
    QTPOKIT_EXPORT QString toString(const PokitProduct product, const quint8 range);
    QTPOKIT_EXPORT QVariant maxValue(const PokitProduct product, const quint8 range);
}

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITPRODUCTS_P_H
