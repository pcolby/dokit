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
    DsoService * service; ///< Bluetooth service this command interracts with.
    DsoService::Settings settings; ///< Settings for the Pokit device's DSO mode.
    DsoService::Metadata metadata; ///< Most recent DSO metadata.
    qint32 samplesToGo; ///< Number of samples we're expecting in the current window.
    bool showCsvHeader; ///< Whether or not to show a header as the first line of CSV output.

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
