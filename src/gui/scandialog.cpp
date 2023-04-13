// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "scandialog.h"

#include <QBluetoothUuid>
#include <QDialogButtonBox>
#include <QDebug>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QVBoxLayout>

/// \todo Move this file into a widgets folder?

ScanDialog::ScanDialog(QWidget * const parent, const Qt::WindowFlags flags) : QDialog(parent, flags)
{
    setWindowTitle(tr("Open Pokit Device"));
    /// \todo setWindowIcon(...);

    auto listView = new QListView;
    listView->setModel(devicesModel = new QStandardItemModel);

    setLayout(new QVBoxLayout);
    layout()->addWidget(new QLabel(QStringLiteral("Pokit devices:")));
    layout()->addWidget(listView);
    layout()->addWidget(status = new QLabel(QStringLiteral("Init")));

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Open|QDialogButtonBox::Close);
    layout()->addWidget(buttonBox);

    discoveryAgent = new PokitDiscoveryAgent(this);
    connect(discoveryAgent, &PokitDiscoveryAgent::pokitDeviceDiscovered, this, &ScanDialog::onDeviceDiscovered);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    connect(discoveryAgent, &PokitDiscoveryAgent::pokitDeviceUpdated, this, &ScanDialog::onDeviceUpdated);
    #endif
    connect(discoveryAgent, &PokitDiscoveryAgent::finished, this, &ScanDialog::onDiscoveryFinished);
    connect(discoveryAgent, &PokitDiscoveryAgent::errorOccurred, this, &ScanDialog::onDiscoveryError);
}

void ScanDialog::showEvent(QShowEvent *event)
{
    discoveryAgent->start();
    status->setText(QStringLiteral("Scanning..."));
    QDialog::showEvent(event);
}

void ScanDialog::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    qCInfo(lc) << "Discovered" << info.deviceUuid() << QIcon::themeName() << QIcon::themeSearchPaths();
    auto item = new QStandardItem(info.name());
    item->setEditable(false);
    if (PokitDiscoveryAgent::isPokitMeter(info)) {
        item->setIcon(QIcon::fromTheme(QStringLiteral("libreoffice-calc"))); /// \todo Use our own icons?
    } else if (PokitDiscoveryAgent::isPokitPro(info)) {
        item->setIcon(QIcon::fromTheme(QStringLiteral("libreoffice-draw"))); /// \todo Use our own icons?
    } else {
        item->setIcon(QIcon::fromTheme(QStringLiteral("libreoffice-main"))); /// \todo Use our own icons?
    }
    /// \todo item->setData(info);
    devicesModel->appendRow(item);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
void ScanDialog::onDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
{
    qCDebug(lc) << "Updated" << info.deviceUuid() << updatedFields;
}
#endif

void ScanDialog::onDiscoveryFinished()
{
    status->setText(QStringLiteral("Stopped scanning"));
}

void ScanDialog::onDiscoveryError()
{
    status->setText(discoveryAgent->errorString());
    auto const messageBox = new QMessageBox(QMessageBox::Warning, tr("Bluetooth Error"),
        tr("An error occured while scanning for Pokit devices."), QMessageBox::NoButton, this);
    messageBox->setDetailedText(tr("discoveryAgent->errorString()"));
    messageBox->setModal(true);
    connect(messageBox, &QMessageBox::finished, messageBox, &QMessageBox::deleteLater);
    messageBox->open();
}
