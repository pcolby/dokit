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

#include "testabstractpokitservice.h"

#include <qtpokit/abstractpokitservice.h>
#include "abstractpokitservice_p.h"

#include <QRegularExpression>

void TestAbstractPokitService::autoDiscover()
{

}

void TestAbstractPokitService::service()
{

}

void TestAbstractPokitService::createServiceObject()
{

}

void TestAbstractPokitService::getCharacteristic()
{

}

void TestAbstractPokitService::readCharacteristic()
{

}

void TestAbstractPokitService::enableCharacteristicNotificatons()
{

}

void TestAbstractPokitService::disableCharacteristicNotificatons()
{

}


void TestAbstractPokitService::checkSize_data()
{
    QTest::addColumn<QString>("label");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("min");
    QTest::addColumn<int>("max");
    QTest::addColumn<bool>("failOnMax");
    QTest::addColumn<bool>("expected");

    QTest::addRow("ok") << QStringLiteral("<label>") << QByteArray(5, '\x01')
        << 2 << 10 << false << true;

    QTest::addRow("too-short") << QStringLiteral("<label>") << QByteArray("\x01")
        << 2 << 10 << false << false;

    QTest::addRow("long") << QStringLiteral("<label>") << QByteArray(20, '\x01')
        << 2 << 10 << false << true;

    QTest::addRow("too-long") << QStringLiteral("<label>") << QByteArray(20,'\x01')
        << 2 << 10 << true << false;
}

void TestAbstractPokitService::checkSize()
{
    QFETCH(QString, label);
    QFETCH(QByteArray, data);
    QFETCH(int, min);
    QFETCH(int, max);
    QFETCH(bool, failOnMax);
    QFETCH(bool, expected);
    if (data.size() < min) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^<label> requires \\d+ bytes, but only \\d+ present: 0x[a-zA-Z0-9,]*$")));
    }
    if (data.size() > max) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            "^<label> has \\d+ extraneous bytes: 0x[a-zA-Z0-9,]*$")));
    }
    QCOMPARE(AbstractPokitServicePrivate::checkSize(label, data, min, max, failOnMax), expected);
}


void TestAbstractPokitService::toHexString_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("max");
    QTest::addColumn<QString>("expected");

    QTest::addRow("empty") << QByteArray() << 10
        << QStringLiteral("0x");

    QTest::addRow("ok") << QByteArray("\x01\x02\x03\x04\x05") << 10
        << QStringLiteral("0x01,02,03,04,05");

    QTest::addRow("just-ok") << QByteArray("\x01\x02\x03\x04\x05") << 5
        << QStringLiteral("0x01,02,03,04,05");

    QTest::addRow("just-over") << QByteArray("\x01\x02\x03\x04\x05") << 4
        << QStringLiteral("0x01,...,05");

    QTest::addRow("well-over") << QByteArray(1000, '\x01') << 10
        << QStringLiteral("0x01,01,01,01,...,01,01,01,01");
}

void TestAbstractPokitService::toHexString()
{
    QFETCH(QByteArray, data);
    QFETCH(int, max);
    QFETCH(QString, expected);
    QCOMPARE(AbstractPokitServicePrivate::toHexString(data, max), expected);
}


void TestAbstractPokitService::connected()
{

}

void TestAbstractPokitService::discoveryFinished()
{

}

void TestAbstractPokitService::errorOccurred()
{

}

void TestAbstractPokitService::serviceDiscovered()
{

}

void TestAbstractPokitService::stateChanged()
{

}


void TestAbstractPokitService::characteristicRead()
{

}

void TestAbstractPokitService::characteristicWritten()
{

}

void TestAbstractPokitService::characteristicChanged()
{

}

QTEST_MAIN(TestAbstractPokitService)
