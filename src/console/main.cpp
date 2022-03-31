/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLoggingCategory>

#include "discover.h"

#if defined(Q_OS_UNIX)
#include <unistd.h>
#elif defined(Q_OS_WIN)
#include <Windows.h>
#endif

inline bool haveConsole()
{
    #if defined(Q_OS_UNIX)
    return isatty(STDERR_FILENO);
    #elif defined(Q_OS_WIN)
    return GetConsoleWindow();
    #else
    return false;
    #endif
}

void configureLogging(const QCommandLineParser &parser)
{
    // Start with the Qt default message pattern (see qtbase:::qlogging.cpp:defaultPattern)
    QString messagePattern = QStringLiteral("%{if-category}%{category}: %{endif}%{message}");

    if (parser.isSet(QStringLiteral("debug"))) {
        messagePattern.prepend(QStringLiteral("%{time process} %{type} %{function} "));
        QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
    }

    const QString color = parser.value(QStringLiteral("color"));
    if ((color == QStringLiteral("yes")) || (color == QStringLiteral("auto") && haveConsole())) {
        messagePattern.prepend(QStringLiteral(
        "%{if-debug}\x1b[37m%{endif}"      // White
        "%{if-info}\x1b[32m%{endif}"       // Green
        "%{if-warning}\x1b[35m%{endif}"    // Magenta
        "%{if-critical}\x1b[31m%{endif}"   // Red
        "%{if-fatal}\x1b[31;1m%{endif}")); // Red and bold
        messagePattern.append(QStringLiteral("\x1b[0m")); // Reset.
    }

    qSetMessagePattern(messagePattern);
}

void parseCommandLine(const QCoreApplication &app, QCommandLineParser &parser)
{
    const QStringList commands = {
        QLatin1String("info"),
        QLatin1String("status"),
        QLatin1String("meter"),
        QLatin1String("dso"),
        QLatin1String("logger"),
        QLatin1String("scan"),
        QLatin1String("set-name"),
        QLatin1String("flash-led"),
    };

    const QStringList appArguments = app.arguments();
    parser.parse(appArguments);
    const QStringList posArguments = parser.positionalArguments();
    QString errorText;
    int commandIndex = -1;
    if (posArguments.size() > 1) {
        errorText = QObject::tr("More than one command: %1").arg(posArguments.join(QStringLiteral(", ")));
    } else if (!posArguments.empty()) {
        commandIndex = commands.indexOf(posArguments.first().toLower());
        if (commandIndex < 0) {
            errorText = QObject::tr("Unknown command: %1").arg(posArguments.first());
        }
    }
    if (!errorText.isEmpty()) {
         // Output the same way QCommandLineParser does (qcommandlineparser.cpp::showParserMessage).
        const QString message = QCoreApplication::applicationName() + QLatin1String(": ")
            + errorText + QLatin1Char('\n');
        fputs(qPrintable(message), stderr);
        ::exit(EXIT_FAILURE);
    }

    parser.addOptions({
        {{QStringLiteral("d"), QStringLiteral("device")},
          QCoreApplication::translate("parseCommandLine", "Pokit device to use"),
          QStringLiteral("name-or-address")}
    });

    // Add common options.
    parser.setApplicationDescription(QStringLiteral(CMAKE_PROJECT_DESCRIPTION));
    parser.addOptions({
        {{QStringLiteral("debug")}, QStringLiteral("Enable debug output")},
        { QStringLiteral("color"),
          QCoreApplication::translate("parseCommandLine","Color the console output (default auto)"),
          QStringLiteral("yes|no|auto"), QStringLiteral("auto")},
    });
    parser.addPositionalArgument(
        QStringLiteral("command"),
        QCoreApplication::translate("parseCommandLine","One of: %1").arg(commands.join(QStringLiteral(", "))),
        QStringLiteral("<command>"));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(appArguments);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral(CMAKE_PROJECT_NAME));
    app.setApplicationVersion(QStringLiteral(CMAKE_PROJECT_VERSION));

    QCommandLineParser parser;
    parseCommandLine(app, parser);
    configureLogging(parser);

    qDebug() << parser.positionalArguments();
    qDebug() << parser.values(QStringLiteral("command"));
    qDebug() << parser.values(QStringLiteral("command2"));

    Discover d(&app);

    return app.exec();
}
