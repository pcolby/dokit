// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/dataloggerservice.h>

class LoggerStopCommand : public DeviceCommand
{
public:
    explicit LoggerStopCommand(QObject * const parent);

protected:
    AbstractPokitService * getService() override;

protected slots:
    void serviceDetailsDiscovered() override;

private:
    DataLoggerService * service; ///< Bluetooth service this command interracts with.

private slots:
    void settingsWritten();

    friend class TestLoggerStopCommand;
};
