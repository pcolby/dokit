// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <qtpokit/qtpokit_global.h>

#include <QTest>

QTPOKIT_BEGIN_NAMESPACE

class TestMultimeterService : public QObject
{
    Q_OBJECT

private slots:
    void toString_Mode_data();
    void toString_Mode();

    void toString_Range_data();
    void toString_Range();

    void maxValue_data();
    void maxValue();

    void readCharacteristics();
    void readReadingCharacteristic();

    void setSettings();

    void reading();
    void enableReadingNotifications();
    void disableReadingNotifications();

    void encodeSettings_data();
    void encodeSettings();

    void parseReading_data();
    void parseReading();

    void characteristicRead();
    void characteristicWritten();
    void characteristicChanged();

    void tr();
};

QTPOKIT_END_NAMESPACE
