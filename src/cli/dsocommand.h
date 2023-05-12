// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/dsoservice.h>
#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>

class DsoCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit DsoCommand(QObject * const parent = nullptr);

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
    quint32 rangeOptionValue { 0 };   ///< The parsed value of range option.
    DsoService * service { nullptr }; ///< Bluetooth service this command interracts with.
    DsoService::Settings settings {   ///< Settings for the Pokit device's DSO mode.
        DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
        +PokitMeter::VoltageRange::AutoRange, 1000*1000, 1000
    };
    DsoService::Metadata metadata; ///< Most recent DSO metadata.
    qint32 samplesToGo { 0 };      ///< Number of samples we're expecting in the current window.
    bool showCsvHeader { true };   ///< Whether or not to show a header as the first line of CSV output.

private slots:
    void settingsWritten();
    void metadataRead(const DsoService::Metadata &data);
    void outputSamples(const DsoService::Samples &samples);

    friend class TestDsoCommand;
};
