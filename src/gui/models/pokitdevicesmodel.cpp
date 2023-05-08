// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "pokitdevicesmodel.h"
#include "../resources.h"

#include "qtpokit/pokitdiscoveryagent.h"
#include "qtpokit/pokitproducts.h"

#include <QBluetoothUuid>
#include <QDebug>

PokitDevicesModel::PokitDevicesModel(QObject * const parent) : QStandardItemModel(parent)
{

}

void PokitDevicesModel::setDiscoveryAgent(const PokitDiscoveryAgent * agent)
{
    connect(agent, &PokitDiscoveryAgent::pokitDeviceDiscovered, this, &PokitDevicesModel::onDeviceDiscovered);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    connect(agent, &PokitDiscoveryAgent::pokitDeviceUpdated, this, &PokitDevicesModel::onDeviceUpdated);
    #endif
}

void PokitDevicesModel::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    qCInfo(lc) << "Discovered" << info.deviceUuid() << QIcon::themeName() << QIcon::themeSearchPaths();
    /// \todo Check for existing entries (ie avoid duplicates).
    auto item = new QStandardItem(info.name());
    item->setCheckable(true);
    item->setEditable(false);

    switch (pokitProduct(info)) {
        case PokitProduct::PokitMeter: {
            static QIcon pokitMeterIcon = loadPokitMeterIcon(QStringLiteral("transparent"));
            item->setIcon(pokitMeterIcon);
        }   break;
        case PokitProduct::PokitPro: {
            static QIcon pokitProIcon = loadPokitProIcon(QStringLiteral("gray"));
            item->setIcon(pokitProIcon);
        }   break;
        default: {
            static QIcon pokitLogoIcon = loadPokitLogoIcon();
            item->setIcon(pokitLogoIcon);
        }
    }

    item->setToolTip(info.address().isNull() ? info.deviceUuid().toString() : info.address().toString());
    item->setData(info.address().toUInt64(), BluetoothAddressRole);
    item->setData(info.deviceUuid(), DeviceUuidRole);
    /// \todo plenty of other data.
    appendRow(item);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
void PokitDevicesModel::onDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
{
    qCDebug(lc) << "Updated" << info.deviceUuid() << updatedFields;
    /// \todo 1. Locate the existing item.
    /// \todo 2. Set the updated field/s.
    /// \todo 3. emit item->dataChanged().
}
#endif
