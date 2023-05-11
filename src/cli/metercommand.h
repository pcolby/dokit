// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/multimeterservice.h>
#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>

class MeterCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit MeterCommand(QObject * const parent = nullptr);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    MultimeterService * service { nullptr }; ///< Bluetooth service this command interracts with.
    MultimeterService::Settings settings     ///< Settings for the Pokit device's multimeter mode.
        { MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000 };
    int samplesToGo { -1 } ;     ///< Number of samples to read, if specified on the CLI.
    bool showCsvHeader { true }; ///< Whether or not to show a header as the first line of CSV output.

private slots:
    void settingsWritten();
    void outputReading(const MultimeterService::Reading &reading);

    friend class TestMeterCommand;
};
