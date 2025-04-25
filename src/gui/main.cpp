// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mainwindow.h"
#include "../stringliterals_p.h"

#include <QApplication>
#include <QDebug>

DOKIT_USE_STRINGLITERALS

int main(int argc, char *argv[])
{
    // Setup the application.
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral(PROJECT_NAME));
    QApplication::setApplicationVersion(QString::fromLatin1(PROJECT_VERSION
        #ifdef PROJECT_PRE_RELEASE
        "-" PROJECT_PRE_RELEASE
        #endif
        #ifdef PROJECT_BUILD_ID
        "+" PROJECT_BUILD_ID
        #endif
    ));
    QApplication::setOrganizationName(u"pcolby"_s);        // Only used for QSettings. We should Manage these via
    QApplication::setOrganizationDomain(u"colby.id.au"_s); // CMake if we ever add settings to the console app too.

    /// \todo Install localised translators, if we have translations for the current locale.

    /// \todo Parse the command line.
    // QLoggingCategory::setFilterRules(u"pokit.*.debug=true"_s);

    // Instantiate the main window.
    MainWindow mainWindow;
    mainWindow.show();
    return QApplication::exec();
}
