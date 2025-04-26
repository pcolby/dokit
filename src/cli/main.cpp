// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
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
#include "../stringliterals_p.h"

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

DOKIT_USE_STRINGLITERALS

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
    QString messagePattern = u"%{if-category}%{category}: %{endif}%{message}"_s;

    if (parser.isSet(u"debug"_s)) {
        #ifdef QT_MESSAGELOGCONTEXT
        // %{file}, %{line} and %{function} are only available when QT_MESSAGELOGCONTEXT is set.
        messagePattern.prepend(u"%{function} "_s);
        #endif
        messagePattern.prepend(u"%{time process} %{threadid} %{type} "_s);
        QLoggingCategory::setFilterRules(u"dokit.*.debug=true\npokit.*.debug=true"_s);
    }

    if (const QString color = parser.value(u"color"_s);
        (color == u"yes"_s) || (color == u"auto"_s && haveConsole())) {
        messagePattern.prepend(QStringLiteral(
        "%{if-debug}\x1b[37m%{endif}"      // White
        "%{if-info}\x1b[32m%{endif}"       // Green
        "%{if-warning}\x1b[35m%{endif}"    // Magenta
        "%{if-critical}\x1b[31m%{endif}"   // Red
        "%{if-fatal}\x1b[31;1m%{endif}")); // Red and bold
        messagePattern.append(u"\x1b[0m"_s); // Reset.
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
    // Output the same way QCommandLineParser::showMessageAndExit() does.
    const QString message = QCoreApplication::applicationName() + u": "_s + errorText + u'\n';
    fputs(qUtf8Printable(message), stderr);
}

Command getCliCommand(const QStringList &posArguments)
{
    if (posArguments.isEmpty()) {
        return Command::None;
    }
    if (posArguments.size() > 1) {
        showCliError(Private::tr("More than one command: %1").arg(posArguments.join(u", "_s)));
        ::exit(EXIT_FAILURE);
    }

    const QMap<QString, Command> supportedCommands {
        { u"info"_s,         Command::Info },
        { u"status"_s,       Command::Status },
        { u"meter"_s,        Command::Meter },
        { u"dso"_s,          Command::DSO },
        { u"logger-start"_s, Command::LoggerStart },
        { u"logger-stop"_s,  Command::LoggerStop },
        { u"logger-fetch"_s, Command::LoggerFetch },
        { u"scan"_s,         Command::Scan },
        { u"set-name"_s,     Command::SetName },
        { u"set-torch"_s,    Command::SetTorch },
        { u"flash-led"_s,    Command::FlashLed },
        { u"calibrate"_s,    Command::Calibrate },
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
        { u"color"_s,
          Private::tr("Colors the console output. Valid options are: yes, no and auto. The default is auto."),
          u"yes|no|auto"_s, u"auto"_s},
        {{u"debug"_s},
          Private::tr("Enable debug output.")},
        {{u"d"_s, u"device"_s},
          Private::tr("Set the name, hardware address or macOS UUID of Pokit device to use. If not specified, "
          "the first discovered Pokit device will be used."),
          Private::tr("device")},
    });
    parser.addHelpOption();
    parser.addOptions({
        {{u"interval"_s},
          Private::tr("Set the update interval for DOS, meter and "
          "logger modes. Suffixes such as 's' and 'ms' (for seconds and milliseconds) may be used. "
          "If no suffix is present, the units will be inferred from the magnitide of the given "
          "interval. If the option itself is not specified, a sensible default will be chosen "
          "according to the selected command."),
          Private::tr("interval")},
        {{u"mode"_s},
          Private::tr("Set the desired operation mode. For "
          "meter, dso, and logger commands, the supported modes are: AC Voltage, DC Voltage, AC Current, "
          "DC Current, Resistance, Diode, Continuity, and Temperature. All are case insensitive. "
          "Only the first four options are available for dso and logger commands; the rest are "
          "available in meter mode only. Temperature is also available for logger commands, but "
          "requires firmware v1.5 or later for Pokit devices to support it. For the set-torch command "
          "supported modes are On and Off."),
          Private::tr("mode")},
        {{u"new-name"_s},
          Private::tr("Give the desired new name for the set-name command."), Private::tr("name")},
        {{u"output"_s},
          Private::tr("Set the format for output. Supported "
          "formats are: CSV, JSON and Text. All are case insenstitve. The default is Text."),
          Private::tr("format"),
          Private::tr("text")},
        {{u"range"_s},
          Private::tr("Set the desired measurement range. Pokit "
          "devices support specific ranges, such as 0 to 300mV. Specify the desired upper limit, "
          "and the best range will be selected, or use 'auto' to enable the Pokit device's auto-"
          "range feature. The default is 'auto'."),
          Private::tr("range"), u"auto"_s},
        {{u"samples"_s}, Private::tr("Set the number of samples to acquire."), Private::tr("count")},
        {{u"temperature"_s},
          Private::tr("Set the current ambient temperature for the calibration command."), Private::tr("degrees")},
        {{u"timeout"_s},
          Private::tr("Set the device discovery scan timeout. "
          "Suffixes such as 's' and 'ms' (for seconds and milliseconds) may be used. "
          "If no suffix is present, the units will be inferred from the magnitide of the given "
          "interval. The default behaviour is no timeout."),
          Private::tr("period")},
        {{u"timestamp"_s},
          Private::tr("Set the optional starting timestamp for data logging. Default to 'now'."),
          Private::tr("period")},
        {{u"trigger-level"_s}, Private::tr("Set the DSO trigger level."), Private::tr("level")},
        {{u"trigger-mode"_s},
          Private::tr("Set the DSO trigger mode. Supported modes are: free, rising and falling. The default is free."),
          Private::tr("mode"), u"free"_s},
    });
    parser.addVersionOption();

    // Add supported 'commands' (as positional arguments, so they'll appear in the help text).
    parser.addPositionalArgument(u"info"_s,         Private::tr("Get Pokit device information"), u" "_s);
    parser.addPositionalArgument(u"status"_s,       Private::tr("Get Pokit device status"), u" "_s);
    parser.addPositionalArgument(u"meter"_s,        Private::tr("Access Pokit device's multimeter mode"), u" "_s);
    parser.addPositionalArgument(u"dso"_s,          Private::tr("Access Pokit device's DSO mode"), u" "_s);
    parser.addPositionalArgument(u"logger-start"_s, Private::tr("Start Pokit device's data logger mode"), u" "_s);
    parser.addPositionalArgument(u"logger-stop"_s,  Private::tr("Stop Pokit device's data logger mode"), u" "_s);
    parser.addPositionalArgument(u"logger-fetch"_s, Private::tr("Fetch Pokit device's data logger samples"), u" "_s);
    parser.addPositionalArgument(u"scan"_s,         Private::tr("Scan Bluetooth for Pokit devices"), u" "_s);
    parser.addPositionalArgument(u"set-name"_s,     Private::tr("Set Pokit device's name"), u" "_s);
    parser.addPositionalArgument(u"set-torch"_s,    Private::tr("Set Pokit device's torch on or off"), u" "_s);
    parser.addPositionalArgument(u"flash-led"_s,    Private::tr("Flash Pokit device's LED (Pokit Meter only)"), u" "_s);
    parser.addPositionalArgument(u"calibrate"_s,    Private::tr("Calibrate Pokit device temperature"), u" "_s);

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
    if (parser.isSet(u"help"_s)) {
        const QString commandString = (command == Command::None) ? u"<command>"_s
                : parser.positionalArguments().constFirst();
        std::cout << qUtf8Printable(parser.helpText()
            .replace(u"[options]"_s, commandString + u" [options]"_s)
            .replace(u"Arguments:"_s,  u"Command:"_s)
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
        showCliError(Private::tr("Missing argument: <command>\nSee --help for usage information."));
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

#if defined(Q_OS_MACOS)
    // Qt ignores shell locale overrides on macOS (QTBUG-51386), so mimic Qt's handling of LANG on *nixes.
    const QString localeName = QString::fromLocal8Bit(qgetenv("LANG"));
    if (!localeName.isEmpty()) {
        const QLocale newLocale(localeName);
        if (newLocale.name() != localeName.split('.'_L1).constFirst()) {
            qWarning() << "Could not find locale" << localeName;
        } else {
            QLocale::setDefault(newLocale);
        }
    }
#endif

    // Install localised translators, if we have translations for the current locale.
    const QLocale locale;
    QTranslator appTranslator, libTranslator;
    if (appTranslator.load(locale, u"cli"_s, u"/"_s, u":/i18n"_s)) {
        QCoreApplication::installTranslator(&appTranslator);
    }
    if (libTranslator.load(locale, u"lib"_s, u"/"_s, u":/i18n"_s)) {
        QCoreApplication::installTranslator(&libTranslator);
    }

    // Parse the command line.
    const QStringList appArguments = QCoreApplication::arguments();
    QCommandLineParser parser;
    const Command commandType = parseCommandLine(appArguments, parser);
    qCDebug(lc).noquote() << "Locale:" << locale << locale.uiLanguages();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)) // QTranslator::filePath() added in Qt 5.15.
    qCDebug(lc).noquote() << "App translations:" <<
        (appTranslator.filePath().isEmpty() ? u"<none>"_s : appTranslator.filePath());
    qCDebug(lc).noquote() << "Library translations:" <<
        (libTranslator.filePath().isEmpty() ? u"<none>"_s : libTranslator.filePath());
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
