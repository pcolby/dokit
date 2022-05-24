// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestCalibrationService : public QObject
{
    Q_OBJECT

private slots:
    void readCharacteristics();
    void calibrateTemperature();

    void encodeTemperature_data();
    void encodeTemperature();

    void characteristicWritten();
};
