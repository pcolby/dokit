// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/dsoservice.h>

class DsoCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit DsoCommand(QObject * const parent);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    DsoService * service { nullptr }; ///< Bluetooth service this command interracts with.
    DsoService::Settings settings {   ///< Settings for the Pokit device's DSO mode.
        DsoService::Command::FreeRunning, 0.0f, DsoService::Mode::DcVoltage,
        DsoService::VoltageRange::_30V_to_60V, 1000*1000, 1000
    };
    DsoService::Metadata metadata; ///< Most recent DSO metadata.
    qint32 samplesToGo { 0 };      ///< Number of samples we're expecting in the current window.
    bool showCsvHeader { true };   ///< Whether or not to show a header as the first line of CSV output.

    static DsoService::Range lowestRange(const DsoService::Mode mode,
                                                const quint32 desiredMax);
    static DsoService::CurrentRange lowestCurrentRange(const quint32 desiredMax);
    static DsoService::VoltageRange lowestVoltageRange(const quint32 desiredMax);

private slots:
    void settingsWritten();
    void metadataRead(const DsoService::Metadata &metadata);
    void outputSamples(const DsoService::Samples &samples);

    friend class TestDsoCommand;
};
