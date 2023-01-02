// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestDataLoggerService : public QObject
{
    Q_OBJECT

private slots:
    void toString_Mode_data();
    void toString_Mode();

    void toString_VoltageRange_data();
    void toString_VoltageRange();

    void minValue_VoltageRange_data();
    void minValue_VoltageRange();

    void maxValue_VoltageRange_data();
    void maxValue_VoltageRange();

    void toString_CurrentRange_data();
    void toString_CurrentRange();

    void minValue_CurrentRange_data();
    void minValue_CurrentRange();

    void maxValue_CurrentRange_data();
    void maxValue_CurrentRange();

    void range_Range();
    void range_Operators();

    void toString_Range_data();
    void toString_Range();

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
