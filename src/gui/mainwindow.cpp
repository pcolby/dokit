// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mainwindow.h"

#include <QApplication>
#include <QChartView>
#include <QDockWidget>
#include <QListView>
#include <QSettings>
#include <QStatusBar>

/// \todo Remove these. Only needed for example code below.
#include <QCategoryAxis>
#include <QChart>
#include <QSplineSeries>

MainWindow::MainWindow(QWidget * const parent, const Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    setWindowIcon(QIcon(QStringLiteral(":/dokit-icon-512.png")));
    setWindowTitle(tr("%1 %2").arg(QApplication::applicationName(), QApplication::applicationVersion()));

    discoveryAgent = new PokitDiscoveryAgent(this);
    devicesModel = new PokitDevicesModel(this);
    devicesModel->setDiscoveryAgent(discoveryAgent);

    /// \todo Remove this block; its just from Qt's multiaxis chart example, and only for visual inspiration currently.
    QChart *chart = new QChart();
    chart->legend()->hide();
    QValueAxis *axisX = new QValueAxis;
    axisX->setTickCount(10);
    chart->addAxis(axisX, Qt::AlignBottom);
    QSplineSeries *series = new QSplineSeries;
    *series << QPointF(1, 5) << QPointF(3.5, 18) << QPointF(4.8, 7.5) << QPointF(10, 2.5);
    chart->addSeries(series);
    QValueAxis *axisY = new QValueAxis;
    axisY->setLinePenColor(series->pen().color());
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    series = new QSplineSeries;
    *series << QPointF(1, 0.5) << QPointF(1.5, 4.5) << QPointF(2.4, 2.5) << QPointF(4.3, 12.5)
            << QPointF(5.2, 3.5) << QPointF(7.4, 16.5) << QPointF(8.3, 7.5) << QPointF(10, 17);
    chart->addSeries(series);
    QCategoryAxis *axisY3 = new QCategoryAxis;
    axisY3->append(QStringLiteral("Low"), 5);
    axisY3->append(QStringLiteral("Medium"), 12);
    axisY3->append(QStringLiteral("High"), 17);
    axisY3->setLinePenColor(series->pen().color());
    axisY3->setGridLinePen((series->pen()));
    chart->addAxis(axisY3, Qt::AlignRight);
    series->attachAxis(axisX);
    series->attachAxis(axisY3);
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    // End of sample code to delete.

    setCentralWidget(chartView);
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
