// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QLabel>
#include <QListView>
#include <QLoggingCategory>
#include <QItemSelection>
#include <QStandardItemModel>

#include <qtpokit/pokitdiscoveryagent.h>

/// \todo Move this file into a widgets folder?
/// \todo Rename class to PokitDevicesListView or something similar? and move the scanning, and model outside.

class ScanWidget : public QListView
{
    Q_OBJECT

public:
    explicit ScanWidget(QWidget * const parent = nullptr, const Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    static Q_LOGGING_CATEGORY(lc, "pokit.gui.scanWidget", QtInfoMsg);
    QStandardItemModel * devicesModel;
    PokitDiscoveryAgent * discoveryAgent;

    virtual void showEvent(QShowEvent *event) override;

protected slots:
    /// \todo Move these to a PokitDevicesModel class.
    void onDeviceDiscovered(const QBluetoothDeviceInfo &info);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    void onDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    #endif
    void onDiscoveryFinished();
    void onDiscoveryError();
};
