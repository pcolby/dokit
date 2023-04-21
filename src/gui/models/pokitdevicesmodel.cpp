// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "pokitdevicesmodel.h"

#include "qtpokit/pokitdiscoveryagent.h"

#include <QBluetoothUuid>
#include <QDebug>

PokitDevicesModel::PokitDevicesModel(QWidget * const parent) : QStandardItemModel(parent)
{

}

void PokitDevicesModel::setDiscoveryAgent(const PokitDiscoveryAgent * agent)
{
    connect(agent, &PokitDiscoveryAgent::pokitDeviceDiscovered, this, &PokitDevicesModel::onDeviceDiscovered);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    connect(agent, &PokitDiscoveryAgent::pokitDeviceUpdated, this, &PokitDevicesModel::onDeviceUpdated);
    #endif
    connect(agent, &PokitDiscoveryAgent::finished, this, &PokitDevicesModel::onDiscoveryFinished);
}

void PokitDevicesModel::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    qCInfo(lc) << "Discovered" << info.deviceUuid() << QIcon::themeName() << QIcon::themeSearchPaths();
    /// \todo Check for existing entries (ie avoid duplicates).
    auto item = new QStandardItem(info.name());
    item->setEditable(false);
    if (PokitDiscoveryAgent::isPokitMeter(info)) {
        item->setIcon(QIcon::fromTheme(QStringLiteral("libreoffice-calc"))); /// \todo Use our own icons?
    } else if (PokitDiscoveryAgent::isPokitPro(info)) {
        item->setIcon(QIcon::fromTheme(QStringLiteral("libreoffice-draw"))); /// \todo Use our own icons?
    } else {
        item->setIcon(QIcon::fromTheme(QStringLiteral("libreoffice-main"))); /// \todo Use our own icons?
    }
    item->setToolTip(info.address().isNull() ? info.deviceUuid().toString() : info.address().toString());
    /// \todo item->setData(info);
    appendRow(item);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
void PokitDevicesModel::onDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
{
    qCDebug(lc) << "Updated" << info.deviceUuid() << updatedFields;
    /// \todo Update the model item.
}
#endif

void PokitDevicesModel::onDiscoveryFinished()
{
//    status->setText(QStringLiteral("Stopped scanning"));
}
