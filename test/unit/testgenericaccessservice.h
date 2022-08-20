// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestGenericAccessService : public QObject
{
    Q_OBJECT

private slots:
    void readCharacteristics();
    void readAppearanceCharacteristic();
    void readDeviceNameCharacteristic();

    void appearance();

    void deviceName();
    void setDeviceName();

    void parseAppearance_data();
    void parseAppearance();

    void characteristicRead();
    void characteristicWritten();

    void tr();
};
