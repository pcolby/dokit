// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "scandialog.h"

#include <QBluetoothUuid>
#include <QDebug>

/// \todo Move this file into a widgets folder?

ScanDialog::ScanDialog(QWidget * const parent, const Qt::WindowFlags flags) : QDialog(parent, flags)
{
    connect(&discoveryAgent, &PokitDiscoveryAgent::pokitDeviceDiscovered, this, &ScanDialog::deviceDiscovered);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    connect(&discoveryAgent, &PokitDiscoveryAgent::pokitDeviceUpdated, this, &ScanDialog::deviceUpdated);
    #endif

    /// \todo Add accept / close buttons.
}

void ScanDialog::showEvent(QShowEvent *event)
{
    discoveryAgent.start();
    QDialog::showEvent(event);
}

void ScanDialog::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    qCInfo(lc) << "Discovered" << info.deviceUuid();
    /// \todo Show the device name (and/or other attributes) in the dialog.
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
void ScanDialog::deviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
{
    qCDebug(lc) << "Updated" << info.deviceUuid() << updatedFields;
}
#endif
