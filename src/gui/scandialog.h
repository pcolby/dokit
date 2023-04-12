// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QDialog>
#include <QLoggingCategory>

#include <qtpokit/pokitdiscoveryagent.h>

/// \todo Move this file into a widgets folder?

class ScanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScanDialog(QWidget * const parent = nullptr, const Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    static Q_LOGGING_CATEGORY(lc, "pokit.gui.scanDialog", QtInfoMsg);
    PokitDiscoveryAgent discoveryAgent;

    virtual void showEvent(QShowEvent *event) override;

protected slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &info);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    void deviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    #endif

};
