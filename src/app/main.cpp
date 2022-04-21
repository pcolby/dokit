/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include "infocommand.h"
#include "scancommand.h"

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
        #ifdef QT_MESSAGELOGCONTEXT
        // %{file}, %{line} and %{function} are only available when QT_MESSAGELOGCONTEXT is set.
        messagePattern.prepend(QStringLiteral("%{function} "));
        #endif
        messagePattern.prepend(QStringLiteral("%{time process} %{type} "));
        QLoggingCategory::setFilterRules(QStringLiteral("pokit.*.debug=true"));
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

enum class Command {
    None, Info, Status, Meter, DSO, Logger, Scan, SetName, FlashLed
};

void showCliError(const QString &errorText) {
    // Output the same way QCommandLineParser does (qcommandlineparser.cpp::showParserMessage).
    const QString message = QCoreApplication::applicationName() + QLatin1String(": ")
        + errorText + QLatin1Char('\n');
    fputs(qPrintable(message), stderr);
}

Command getCliCommand(const QStringList &posArguments) {
    if (posArguments.isEmpty()) {
        return Command::None;
    }
    if (posArguments.size() > 1) {
        showCliError(QObject::tr("More than one command: %1").arg(posArguments.join(QStringLiteral(", "))));
        ::exit(EXIT_FAILURE);
    }

    const QMap<QString, Command> supportedCommands {
        { QStringLiteral("info"),      Command::Info },
        { QStringLiteral("status"),    Command::Status },
        { QStringLiteral("meter"),     Command::Meter },
        { QStringLiteral("dso"),       Command::DSO },
        { QStringLiteral("logger"),    Command::Logger },
        { QStringLiteral("scan"),      Command::Scan },
        { QStringLiteral("set-name"),  Command::SetName },
        { QStringLiteral("flash-led"), Command::FlashLed },
    };
    const Command command = supportedCommands.value(posArguments.first().toLower(), Command::None);
    if (command == Command::None) {
        showCliError(QObject::tr("Unknown command: %1").arg(posArguments.first()));
        ::exit(EXIT_FAILURE);
    }
    return command;
}

Command parseCommandLine(const QStringList &appArguments, QCommandLineParser &parser)
{
    // Setupt the command line options.
    parser.addOptions({
        {{QStringLiteral("debug")},
          QCoreApplication::translate("parseCommandLine", "Enable debug output.")},
        {{QStringLiteral("d"), QStringLiteral("device")},
          QCoreApplication::translate("parseCommandLine", "Name or address of Pokit device(s) to use."),
          QStringLiteral("name|addr")},
        { QStringLiteral("color"),
          QCoreApplication::translate("parseCommandLine", "Color the console output (default auto)."),
          QStringLiteral("yes|no|auto"), QStringLiteral("auto")},
    });
    parser.addHelpOption();
    parser.addOptions({
        {{QStringLiteral("interval")},
          QCoreApplication::translate("parseCommandLine","Update interval for meter and logger modes"),
          QStringLiteral("n[m][s]")},
        {{QStringLiteral("mode")},
          QCoreApplication::translate("parseCommandLine","@todo Needs more work."),
          QStringLiteral("AC|DC")},
        {{QStringLiteral("new-name")},
          QCoreApplication::translate("parseCommandLine","New name for Pokit device."),
          QStringLiteral("name")},
        {{QStringLiteral("output")},
          QCoreApplication::translate("parseCommandLine","Format for output (default text)."),
          QStringLiteral("text|json"), QStringLiteral("text")},
        {{QStringLiteral("range")},
          QCoreApplication::translate("parseCommandLine","Desired measurement range (default auto)."),
          QStringLiteral("auto|n[m|K|M][V|A|O[hms]"), QStringLiteral("auto")},
        {{QStringLiteral("samples")},
          QCoreApplication::translate("parseCommandLine","Number of samples to acquire"),
          QStringLiteral("samples")},
        {{QStringLiteral("timeout")},
          QCoreApplication::translate("parseCommandLine","@todo"),
          QStringLiteral("seconds")},
        {{QStringLiteral("trigger-level")},
          QCoreApplication::translate("parseCommandLine","Level to trigger DSO acquisition."),
          QStringLiteral("n[m][V|A]")},
        {{QStringLiteral("trigger-mode")},
          QCoreApplication::translate("parseCommandLine","Mode to trigger DSO acquisition (default free)."),
          QStringLiteral("free|rising|falling"), QStringLiteral("free")},
    });
    parser.addVersionOption();
    parser.addOptions({
        {{QStringLiteral("window")},
          QCoreApplication::translate("parseCommandLine","Sampling window for DSO acquisition"),
          QStringLiteral("duration")},
    });

    // Add supported 'commands' (as positional arguments, so they'll appear in the help text).
    parser.addPositionalArgument(QStringLiteral("info"),
        QCoreApplication::translate("parseCommandLine", "Get Pokit device information"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("status"),
        QCoreApplication::translate("parseCommandLine", "Get Pokit device status"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("meter"),
        QCoreApplication::translate("parseCommandLine", "Access Pokit device's multimeter mode"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("dso"),
        QCoreApplication::translate("parseCommandLine", "Access Pokit device's DSO mode"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("logger"),
        QCoreApplication::translate("parseCommandLine", "Access Pokit device's data logger mode"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("scan"),
        QCoreApplication::translate("parseCommandLine", "Scan Bluetooth for Pokit devices"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("set-name"),
        QCoreApplication::translate("parseCommandLine", "Set Pokit device's name"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("flash-led"),
        QCoreApplication::translate("parseCommandLine", "Flash Pokit device's LED"),
        QStringLiteral(" "));

    // Do the initial parse, the see if we have a command specified yet.
    parser.parse(appArguments);
    const Command command = getCliCommand(parser.positionalArguments());

    // If we have a (single, valid) command, then remove the commands list from the help text.
    if (command != Command::None) {
        parser.clearPositionalArguments();
    }

    // Handle -h|--help explicitly, so we can tweak the output to include the <command> info.
    if (parser.isSet(QStringLiteral("help"))) {
        const QString commandString = (command == Command::None) ? QStringLiteral("<command>")
                : parser.positionalArguments().constFirst();
        fputs(qPrintable(parser.helpText()
            .replace(QStringLiteral("[options]"), commandString + QStringLiteral(" [options]"))
            .replace(QStringLiteral("Arguments:"),  QStringLiteral("Command:"))
        ), stdout);
        ::exit(EXIT_SUCCESS);
    }

    // Process the command for real (ie throw errors for unknown options, etc).
    parser.process(appArguments);
    return command;
}

/// \todo Move to AbstractCommand::processOptions?
int requireOptions(const QCommandLineParser &parser, const AbstractCommand &worker)
{
    QStringList requiredOptions = worker.requiredOptions();
    std::remove_if(requiredOptions.begin(), requiredOptions.end(), [&parser](const QString &name){
        return parser.isSet(name);
    });
    for (const QString &name: requiredOptions) {
        showCliError(QCoreApplication::translate("main", "Missing required option: %1").arg(name));
    }
    return requiredOptions.length();
}

/// \todo Move to AbstractCommand::processOptions?
int warnOnIgnoredOptions(const QCommandLineParser &parser, const AbstractCommand &worker)
{
    const QStringList supportedOptions = worker.supportedOptions();
    QStringList ignoredOptions = parser.optionNames();
    ignoredOptions.removeDuplicates();
    ignoredOptions.sort();
    std::remove_if(ignoredOptions.begin(), ignoredOptions.end(), [&ignoredOptions](const QString &name){
        return ignoredOptions.contains(name);
    });
    for (const QString &name: qAsConst(ignoredOptions)) {
        qWarning() << QCoreApplication::translate("main", "Ignoring option: %1").arg(name);
    }
    return ignoredOptions.length();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral(CMAKE_PROJECT_NAME));
    app.setApplicationVersion(QStringLiteral(CMAKE_PROJECT_VERSION));

    // Parse the command line.
    const QStringList appArguments = app.arguments();
    QCommandLineParser parser;
    const Command command = parseCommandLine(appArguments, parser);
    configureLogging(parser);

    // Handle the given command.
    AbstractCommand * worker = nullptr;
    switch (command) {
    case Command::DSO:      break;
    case Command::FlashLed: break;
    case Command::Info:
        worker = new InfoCommand(&app);
        break;
    case Command::Logger:   break;
    case Command::Meter:    break;
    case Command::None:
        showCliError(QCoreApplication::translate("main",
            "Missing argument: <command>\nSee --help for usage information."));
        return EXIT_FAILURE;
    case Command::Scan:
        worker = new ScanCommand(&app);
        //scanner.start(parser.value(QStringLiteral("timeout")).toInt()); /// \todo Move.
        break;
    case Command::Status:   break;
    case Command::SetName:  break;
    }
    if (worker == nullptr) {
        showCliError(QCoreApplication::translate("main", "Unknown command (%1)").arg((int)command));
        return EXIT_FAILURE;
    }

    if (requireOptions(parser, *worker) > 0) {
        return EXIT_FAILURE;
    }
    warnOnIgnoredOptions(parser, *worker);
    worker->processOptions(parser);
    return app.exec();
}
