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

#include "tst_utils.h"

#include "utils.h"

void TestUtils::toString_majorClass_data()
{
    QTest::addColumn<int>("majorClass");
    QTest::addColumn<QString>("expected");

    // Test all happy paths.
    #define ADD_ROW(majorClass) QTest::addRow(#majorClass) \
        << (int)QBluetoothDeviceInfo::majorClass << QString::fromLatin1(#majorClass)
    ADD_ROW(MiscellaneousDevice);
    ADD_ROW(ComputerDevice);
    ADD_ROW(PhoneDevice);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    ADD_ROW(LANAccessDevice); // Deprecated since Qt 5.13.
    #else
    ADD_ROW(NetworkDevice); // Added in Qt 5.13.
    #endif
    ADD_ROW(AudioVideoDevice);
    ADD_ROW(PeripheralDevice);
    ADD_ROW(ImagingDevice);
    ADD_ROW(WearableDevice);
    ADD_ROW(ToyDevice);
    ADD_ROW(HealthDevice);
    ADD_ROW(UncategorizedDevice);
    #undef ADD_ROW

    // Test unhappy paths; these should all safely return null strings.
    QTest::addRow("-999") << -999 << QString();
    QTest::addRow("-1")   << -1 << QString();
    QTest::addRow("999")  << 999 << QString();
}

void TestUtils::toString_majorClass()
{
    QFETCH(int, majorClass);
    QFETCH(QString, expected);
    QCOMPARE(toString(static_cast<QBluetoothDeviceInfo::MajorDeviceClass>(majorClass)), expected);
}

QTEST_MAIN(TestUtils)
