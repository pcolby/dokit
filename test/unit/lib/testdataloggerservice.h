// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <qtpokit/qtpokit_global.h>

#include <QTest>

QTPOKIT_BEGIN_NAMESPACE

class TestDataLoggerService : public QObject
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
    void readMetadataCharacteristic();

    void setSettings();
    void startLogger();
    void stopLogger();
    void fetchSamples();

    void metadata();
    void enableMetadataNotifications();
    void disableMetadataNotifications();

    void enableReadingNotifications();
    void disableReadingNotifications();

    void encodeSettings_data();
    void encodeSettings();

    void parseMetadata_data();
    void parseMetadata();

    void parseSamples_data();
    void parseSamples();

    void characteristicRead();
    void characteristicWritten();
    void characteristicChanged();

    void tr();
};

QTPOKIT_END_NAMESPACE
