// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestDeviceInfoService : public QObject
{
    Q_OBJECT

private slots:
    void readCharacteristics();
    void readFirmwareRevisionCharacteristic();
    void readHardwareRevisionCharacteristic();
    void readManufacturerCharacteristics();
    void readModelNumberCharacteristic();
    void readSoftwareRevisionCharacteristic();

    void manufacturer();
    void modelNumber();
    void hardwareRevision();
    void firmwareRevision();
    void softwareRevision();

    void characteristicRead();
};
