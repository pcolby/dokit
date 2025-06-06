// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DOKIT_ABSTRACTCOMMAND_H
#define DOKIT_ABSTRACTCOMMAND_H

#include <qtpokit/qtpokit_global.h>

#include <QBluetoothDeviceInfo>
#include <QCommandLineParser>
#include <QLoggingCategory>
#include <QObject>

QTPOKIT_FORWARD_DECLARE_CLASS(PokitDiscoveryAgent)

class AbstractCommand : public QObject
{
    Q_OBJECT

public:
    /// Supported output formats.
    enum class OutputFormat {
        Csv,  ///< RFC 4180 compliant CSV text.
        Json, ///< RFC 8259 compliant JSON text.
        Text, ///< Plain unstructured text.
    };

    explicit AbstractCommand(QObject * const parent = nullptr);

    virtual QStringList requiredOptions(const QCommandLineParser &parser) const;
    virtual QStringList supportedOptions(const QCommandLineParser &parser) const;

    static QString escapeCsvField(const QString &field);

    template<typename R>
    static quint32 parseNumber(const QString &value, const QString &unit, const quint32 sensibleMinimum = 0);

public slots:
    virtual QStringList processOptions(const QCommandLineParser &parser);
    virtual bool start() = 0;

protected:
    QString deviceToScanFor; ///< Device (if any) that were passed to processOptions().
    PokitDiscoveryAgent * discoveryAgent; ///< Agent for Pokit device discovery.
    OutputFormat format { OutputFormat::Text }; ///< Selected output format.
    static Q_LOGGING_CATEGORY(lc, "dokit.cli.command", QtInfoMsg); ///< Logging category for UI commands.

protected slots:
    virtual void deviceDiscovered(const QBluetoothDeviceInfo &info) = 0;
    virtual void deviceDiscoveryFinished() = 0;

    QTPOKIT_BEFRIEND_TEST(AbstractCommand)
};

#endif // DOKIT_ABSTRACTCOMMAND_H
