// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef POKIT_GUI_POKITDEVICESMODEL_H
#define POKIT_GUI_POKITDEVICESMODEL_H

#include <QBluetoothDeviceInfo>
#include <QLoggingCategory>
#include <QStandardItemModel>

class PokitDiscoveryAgent;

class PokitDevicesModel : public QStandardItemModel
{
    Q_OBJECT

public:
    PokitDevicesModel(QWidget * const parent = nullptr);
    void setDiscoveryAgent(const PokitDiscoveryAgent * agent);

protected:
    static Q_LOGGING_CATEGORY(lc, "pokit.model.devices", QtInfoMsg);

protected slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &info);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    void onDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    #endif
    void onDiscoveryFinished();
};

#endif // POKIT_GUI_POKITDEVICESMODEL_H
