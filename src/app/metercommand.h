// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/multimeterservice.h>

class MeterCommand : public DeviceCommand
{
public:
    explicit MeterCommand(QObject * const parent);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    MultimeterService * service; ///< Bluetooth service this command interracts with.
    MultimeterService::Settings settings; ///< Settings for the Pokit device's multimeter mode.
    int samplesToGo; ///< Number of samples to read, if specified on the CLI.
    bool showCsvHeader; ///< Whether or not to show a header as the first line of CSV output.

    static MultimeterService::Range lowestRange(const MultimeterService::Mode mode, const quint32 desiredMax);
    static MultimeterService::CurrentRange lowestCurrentRange(const quint32 desiredMax);
    static MultimeterService::ResistanceRange lowestResistanceRange(const quint32 desiredMax);
    static MultimeterService::VoltageRange lowestVoltageRange(const quint32 desiredMax);

private slots:
    void settingsWritten();
    void outputReading(const MultimeterService::Reading &reading);

    friend class TestMeterCommand;
};
