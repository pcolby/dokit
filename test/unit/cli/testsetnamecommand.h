// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestSetNameCommand : public QObject
{
    Q_OBJECT

private slots:
    void requiredOptions();

    void supportedOptions();

    void processOptions_data();
    void processOptions();

    void getService();

    void serviceDetailsDiscovered();

    void deviceNameWritten_data();
    void deviceNameWritten();

    void tr();
};
