// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "calibratecommand.h"
#include "dsocommand.h"
#include "flashledcommand.h"
#include "infocommand.h"
#include "loggerfetchcommand.h"
#include "loggerstartcommand.h"
#include "loggerstopcommand.h"
#include "metercommand.h"
#include "scancommand.h"
#include "setnamecommand.h"
#include "settorchcommand.h"
#include "statuscommand.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QLocale>
#include <QLoggingCategory>
#include <QTranslator>

#include <iostream>

#if defined(Q_OS_UNIX)
#include <unistd.h>
#elif defined(Q_OS_WIN)
#include <Windows.h>
#endif

static Q_LOGGING_CATEGORY(lc, "dokit.cli.main", QtInfoMsg);

class Private
{
    Q_DECLARE_TR_FUNCTIONS(cli_main)
};

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
        messagePattern.prepend(QStringLiteral("%{time process} %{threadid} %{type} "));
        QLoggingCategory::setFilterRules(QStringLiteral("dokit.*.debug=true\npokit.*.debug=true"));
    }

    if (const QString color = parser.value(QStringLiteral("color"));
        (color == QStringLiteral("yes")) || (color == QStringLiteral("auto") && haveConsole())) {
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
    None,
    Info,
    Status,
    Meter,
    DSO,
    LoggerStart,
    LoggerStop,
    LoggerFetch,
    Scan,
    SetName,
    SetTorch,
    FlashLed,
    Calibrate
};

void showCliError(const QString &errorText)
{
    // Output the same way QCommandLineParser does (qcommandlineparser.cpp::showParserMessage).
    const QString message = QCoreApplication::applicationName() + QLatin1String(": ")
        + errorText + QLatin1Char('\n');
    std::cerr << qUtf8Printable(message);
}

Command getCliCommand(const QStringList &posArguments)
{
    if (posArguments.isEmpty()) {
        return Command::None;
    }
    if (posArguments.size() > 1) {
        showCliError(Private::tr("More than one command: %1").arg(posArguments.join(QStringLiteral(", "))));
        ::exit(EXIT_FAILURE);
    }

    const QMap<QString, Command> supportedCommands {
        { QStringLiteral("info"),         Command::Info },
        { QStringLiteral("status"),       Command::Status },
        { QStringLiteral("meter"),        Command::Meter },
        { QStringLiteral("dso"),          Command::DSO },
        { QStringLiteral("logger-start"), Command::LoggerStart },
        { QStringLiteral("logger-stop"),  Command::LoggerStop },
        { QStringLiteral("logger-fetch"), Command::LoggerFetch },
        { QStringLiteral("scan"),         Command::Scan },
        { QStringLiteral("set-name"),     Command::SetName },
        { QStringLiteral("set-torch"),    Command::SetTorch },
        { QStringLiteral("flash-led"),    Command::FlashLed },
        { QStringLiteral("calibrate"),    Command::Calibrate },
    };
    const Command command = supportedCommands.value(posArguments.first().toLower(), Command::None);
    if (command == Command::None) {
        showCliError(Private::tr("Unknown command: %1").arg(posArguments.first()));
        ::exit(EXIT_FAILURE);
    }
    return command;
}

Command parseCommandLine(const QStringList &appArguments, QCommandLineParser &parser)
{
    // Setup the command line options.
    parser.addOptions({
        { QStringLiteral("color"),
          Private::tr("Colors the console output. Valid options "
          "are: yes, no and auto. The default is auto."),
          QStringLiteral("yes|no|auto"), QStringLiteral("auto")},
        {{QStringLiteral("debug")},
          Private::tr("Enable debug output.")},
        {{QStringLiteral("d"), QStringLiteral("device")},
          Private::tr(
          "Set the name, hardware address or macOS UUID of Pokit device to use. If not specified, "
          "the first discovered Pokit device will be used."),
          Private::tr("device")},
    });
    parser.addHelpOption();
    parser.addOptions({
        {{QStringLiteral("interval")},
          Private::tr("Set the update interval for DOS, meter and "
          "logger modes. Suffixes such as 's' and 'ms' (for seconds and milliseconds) may be used. "
          "If no suffix is present, the units will be inferred from the magnitide of the given "
          "interval. If the option itself is not specified, a sensible default will be chosen "
          "according to the selected command."),
          Private::tr("interval")},
        {{QStringLiteral("mode")},
          Private::tr("Set the desired operation mode. For "
          "meter, dso, and logger commands, the supported modes are: AC Voltage, DC Voltage, AC Current, "
          "DC Current, Resistance, Diode, Continuity, and Temperature. All are case insensitive. "
          "Only the first four options are available for dso and logger commands; the rest are "
          "available in meter mode only. Temperature is also available for logger commands, but "
          "requires firmware v1.5 or later for Pokit devices to support it. For the set-torch command "
          "supported modes are On and Off."),
          Private::tr("mode")},
        {{QStringLiteral("new-name")},
          Private::tr("Give the desired new name for the set-"
          "name command."), Private::tr("name")},
        {{QStringLiteral("output")},
          Private::tr("Set the format for output. Supported "
          "formats are: CSV, JSON and Text. All are case insenstitve. The default is Text."),
          Private::tr("format"),
          Private::tr("text")},
        {{QStringLiteral("range")},
          Private::tr("Set the desired measurement range. Pokit "
          "devices support specific ranges, such as 0 to 300mV. Specify the desired upper limit, "
          "and the best range will be selected, or use 'auto' to enable the Pokit device's auto-"
          "range feature. The default is 'auto'."),
          Private::tr("range"), QStringLiteral("auto")},
        {{QStringLiteral("samples")},
          Private::tr("Set the number of samples to acquire."),
          Private::tr("count")},
        {{QStringLiteral("temperature")},
          Private::tr("Set the current ambient temperature for "
          "the calibration command."), Private::tr("degrees")},
        {{QStringLiteral("timeout")},
          Private::tr("Set the device discovery scan timeout."
          "Suffixes such as 's' and 'ms' (for seconds and milliseconds) may be used. "
          "If no suffix is present, the units will be inferred from the magnitide of the given "
          "interval. The default behaviour is no timeout."),
          Private::tr("period")},
        {{QStringLiteral("timestamp")},
          Private::tr("Set the optional starting timestamp for "
          "data logging. Default to 'now'."),
        Private::tr("period")},
        {{QStringLiteral("trigger-level")},
          Private::tr("Set the DSO trigger level."),
          Private::tr("level")},
        {{QStringLiteral("trigger-mode")},
          Private::tr("Set the DSO trigger mode. Supported "
          "modes are: free, rising and falling. The default is free."),
          Private::tr("mode"), QStringLiteral("free")},
    });
    parser.addVersionOption();

    // Add supported 'commands' (as positional arguments, so they'll appear in the help text).
    parser.addPositionalArgument(QStringLiteral("info"),
        Private::tr("Get Pokit device information"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("status"),
        Private::tr("Get Pokit device status"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("meter"),
        Private::tr("Access Pokit device's multimeter mode"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("dso"),
        Private::tr("Access Pokit device's DSO mode"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("logger-start"),
        Private::tr("Start Pokit device's data logger mode"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("logger-stop"),
        Private::tr("Stop Pokit device's data logger mode"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("logger-fetch"),
        Private::tr("Fetch Pokit device's data logger samples"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("scan"),
        Private::tr("Scan Bluetooth for Pokit devices"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("set-name"),
        Private::tr("Set Pokit device's name"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("set-torch"),
        Private::tr("Set Pokit device's torch on or off"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("flash-led"),
        Private::tr("Flash Pokit device's LED (Pokit Meter only)"),
        QStringLiteral(" "));
    parser.addPositionalArgument(QStringLiteral("calibrate"),
        Private::tr("Calibrate Pokit device temperature"),
        QStringLiteral(" "));

    // Do the initial parse, the see if we have a command specified yet.
    parser.parse(appArguments);
    configureLogging(parser);
    qCDebug(lc).noquote() << QCoreApplication::applicationName() << QCoreApplication::applicationVersion();
    qCDebug(lc).noquote() << "Qt " QT_VERSION_STR " compile-time";
    qCDebug(lc).noquote() << "Qt" << qVersion() << "runtime";
    const Command command = getCliCommand(parser.positionalArguments());

    // If we have a (single, valid) command, then remove the commands list from the help text.
    if (command != Command::None) {
        parser.clearPositionalArguments();
    }

    // Handle -h|--help explicitly, so we can tweak the output to include the <command> info.
    if (parser.isSet(QStringLiteral("help"))) {
        const QString commandString = (command == Command::None) ? QStringLiteral("<command>")
                : parser.positionalArguments().constFirst();
        std::cout << qUtf8Printable(parser.helpText()
            .replace(QStringLiteral("[options]"), commandString + QStringLiteral(" [options]"))
            .replace(QStringLiteral("Arguments:"),  QStringLiteral("Command:"))
        );
        ::exit(EXIT_SUCCESS);
    }

    // Process the command for real (ie throw errors for unknown options, etc).
    parser.process(appArguments);
    return command;
}

AbstractCommand * getCommandObject(const Command command, QObject * const parent)
{
    switch (command) {
    case Command::None:
        showCliError(Private::tr(
            "Missing argument: <command>\nSee --help for usage information."));
        return nullptr;
    case Command::Calibrate:   return new CalibrateCommand(parent);
    case Command::DSO:         return new DsoCommand(parent);
    case Command::FlashLed:    return new FlashLedCommand(parent);
    case Command::Info:        return new InfoCommand(parent);
    case Command::LoggerStart: return new LoggerStartCommand(parent);
    case Command::LoggerStop:  return new LoggerStopCommand(parent);
    case Command::LoggerFetch: return new LoggerFetchCommand(parent);
    case Command::Meter:       return new MeterCommand(parent);
    case Command::Scan:        return new ScanCommand(parent);
    case Command::Status:      return new StatusCommand(parent);
    case Command::SetName:     return new SetNameCommand(parent);
    case Command::SetTorch:    return new SetTorchCommand(parent);
    }
    showCliError(Private::tr("Unknown command (%1)").arg((int)command));
    return nullptr;
}

int main(int argc, char *argv[])
{
    // Setup the core application.
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral(PROJECT_NAME));
    QCoreApplication::setApplicationVersion(QString::fromLatin1(PROJECT_VERSION
        #ifdef PROJECT_PRE_RELEASE
        "-" PROJECT_PRE_RELEASE
        #endif
        #ifdef PROJECT_BUILD_ID
        "+" PROJECT_BUILD_ID
        #endif
    ));

    // Install localised translators, if we have translations for the current locale.
    QTranslator appTranslator, libTranslator;
    if (appTranslator.load(QLocale(), QStringLiteral("cli"), QStringLiteral("/"), QStringLiteral(":/i18n"))) {
        QCoreApplication::installTranslator(&appTranslator);
    }
    if (libTranslator.load(QLocale(), QStringLiteral("lib"), QStringLiteral("/"), QStringLiteral(":/i18n"))) {
        QCoreApplication::installTranslator(&libTranslator);
    }

    // Parse the command line.
    const QStringList appArguments = QCoreApplication::arguments();
    QCommandLineParser parser;
    const Command commandType = parseCommandLine(appArguments, parser);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)) // QTranslator::filePath() added in Qt 5.15.
    qCDebug(lc).noquote() << "App translations:" <<
        (appTranslator.filePath().isEmpty() ? QStringLiteral("<none>") : appTranslator.filePath());
    qCDebug(lc).noquote() << "Library translations:" <<
        (libTranslator.filePath().isEmpty() ? QStringLiteral("<none>") : libTranslator.filePath());
#else
    qCDebug(lc).noquote() << "App translations:" << (!appTranslator.isEmpty());
    qCDebug(lc).noquote() << "Lib translations:" << (!libTranslator.isEmpty());
#endif

    // Handle the given command.
    AbstractCommand * const command = getCommandObject(commandType, &app);
    if (command == nullptr) {
        return EXIT_FAILURE; // getCommandObject will have logged the reason already.
    }
    const QStringList cliErrors = command->processOptions(parser);
    for (const QString &error: cliErrors) {
        showCliError(error);
    }
    const int result = ((cliErrors.isEmpty()) && (command->start())) ? QCoreApplication::exec() : EXIT_FAILURE;
    delete command; // We don't strictly need to do this, but it does fix QTBUG-119063, and is probably good practice.
    return result;
}
