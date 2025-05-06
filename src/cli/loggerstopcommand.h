// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/dataloggerservice.h>

class LoggerStopCommand : public DeviceCommand
{
    Q_OBJECT

public:
    explicit LoggerStopCommand(QObject * const parent = nullptr);

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    DataLoggerService * service { nullptr }; ///< Bluetooth service this command interacts with.

private slots:
    void settingsWritten();

    QTPOKIT_BEFRIEND_TEST(LoggerStopCommand)
};
