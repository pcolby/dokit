// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <qtpokit/qtpokit_global.h>

#include <QTest>

QTPOKIT_BEGIN_NAMESPACE

class TestCalibrationService : public QObject
{
    Q_OBJECT

private slots:
    void readCharacteristics();
    void calibrateTemperature();

    void encodeTemperature_data();
    void encodeTemperature();

    void characteristicWritten();

    void tr();
};

QTPOKIT_END_NAMESPACE
