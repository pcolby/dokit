// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestCalibrateCommand : public QObject
{
    Q_OBJECT

private slots:
    void requiredOptions();

    void supportedOptions();

    void processOptions_data();
    void processOptions();

    void getService();

    void serviceDetailsDiscovered();

    void temperatureCalibrated();
};
