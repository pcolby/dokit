// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestDeviceCommand : public QObject
{
    Q_OBJECT

private slots:
    void start();

    void disconnect();

    void controllerError();

    void deviceDisconnected();

    void serviceError();

    void serviceDetailsDiscovered();

    void deviceDiscovered();

    void deviceDiscoveryFinished();
};
