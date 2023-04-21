// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "scanwidget.h"

#include <QBluetoothUuid>
#include <QDebug>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QVBoxLayout>

/// \todo Move this file into a widgets folder?

ScanWidget::ScanWidget(QWidget * const parent, const Qt::WindowFlags flags) : QWidget(parent, flags)
{
    auto listView = new QListView;
    listView->setModel(devicesModel = new QStandardItemModel);
    listView->setSelectionMode(QListView::MultiSelection);

    setLayout(new QVBoxLayout);
    layout()->addWidget(listView);
    layout()->addWidget(status = new QLabel(QStringLiteral("Init")));

    discoveryAgent = new PokitDiscoveryAgent(this);
    connect(discoveryAgent, &PokitDiscoveryAgent::pokitDeviceDiscovered, this, &ScanWidget::onDeviceDiscovered);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    connect(discoveryAgent, &PokitDiscoveryAgent::pokitDeviceUpdated, this, &ScanWidget::onDeviceUpdated);
    #endif
    connect(discoveryAgent, &PokitDiscoveryAgent::finished, this, &ScanWidget::onDiscoveryFinished);
    connect(discoveryAgent, &PokitDiscoveryAgent::errorOccurred, this, &ScanWidget::onDiscoveryError);

    connect(listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ScanWidget::selectionChanged);
}

void ScanWidget::showEvent(QShowEvent *event)
{
    discoveryAgent->start();
    status->setText(QStringLiteral("Scanning..."));
    QWidget::showEvent(event);
}

void ScanWidget::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
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
void ScanWidget::onDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields)
{
    qCDebug(lc) << "Updated" << info.deviceUuid() << updatedFields;
}
#endif

void ScanWidget::onDiscoveryFinished()
{
    status->setText(QStringLiteral("Stopped scanning"));
}

void ScanWidget::onDiscoveryError()
{
    status->setText(discoveryAgent->errorString());
    auto const messageBox = new QMessageBox(QMessageBox::Warning, tr("Bluetooth Error"),
        tr("An error occured while scanning for Pokit devices."), QMessageBox::NoButton, this);
    messageBox->setDetailedText(tr("discoveryAgent->errorString()"));
    messageBox->setModal(true);
    connect(messageBox, &QMessageBox::finished, messageBox, &QMessageBox::deleteLater);
    messageBox->open();
}

void ScanWidget::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    qCInfo(lc) << selected << deselected;
}
