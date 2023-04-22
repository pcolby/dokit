// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mainwindow.h"

#include <QApplication>
#include <QChartView>
#include <QDockWidget>
#include <QListView>
#include <QSettings>
#include <QStatusBar>

MainWindow::MainWindow(QWidget * const parent, const Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    setWindowTitle(tr("%1 %2").arg(QApplication::applicationName(), QApplication::applicationVersion()));

    discoveryAgent = new PokitDiscoveryAgent(this);
    devicesModel = new PokitDevicesModel(this);
    devicesModel->setDiscoveryAgent(discoveryAgent);

    setCentralWidget(new QChartView);
    setDockOptions(QMainWindow::AnimatedDocks);

    auto pokitDevicesListView = new QListView(this);
    pokitDevicesListView->setModel(devicesModel);
    auto const scanDockWidget = new QDockWidget(tr("Pokit Devices"));
    scanDockWidget->setObjectName("pokitDevicesDockWidget"); ///< For save/restore state (and presumably geometry).
    scanDockWidget->setWidget(pokitDevicesListView);
    addDockWidget(Qt::RightDockWidgetArea, scanDockWidget);

    // Restore the window's geometry and state.
    QSettings settings;
    settings.beginGroup(QStringLiteral("mainwindow"));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    restoreState(settings.value(QStringLiteral("state")).toByteArray());

    /// \todo Restore the model's default check states.

    // Begin the Pokit device discovery.
    connect(discoveryAgent, &PokitDiscoveryAgent::finished, this, &MainWindow::discoveryFinished);
    statusBar()->showMessage(tr("Scanning for Pokit devices"));
    discoveryAgent->start();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save the window's geometry and state.
    QSettings settings;
    settings.beginGroup(QStringLiteral("mainwindow"));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("state"), saveState());

    /// \todo Save the checked states of devices.

    // Let the base class accept or ignore the event.
    QMainWindow::closeEvent(event);
}

void MainWindow::discoveryFinished()
{
    statusBar()->showMessage(tr("Finishing scanning for Pokit Devices"));
}