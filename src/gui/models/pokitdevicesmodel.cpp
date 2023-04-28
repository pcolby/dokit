// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "pokitdevicesmodel.h"

#include "qtpokit/pokitdiscoveryagent.h"

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

QIcon PokitDevicesModel::getPokitMeterIcon()
{
    QIcon icon;
    icon.addFile(QStringLiteral(":/devices/pokit_meter-157x166.png"));
    icon.addFile(QStringLiteral(":/devices/pokit_meter-236x249.png"));
    icon.addFile(QStringLiteral(":/devices/pokit_meter-315x333.png"));
    icon.addFile(QStringLiteral(":/devices/pokit_meter-472x499.png"));
    icon.addFile(QStringLiteral(":/devices/pokit_meter-630x666.png"));
    Q_ASSERT(icon.availableSizes().size() == 5);
    return icon;
}

QIcon PokitDevicesModel::getPokitProIcon()
{
    QIcon icon;
    icon.addFile(QStringLiteral(":/devices/pokit_pro-75x68.png"));
    icon.addFile(QStringLiteral(":/devices/pokit_pro-112x101.png"));
    icon.addFile(QStringLiteral(":/devices/pokit_pro-150x135.png"));
    icon.addFile(QStringLiteral(":/devices/pokit_pro-225x203.png"));
    icon.addFile(QStringLiteral(":/devices/pokit_pro-300x270.png"));
    Q_ASSERT(icon.availableSizes().size() == 5);
    return icon;
}

void PokitDevicesModel::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    static QIcon pokitMeterIcon = getPokitMeterIcon();
    static QIcon pokitProIcon = getPokitProIcon();

    qCInfo(lc) << "Discovered" << info.deviceUuid() << QIcon::themeName() << QIcon::themeSearchPaths();
    /// \todo Check for existing entries (ie avoid duplicates).
    auto item = new QStandardItem(info.name());
    item->setCheckable(true);
    item->setEditable(false);

    if (PokitDiscoveryAgent::isPokitMeter(info)) {
        item->setIcon(pokitMeterIcon);
    } else if (PokitDiscoveryAgent::isPokitPro(info)) {
        item->setIcon(pokitProIcon);
    } else {
        item->setIcon(QIcon(QStringLiteral(":/devices/pokit_pro-150x135.png"))); /// \todo Use our own icons?
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