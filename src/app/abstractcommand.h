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

#ifndef QTPOKIT_ABSTRACTCOMMAND_H
#define QTPOKIT_ABSTRACTCOMMAND_H

#include <QBluetoothDeviceInfo>
#include <QCommandLineParser>
#include <QLoggingCategory>
#include <QObject>

class PokitDiscoveryAgent;

class AbstractCommand : public QObject
{
public:
    enum class OutputFormat {
        Csv,  ///< RFC 4180 compliant CSV text.
        Json, ///< RFC 8259 compliant JSON text.
        Text, ///< Plain unstructured text.
    };

    explicit AbstractCommand(QObject * const parent);

    virtual QStringList requiredOptions(const QCommandLineParser &parser) const;
    virtual QStringList supportedOptions(const QCommandLineParser &parser) const;

    static QString escapeCsvField(const QString &field);
    static quint32 parseMilliValue(const QString &value, const QString &unit,
                                   const quint32 sensibleMinimum=0);
    static quint32 parseWholeValue(const QString &value, const QString &unit);

public slots:
    virtual QStringList processOptions(const QCommandLineParser &parser);
    virtual bool start() = 0;

protected:
    PokitDiscoveryAgent * discoveryAgent; ///< Agent for Pokit device descovery.
    OutputFormat format; ///< Selected output format.
    static Q_LOGGING_CATEGORY(lc, "pokit.ui.command", QtInfoMsg); ///< Logging category for UI commands.

protected slots:
    QString deviceToScanFor; ///< Device (if any) that were passed to processOptions().
    virtual void deviceDiscovered(const QBluetoothDeviceInfo &info) = 0;
    virtual void deviceDiscoveryFinished() = 0;

    friend class TestAbstractCommand;
};

#endif // QTPOKIT_ABSTRACTCOMMAND_H
