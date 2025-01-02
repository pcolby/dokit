// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestMeterCommand : public QObject
{
    Q_OBJECT

private slots:
    void requiredOptions();

    void supportedOptions();

    void processOptions_data();
    void processOptions();

    void getService();

    void serviceDetailsDiscovered();

    void settingsWritten();

    void outputReading_data();
    void outputReading();

    void tr();
};
