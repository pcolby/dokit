// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

class DeviceInfoService;

class InfoCommand : public DeviceCommand
{
public:
    explicit InfoCommand(QObject * const parent);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    DeviceInfoService * service; ///< Bluetooth service this command interracts with.

    friend class TestInfoCommand;
};
