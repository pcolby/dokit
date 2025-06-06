// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/statusservice.h>

class SetTorchCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit SetTorchCommand(QObject * const parent = nullptr);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    StatusService::TorchStatus newStatus { }; ///< New status to set on the Pokit device's torch to.
    StatusService * service { nullptr };      ///< Bluetooth service this command interacts with.

private slots:
    void torchStatusWritten();

    QTPOKIT_BEFRIEND_TEST(SetTorchCommand)
};
