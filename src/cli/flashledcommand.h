// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

class StatusService;

class FlashLedCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit FlashLedCommand(QObject * const parent = nullptr);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    StatusService * service { nullptr }; ///< Bluetooth service this command interracts with.

private slots:
    void deviceLedFlashed();

    friend class TestFlashLedCommand;
};
