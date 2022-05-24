// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

class StatusService;

class SetNameCommand : public DeviceCommand
{
public:
    explicit SetNameCommand(QObject * const parent);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    QString newName; ///< New name to set on the Pokit device.
    StatusService * service; ///< Bluetooth service this command interracts with.

private slots:
    void deivceNameWritten();

    friend class TestSetNameCommand;
};
