// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DOKIT_GUI_POKITDEVICESMODEL_H
#define DOKIT_GUI_POKITDEVICESMODEL_H

#include <QBluetoothDeviceInfo>
#include <QLoggingCategory>
#include <QStandardItemModel>

class PokitDiscoveryAgent;

class PokitDevicesModel : public QStandardItemModel
{
    Q_OBJECT

public:
    enum : int {
        BluetoothAddressRole = Qt::UserRole,
        DeviceUuidRole,
    };

    PokitDevicesModel(QObject * const parent = nullptr);
    void setDiscoveryAgent(const PokitDiscoveryAgent * agent);

protected:
    static Q_LOGGING_CATEGORY(lc, "dokit.model.devices", QtInfoMsg);

protected slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &info);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    void onDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    #endif

};

#endif // DOKIT_GUI_POKITDEVICESMODEL_H
