// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

QTPOKIT_FORWARD_DECLARE_CLASS(CalibrationService)

class CalibrateCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit CalibrateCommand(QObject * const parent = nullptr);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    CalibrationService * service { nullptr }; ///< Bluetooth service this command interacts with.
    float temperature { std::numeric_limits<float>::quiet_NaN() }; ///< Ambient temperature from the CLI options.

private slots:
    void temperatureCalibrated();

    QTPOKIT_BEFRIEND_TEST(CalibrateCommand)
};
