// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "models/pokitdevicesmodel.h"

#include "qtpokit/pokitdiscoveryagent.h"

#include <QLoggingCategory>
#include <QMainWindow>

/// \todo Move this file into a widgets folder?

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget * const parent = nullptr, const Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    static Q_LOGGING_CATEGORY(lc, "pokit.gui.mainWindow", QtInfoMsg);

    virtual void closeEvent(QCloseEvent *event) override;

private:
    PokitDiscoveryAgent * discoveryAgent;
    PokitDevicesModel * devicesModel;
};
