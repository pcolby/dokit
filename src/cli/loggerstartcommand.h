// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/dataloggerservice.h>
#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>

class LoggerStartCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit LoggerStartCommand(QObject * const parent = nullptr);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    quint8 (* minRangeFunc)(const PokitProduct product, const quint32 maxValue) { nullptr };
    quint32 rangeOptionValue { 0 };          ///< The parsed value of range option.
    DataLoggerService * service { nullptr }; ///< Bluetooth service this command interracts with.
    DataLoggerService::Settings settings {   ///< Settings for the Pokit device's data logger mode.
        DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
        +PokitMeter::VoltageRange::AutoRange, 60'000, 0
    };

private slots:
    void settingsWritten();

    friend class TestLoggerStartCommand;
};
