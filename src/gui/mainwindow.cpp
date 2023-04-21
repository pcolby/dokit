// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "scanwidget.h"

#include "mainwindow.h"

#include <QApplication>
#include <QChartView>
#include <QDockWidget>
#include <QSettings>

/// \todo Move this file into a widgets folder?

MainWindow::MainWindow(QWidget * const parent, const Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    setWindowTitle(tr("%1 %2").arg(QApplication::applicationName(), QApplication::applicationVersion()));

    setCentralWidget(new QChartView);
    setDockOptions(QMainWindow::AnimatedDocks);

    auto * const scanDockWidget = new QDockWidget(tr("Pokit Devices"));
    scanDockWidget->setWidget(new ScanWidget);
    addDockWidget(Qt::RightDockWidgetArea, scanDockWidget);

    // Restore the window's geometry and state.
    QSettings settings;
    settings.beginGroup(QStringLiteral("mainwindow"));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    restoreState(settings.value(QStringLiteral("state")).toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save the window's geometry and state.
    QSettings settings;
    settings.beginGroup(QStringLiteral("mainwindow"));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("state"), saveState());

    // Let the base class accept or ignore the event.
    QMainWindow::closeEvent(event);
}
