// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTPOKIT_POKITPRODUCTS_H
#define QTPOKIT_POKITPRODUCTS_H

#include "qtpokit_global.h"

#include <QBluetoothDeviceInfo>
#include <QFlags>
#include <QLowEnergyController>
#include <QString>

QTPOKIT_BEGIN_NAMESPACE

enum class PokitProduct : quint8 {
  //PokitClamp = 3, ///< \todo Pokit Clamp
    PokitMeter = 0, ///< Pokit Meter
    PokitPro   = 1, ///< Pokit Pro
};
Q_DECLARE_FLAGS(PokitProducts, PokitProduct)
QTPOKIT_EXPORT QString toString(const PokitProduct product);

bool isPokitProduct(const QBluetoothDeviceInfo &info);
bool isPokitProduct(const QLowEnergyController &controller);

PokitProduct pokitProduct(const QBluetoothDeviceInfo &info);
PokitProduct pokitProduct(const QLowEnergyController &controller);

/// \todo Range conversion functions.

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITPRODUCTS_H