// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

class StatusService;

class FlashLedCommand : public DeviceCommand
{
public:
    explicit FlashLedCommand(QObject * const parent);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    StatusService * service; ///< Bluetooth service this command interracts with.

private slots:
    void deviceLedFlashed();

    friend class TestFlashLedCommand;
};
