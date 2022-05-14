/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

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
};
