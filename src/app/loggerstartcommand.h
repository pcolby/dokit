// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/dataloggerservice.h>

class LoggerStartCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit LoggerStartCommand(QObject * const parent);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    DataLoggerService * service; ///< Bluetooth service this command interracts with.
    DataLoggerService::Settings settings; ///< Settings for the Pokit device's data logger mode.

    static DataLoggerService::Range lowestRange(const DataLoggerService::Mode mode,
                                                const quint32 desiredMax);
    static DataLoggerService::CurrentRange lowestCurrentRange(const quint32 desiredMax);
    static DataLoggerService::VoltageRange lowestVoltageRange(const quint32 desiredMax);

private slots:
    void settingsWritten();

    friend class TestLoggerStartCommand;
};
