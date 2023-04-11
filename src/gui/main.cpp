// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    // Setup the application.
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral(PROJECT_NAME));
    app.setApplicationVersion(QString::fromLatin1(PROJECT_VERSION
        #ifdef PROJECT_PRE_RELEASE
        "-" PROJECT_PRE_RELEASE
        #endif
        #ifdef PROJECT_BUILD_ID
        "+" PROJECT_BUILD_ID
        #endif
    ));
    app.setOrganizationName(QStringLiteral("pcolby"));        // Only used for QSettings. We should Manage these via
    app.setOrganizationDomain(QStringLiteral("colby.id.au")); // CMake if we ever add settings to the console app too.

    /// \todo Install localised translators, if we have translations for the current locale.

    /// \todo Parse the command line.

    // Instantiate the main window.
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
