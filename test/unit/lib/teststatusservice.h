// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestStatusService : public QObject
{
    Q_OBJECT

private slots:
    void toString_DeviceStatus_data();
    void toString_DeviceStatus();

    void toString_BatteryStatus_data();
    void toString_BatteryStatus();

    void toString_SwitchPosition_data();
    void toString_SwitchPosition();

    void toString_ChargingStatus_data();
    void toString_ChargingStatus();

    void toString_TorchStatus_data();
    void toString_TorchStatus();

    void toString_ButtonStatus_data();
    void toString_ButtonStatus();

    void readCharacteristics();
    void readDeviceCharacteristics();
    void readStatusCharacteristic();
    void readNameCharacteristic();
    void readTorchCharacteristic();
    void readButtonPressCharacteristic();

    void deviceCharacteristics();

    void status();

    void deviceName();
    void setDeviceName();

    void flashLed();

    void parseDeviceCharacteristics_data();
    void parseDeviceCharacteristics();

    void parseStatus_data();
    void parseStatus();

    void parseTorchStatus_data();
    void parseTorchStatus();

    void parseButtonPress_data();
    void parseButtonPress();

    void serviceDiscovered();

    void characteristicRead();
    void characteristicWritten();

    void tr();
};
