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
#include "uuids.h"

#include <QJsonDocument>

// Serialiser for QCOMPARE to output QJsonArray objects on test failures.
char *toString(const QJsonArray &array)
{
    return qstrdup(("QJsonArray(" + QJsonDocument(array).toJson(QJsonDocument::Compact) + ")").constData());
}

void TestUtils::toJson_info_data()
{

}

void TestUtils::toJson_info()
{

}

void TestUtils::toJson_majorClass_data()
{

}

void TestUtils::toJson_majorClass()
{

}

void TestUtils::toJson_minorClass_data()
{

}

void TestUtils::toJson_minorClass()
{

}

void TestUtils::toJson_serviceClasses_data()
{

}

void TestUtils::toJson_serviceClasses()
{

}

void TestUtils::toJson_uuids_data()
{
    QTest::addColumn<QList<QBluetoothUuid>>("list");
    QTest::addColumn<QJsonArray>("expected");

    QList<QBluetoothUuid> list;
    QJsonArray array;
    QTest::addRow("empty") << list << array;

    list.append(QBluetoothUuid());
    array.append(QBluetoothUuid().toString());
    QTest::addRow("null") << list << array;

    list.append(QBluetoothUuid(QBluetoothUuid::DeviceName));
    array.append(QBluetoothUuid(QBluetoothUuid::DeviceName).toString());
    QTest::addRow("deviceName") << list << array;

    list.append(QBluetoothUuid(QUuid(POKIT_SERVICE_MULTIMETER)));
    array.append(QUuid(POKIT_SERVICE_MULTIMETER).toString());
    QTest::addRow("multimeter") << list << array;

    const QUuid randomUuid = QUuid::createUuid();
    list.append(QBluetoothUuid(randomUuid));
    array.append(randomUuid.toString());
    QTest::addRow("random") << list << array;
}

void TestUtils::toJson_uuids()
{
    QFETCH(QList<QBluetoothUuid>, list);
    QFETCH(QJsonArray, expected);
    QCOMPARE(toJson(list), expected);
}

void TestUtils::toJson_manufacturerData_data()
{

}

void TestUtils::toJson_manufacturerData()
{

}

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
    QTest::addRow("-1")   <<   -1 << QString();
    QTest::addRow("999")  <<  999 << QString();
}

void TestUtils::toString_majorClass()
{
    QFETCH(int, majorClass);
    QFETCH(QString, expected);
    QCOMPARE(toString(static_cast<QBluetoothDeviceInfo::MajorDeviceClass>(majorClass)), expected);
}

void TestUtils::toString_minorClass_data()
{
    QTest::addColumn<int>("majorClass");
    QTest::addColumn<quint8>("minorClass");
    QTest::addColumn<QString>("expected");

    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    #define MAJOR_CLASS_NETWORK_DEVICE LANAccessDevice // Deprecated since Qt 5.13.
    #else
    #define MAJOR_CLASS_NETWORK_DEVICE NetworkDevice // Added in Qt 5.13.
    #endif

    // Test all happy paths.
    #define ADD_ROW(majorClass, minorClass) QTest::addRow(#majorClass ":" #minorClass) \
        << (int)QBluetoothDeviceInfo::majorClass << (quint8)QBluetoothDeviceInfo::minorClass \
        << QString::fromLatin1(#minorClass)

    ADD_ROW(MiscellaneousDevice, UncategorizedMiscellaneous);

    ADD_ROW(ComputerDevice, UncategorizedComputer);
    ADD_ROW(ComputerDevice, DesktopComputer);
    ADD_ROW(ComputerDevice, ServerComputer);
    ADD_ROW(ComputerDevice, LaptopComputer);
    ADD_ROW(ComputerDevice, HandheldClamShellComputer);
    ADD_ROW(ComputerDevice, HandheldComputer);
    ADD_ROW(ComputerDevice, WearableComputer);

    ADD_ROW(PhoneDevice, UncategorizedPhone);
    ADD_ROW(PhoneDevice, CellularPhone);
    ADD_ROW(PhoneDevice, CordlessPhone);
    ADD_ROW(PhoneDevice, SmartPhone);
    ADD_ROW(PhoneDevice, WiredModemOrVoiceGatewayPhone);
    ADD_ROW(PhoneDevice, CommonIsdnAccessPhone);

    ADD_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkFullService);
    ADD_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorOne);
    ADD_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorTwo);
    ADD_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorThree);
    ADD_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorFour);
    ADD_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorFive);
    ADD_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorSix);
    ADD_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkNoService);

    ADD_ROW(AudioVideoDevice, UncategorizedAudioVideoDevice);
    ADD_ROW(AudioVideoDevice, WearableHeadsetDevice);
    ADD_ROW(AudioVideoDevice, HandsFreeDevice);
    ADD_ROW(AudioVideoDevice, Microphone);
    ADD_ROW(AudioVideoDevice, Loudspeaker);
    ADD_ROW(AudioVideoDevice, Headphones);
    ADD_ROW(AudioVideoDevice, PortableAudioDevice);
    ADD_ROW(AudioVideoDevice, CarAudio);
    ADD_ROW(AudioVideoDevice, SetTopBox);
    ADD_ROW(AudioVideoDevice, HiFiAudioDevice);
    ADD_ROW(AudioVideoDevice, Vcr);
    ADD_ROW(AudioVideoDevice, VideoCamera);
    ADD_ROW(AudioVideoDevice, Camcorder);
    ADD_ROW(AudioVideoDevice, VideoMonitor);
    ADD_ROW(AudioVideoDevice, VideoDisplayAndLoudspeaker);
    ADD_ROW(AudioVideoDevice, VideoConferencing);
    ADD_ROW(AudioVideoDevice, GamingDevice);

    ADD_ROW(PeripheralDevice, UncategorizedPeripheral);
    ADD_ROW(PeripheralDevice, KeyboardPeripheral);
    ADD_ROW(PeripheralDevice, PointingDevicePeripheral);
    ADD_ROW(PeripheralDevice, KeyboardWithPointingDevicePeripheral);
    ADD_ROW(PeripheralDevice, JoystickPeripheral);
    ADD_ROW(PeripheralDevice, GamepadPeripheral);
    ADD_ROW(PeripheralDevice, RemoteControlPeripheral);
    ADD_ROW(PeripheralDevice, SensingDevicePeripheral);
    ADD_ROW(PeripheralDevice, DigitizerTabletPeripheral);
    ADD_ROW(PeripheralDevice, CardReaderPeripheral);

    ADD_ROW(ImagingDevice, UncategorizedImagingDevice);
    ADD_ROW(ImagingDevice, ImageDisplay);
    ADD_ROW(ImagingDevice, ImageCamera);
    ADD_ROW(ImagingDevice, ImageScanner);
    ADD_ROW(ImagingDevice, ImagePrinter);

    ADD_ROW(WearableDevice, UncategorizedWearableDevice);
    ADD_ROW(WearableDevice, WearableWristWatch);
    ADD_ROW(WearableDevice, WearablePager);
    ADD_ROW(WearableDevice, WearableJacket);
    ADD_ROW(WearableDevice, WearableHelmet);
    ADD_ROW(WearableDevice, WearableGlasses);

    ADD_ROW(ToyDevice, UncategorizedToy);
    ADD_ROW(ToyDevice, ToyRobot);
    ADD_ROW(ToyDevice, ToyVehicle);
    ADD_ROW(ToyDevice, ToyDoll);
    ADD_ROW(ToyDevice, ToyController);
    ADD_ROW(ToyDevice, ToyGame);

    ADD_ROW(HealthDevice, UncategorizedHealthDevice);
    ADD_ROW(HealthDevice, HealthBloodPressureMonitor);
    ADD_ROW(HealthDevice, HealthThermometer);
    ADD_ROW(HealthDevice, HealthWeightScale);
    ADD_ROW(HealthDevice, HealthGlucoseMeter);
    ADD_ROW(HealthDevice, HealthPulseOximeter);
    ADD_ROW(HealthDevice, HealthDataDisplay);
    ADD_ROW(HealthDevice, HealthStepCounter);
    #undef ADD_ROW

    // Test that all major classs fall through on out-of-range minor classes.
    #define ADD_ROW(majorClass) QTest::addRow(#majorClass ".invalid") \
        << (int)QBluetoothDeviceInfo::majorClass << (quint8)999 << QString()
    ADD_ROW(MiscellaneousDevice);
    ADD_ROW(ComputerDevice);
    ADD_ROW(PhoneDevice);
    ADD_ROW(MAJOR_CLASS_NETWORK_DEVICE);
    ADD_ROW(AudioVideoDevice);
    ADD_ROW(PeripheralDevice);
    ADD_ROW(ImagingDevice);
    ADD_ROW(WearableDevice);
    ADD_ROW(ToyDevice);
    ADD_ROW(HealthDevice);
    ADD_ROW(UncategorizedDevice);
    #undef ADD_ROW
    #undef MAJOR_CLASS_NETWORK_DEVICE

    // There are no minor classed for UncategorizedDevice.
    QTest::addRow("UncategorizedDevice:0")
        << (int)QBluetoothDeviceInfo::UncategorizedDevice << (quint8)0 << QString();
    QTest::addRow("UncategorizedDevice:1")
        << (int)QBluetoothDeviceInfo::UncategorizedDevice << (quint8)1 << QString();

    // Test some other, semi-random, invalid combinations.
    QTest::addRow("ToyDevice.123")
        << (int)QBluetoothDeviceInfo::ToyDevice << (quint8)123 << QString();
    QTest::addRow("HealthDevice:999")
        << (int)QBluetoothDeviceInfo::ToyDevice << (quint8)999 << QString();
    QTest::addRow("-999.WearableHelmet")
        << -999 << (quint8)QBluetoothDeviceInfo::WearableHelmet << QString();
    QTest::addRow(  "-1.WearableHelmet")
        << -1 << (quint8)QBluetoothDeviceInfo::WearableHelmet << QString();
    QTest::addRow( "999.WearableHelmet")
        << 999 << (quint8)QBluetoothDeviceInfo::WearableHelmet << QString();
}

void TestUtils::toString_minorClass()
{
    QFETCH(int, majorClass);
    QFETCH(quint8, minorClass);
    QFETCH(QString, expected);
    QCOMPARE(toString(static_cast<QBluetoothDeviceInfo::MajorDeviceClass>(majorClass), minorClass),
             expected);
}

QTEST_MAIN(TestUtils)
