// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestAbstractPokitService : public QObject
{
    Q_OBJECT

private slots:
    // AbstractPokitService tests.
    void autoDiscover();
    void service();

    // AbstractPokitServicePrivate tests.
    // Most of these only test safe error handling, since more would require mocking Qt's BLE classes.
    void createServiceObject();
    void getCharacteristic();
    void readCharacteristic();
    void enableCharacteristicNotificatons();
    void disableCharacteristicNotificatons();

    void checkSize_data();
    void checkSize();

    void toHexString_data();
    void toHexString();

    void connected();
    void discoveryFinished();
    void errorOccurred();
    void serviceDiscovered();
    void stateChanged();

    void characteristicRead();
    void characteristicWritten();
    void characteristicChanged();

    void tr();
};
