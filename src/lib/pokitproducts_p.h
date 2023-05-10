// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTPOKIT_POKITPRODUCTS_P_H
#define QTPOKIT_POKITPRODUCTS_P_H

#include <qtpokit/pokitproducts.h>

QTPOKIT_BEGIN_NAMESPACE

bool isPokitProduct(const QList<QBluetoothUuid> &serviceUuids);
bool isPokitProduct(const QLowEnergyController &controller);

PokitProduct pokitProduct(QList<QBluetoothUuid> &serviceUuids);
PokitProduct pokitProduct(const QLowEnergyController &controller);

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITPRODUCTS_P_H
