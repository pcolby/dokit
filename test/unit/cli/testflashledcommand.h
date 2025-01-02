// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestFlashLedCommand : public QObject
{
    Q_OBJECT

private slots:
    void requiredOptions();

    void supportedOptions();

    void processOptions();

    void getService();

    void serviceDetailsDiscovered();

    void deviceLedFlashed_data();
    void deviceLedFlashed();

    void tr();
};
