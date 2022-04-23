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

#include <qtpokit/utils.h>

#include <QBluetoothAddress>
#include <QJsonDocument>

#define POKIT_SERVICE_MULTIMETER "e7481d2f-5781-442e-bb9a-fd4e3441dadc"
#define POKIT_SERVICE_DSO        "1569801e-1425-4a7a-b617-a4f4ed719de6"

typedef QMultiHash<quint16, QByteArray> ManufacturerData;
Q_DECLARE_METATYPE(ManufacturerData)
Q_DECLARE_METATYPE(QBluetoothDeviceInfo::CoreConfiguration)
Q_DECLARE_METATYPE(QBluetoothDeviceInfo::ServiceClasses)

// Serialiser for QCOMPARE to output QJsonArray objects on test failures.
char *toString(const QJsonArray &array)
{
    return qstrdup(("QJsonArray(" + QJsonDocument(array).toJson(QJsonDocument::Compact) + ")").constData());
}

// Serialiser for QCOMPARE to output QJsonObject objects on test failures.
char *toString(const QJsonObject &object)
{
    return qstrdup(("QJsonObject(" + QJsonDocument(object).toJson(QJsonDocument::Compact) + ")").constData());
}

void TestUtils::toJson_info_data()
{
    QTest::addColumn<QBluetoothDeviceInfo>("info");
    QTest::addColumn<QJsonObject>("expected");

    QTest::newRow("invalid") << QBluetoothDeviceInfo() << QJsonObject();

    const QUuid randomUuid = QUuid::createUuid();
    QBluetoothDeviceInfo info(QBluetoothUuid(randomUuid), QLatin1String("foo"), 0);
    QTest::newRow("uuid")
        << info
        << QJsonObject{
            { QString::fromLatin1("address"), QLatin1String("00:00:00:00:00:00") },
            { QString::fromLatin1("deviceUuid"), randomUuid.toString() },
            { QString::fromLatin1("isCached"), false },
            { QString::fromLatin1("majorDeviceClass"), QLatin1String("MiscellaneousDevice") },
            { QString::fromLatin1("minorDeviceClass"), QLatin1String("UncategorizedMiscellaneous") },
            { QString::fromLatin1("name"), QLatin1String("foo") },
            { QString::fromLatin1("signalStrength"), 0 },
        };

    info.setCached(true);
    QTest::newRow("cached")
        << info
        << QJsonObject{
            { QString::fromLatin1("address"), QLatin1String("00:00:00:00:00:00") },
            { QString::fromLatin1("deviceUuid"), randomUuid.toString() },
            { QString::fromLatin1("isCached"), true },
            { QString::fromLatin1("majorDeviceClass"), QLatin1String("MiscellaneousDevice") },
            { QString::fromLatin1("minorDeviceClass"), QLatin1String("UncategorizedMiscellaneous") },
            { QString::fromLatin1("name"), QLatin1String("foo") },
            { QString::fromLatin1("signalStrength"), 0 },
        };

    info.setCoreConfigurations(QBluetoothDeviceInfo::LowEnergyCoreConfiguration);
    QTest::newRow("coreConfig")
        << info
        << QJsonObject{
            { QString::fromLatin1("address"), QLatin1String("00:00:00:00:00:00") },
            { QString::fromLatin1("coreConfiguration"), QJsonArray{
                QString::fromLatin1("LowEnergyCoreConfiguration"),
            } },
            { QString::fromLatin1("deviceUuid"), randomUuid.toString() },
            { QString::fromLatin1("isCached"), true },
            { QString::fromLatin1("majorDeviceClass"), QLatin1String("MiscellaneousDevice") },
            { QString::fromLatin1("minorDeviceClass"), QLatin1String("UncategorizedMiscellaneous") },
            { QString::fromLatin1("name"), QLatin1String("foo") },
            { QString::fromLatin1("signalStrength"), 0 },
        };

    // QBluetoothDeviceInfo::setManufacturerData added in Qt 5.12.
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    info.setManufacturerData(0, QByteArray("zero"));
    info.setManufacturerData(0, QByteArray("multi-zero"));
    info.setManufacturerData(1, QByteArray("one"));
    QTest::newRow("manufacturerData")
        << info
        << QJsonObject{
            { QString::fromLatin1("address"), QLatin1String("00:00:00:00:00:00") },
            { QString::fromLatin1("coreConfiguration"), QJsonArray{
                QString::fromLatin1("LowEnergyCoreConfiguration"),
            } },
            { QString::fromLatin1("deviceUuid"), randomUuid.toString() },
            { QString::fromLatin1("isCached"), true },
            { QString::fromLatin1("manufacturerData"), QJsonObject{
                { QString::fromLatin1("0"), QJsonArray{
                    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
                    QString::fromLatin1("emVybw=="),  // Multi support added in Qt 5.14.
                    #endif
                    QString::fromLatin1("bXVsdGktemVybw=="),
                } },
                { QString::fromLatin1("1"), QJsonArray{QString::fromLatin1("b25l") } },
            } },
            { QString::fromLatin1("majorDeviceClass"), QLatin1String("MiscellaneousDevice") },
            { QString::fromLatin1("minorDeviceClass"), QLatin1String("UncategorizedMiscellaneous") },
            { QString::fromLatin1("name"), QLatin1String("foo") },
            { QString::fromLatin1("signalStrength"), 0 },
        };
    #endif

    info.setRssi(-123);
    QTest::newRow("rssi")
        << info
        << QJsonObject{
            { QString::fromLatin1("address"), QLatin1String("00:00:00:00:00:00") },
            { QString::fromLatin1("coreConfiguration"), QJsonArray{
                QString::fromLatin1("LowEnergyCoreConfiguration"),
            } },
            { QString::fromLatin1("deviceUuid"), randomUuid.toString() },
            { QString::fromLatin1("isCached"), true },
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // ManufacturerData added in Qt 5.12.
            { QString::fromLatin1("manufacturerData"), QJsonObject{
                { QString::fromLatin1("0"), QJsonArray{
                    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
                    QString::fromLatin1("emVybw=="),  // Multi support added in Qt 5.14.
                    #endif
                    QString::fromLatin1("bXVsdGktemVybw=="),
                } },
                { QString::fromLatin1("1"), QJsonArray{QString::fromLatin1("b25l") } },
            } },
            #endif
            { QString::fromLatin1("majorDeviceClass"), QLatin1String("MiscellaneousDevice") },
            { QString::fromLatin1("minorDeviceClass"), QLatin1String("UncategorizedMiscellaneous") },
            { QString::fromLatin1("name"), QLatin1String("foo") },
            { QString::fromLatin1("signalStrength"), -123 },
        };

    info.setServiceUuids({QUuid(POKIT_SERVICE_MULTIMETER), QUuid(POKIT_SERVICE_DSO)}
        #if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
        , QBluetoothDeviceInfo::DataUnavailable // Required prior to 5.13, but deprecated in 5.13.
        #endif
    );
    QTest::newRow("rssi")
        << info
        << QJsonObject{
            { QString::fromLatin1("address"), QLatin1String("00:00:00:00:00:00") },
            { QString::fromLatin1("coreConfiguration"), QJsonArray{
                QString::fromLatin1("LowEnergyCoreConfiguration"),
            } },
            { QString::fromLatin1("deviceUuid"), randomUuid.toString() },
            { QString::fromLatin1("isCached"), true },
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // ManufacturerData added in Qt 5.12.
            { QString::fromLatin1("manufacturerData"), QJsonObject{
                { QString::fromLatin1("0"), QJsonArray{
                    #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
                    QString::fromLatin1("emVybw=="),  // Multi support added in Qt 5.14.
                    #endif
                    QString::fromLatin1("bXVsdGktemVybw=="),
                } },
                { QString::fromLatin1("1"), QJsonArray{QString::fromLatin1("b25l") } },
            } },
            #endif
            { QString::fromLatin1("majorDeviceClass"), QLatin1String("MiscellaneousDevice") },
            { QString::fromLatin1("minorDeviceClass"), QLatin1String("UncategorizedMiscellaneous") },
            { QString::fromLatin1("name"), QLatin1String("foo") },
            { QString::fromLatin1("serviceUuids"), QJsonArray{
                QString::fromLatin1("{e7481d2f-5781-442e-bb9a-fd4e3441dadc}"),
                QString::fromLatin1("{1569801e-1425-4a7a-b617-a4f4ed719de6}")
            } },
            { QString::fromLatin1("signalStrength"), -123 },
        };

    const QBluetoothAddress address(QLatin1String("12:34:56:78:9A:BC"));
    QTest::newRow("address")
        << QBluetoothDeviceInfo(address, QLatin1String("bar"), 0xffff)
        << QJsonObject{
            { QString::fromLatin1("address"), address.toString() },
            { QString::fromLatin1("isCached"), false },
            { QString::fromLatin1("majorDeviceClass"), QLatin1String("UncategorizedDevice") },
            { QString::fromLatin1("minorDeviceClass"), 63 },
            { QString::fromLatin1("name"), QLatin1String("bar") },
            { QString::fromLatin1("serviceClasses"), QJsonArray{
                QString::fromLatin1("PositioningService"),
                QString::fromLatin1("NetworkingService"),
                QString::fromLatin1("RenderingService"),
            } },
            { QString::fromLatin1("signalStrength"), 0 },
        };

}

void TestUtils::toJson_info()
{
    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(QJsonObject, expected);
    QCOMPARE(toJson(info), expected);
}

void TestUtils::toJson_coreConfig_data()
{
    QTest::addColumn<QBluetoothDeviceInfo::CoreConfiguration>("config");
    QTest::addColumn<QJsonArray>("expected");

    #define ADD_ROW(config) QTest::newRow(#config) \
        << QBluetoothDeviceInfo::CoreConfiguration(QBluetoothDeviceInfo::config) \
        << QJsonArray{ QLatin1String(#config) };
    ADD_ROW(UnknownCoreConfiguration);
    ADD_ROW(LowEnergyCoreConfiguration);
    ADD_ROW(BaseRateCoreConfiguration);
    #undef ADD_ROW

    // BaseRateAndLowEnergy is a combination of LowEnergy and BaseRate core configurations.
    QTest::newRow("BaseRateAndLowEnergyCoreConfiguration") \
        << QBluetoothDeviceInfo::CoreConfiguration(
            QBluetoothDeviceInfo::BaseRateAndLowEnergyCoreConfiguration) \
        << QJsonArray{
            QLatin1String("LowEnergyCoreConfiguration"),
            QLatin1String("BaseRateCoreConfiguration"),
        };
}

void TestUtils::toJson_coreConfig()
{
    QFETCH(QBluetoothDeviceInfo::CoreConfiguration, config);
    QFETCH(QJsonArray, expected);
    QCOMPARE(toJson(config), expected);
}

void TestUtils::toJson_majorClass_data()
{
    toString_majorClass_data();
}

void TestUtils::toJson_majorClass()
{
    QFETCH(int, majorClass);
    QFETCH(QString, expected);
    QCOMPARE(toJson(static_cast<QBluetoothDeviceInfo::MajorDeviceClass>(majorClass)),
        (expected.isNull()) ? QJsonValue(majorClass) : QJsonValue(expected));
}

void TestUtils::toJson_minorClass_data()
{
    toString_minorClass_data();
}

void TestUtils::toJson_minorClass()
{
    QFETCH(int, majorClass);
    QFETCH(quint8, minorClass);
    QFETCH(QString, expected);
    QCOMPARE(toJson(static_cast<QBluetoothDeviceInfo::MajorDeviceClass>(majorClass), minorClass),
             (expected.isNull()) ? QJsonValue(minorClass) : QJsonValue(expected));
}

void TestUtils::toJson_serviceClasses_data()
{
    QTest::addColumn<QBluetoothDeviceInfo::ServiceClasses>("classes");
    QTest::addColumn<QJsonArray>("expected");

    QTest::newRow("empty") << QBluetoothDeviceInfo::ServiceClasses() << QJsonArray();

    #define ADD_ROW(serviceClass) QTest::newRow(#serviceClass) \
        << QBluetoothDeviceInfo::ServiceClasses(QBluetoothDeviceInfo::serviceClass) \
        << QJsonArray{ QLatin1String(#serviceClass) };
    ADD_ROW(PositioningService);
    ADD_ROW(NetworkingService);
    ADD_ROW(RenderingService);
    ADD_ROW(CapturingService);
    ADD_ROW(ObjectTransferService);
    ADD_ROW(AudioService);
    ADD_ROW(TelephonyService);
    ADD_ROW(InformationService);
    #undef ADD_ROW

    QTest::newRow("all")
        << QBluetoothDeviceInfo::ServiceClasses(
            QBluetoothDeviceInfo::PositioningService|
            QBluetoothDeviceInfo::NetworkingService|
            QBluetoothDeviceInfo::RenderingService|
            QBluetoothDeviceInfo::CapturingService|
            QBluetoothDeviceInfo::ObjectTransferService|
            QBluetoothDeviceInfo::AudioService|
            QBluetoothDeviceInfo::TelephonyService|
            QBluetoothDeviceInfo::InformationService)
        << QJsonArray{
            QString::fromLatin1("PositioningService"),
            QString::fromLatin1("NetworkingService"),
            QString::fromLatin1("RenderingService"),
            QString::fromLatin1("CapturingService"),
            QString::fromLatin1("ObjectTransferService"),
            QString::fromLatin1("AudioService"),
            QString::fromLatin1("TelephonyService"),
            QString::fromLatin1("InformationService"),
        };
}

void TestUtils::toJson_serviceClasses()
{
    QFETCH(QBluetoothDeviceInfo::ServiceClasses, classes);
    QFETCH(QJsonArray, expected);
    QCOMPARE(toJson(classes), expected);
}

void TestUtils::toJson_uuids_list_data()
{
    QTest::addColumn<QList<QBluetoothUuid>>("list");
    QTest::addColumn<QJsonArray>("expected");

    QList<QBluetoothUuid> list;
    QJsonArray array;
    QTest::addRow("empty") << list << array;

    list.append(QBluetoothUuid());
    array.append(QBluetoothUuid().toString());
    QTest::addRow("null") << list << array;

    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    list.append(QBluetoothUuid(QBluetoothUuid::DeviceName));
    array.append(QBluetoothUuid(QBluetoothUuid::DeviceName).toString());
    #else
    list.append(QBluetoothUuid(QBluetoothUuid::CharacteristicType::DeviceName));
    array.append(QBluetoothUuid(QBluetoothUuid::CharacteristicType::DeviceName).toString());
    #endif
    QTest::addRow("deviceName") << list << array;

    list.append(QBluetoothUuid(QUuid(POKIT_SERVICE_MULTIMETER)));
    array.append(QUuid(POKIT_SERVICE_MULTIMETER).toString());
    QTest::addRow("multimeter") << list << array;

    const QUuid randomUuid = QUuid::createUuid();
    list.append(QBluetoothUuid(randomUuid));
    array.append(randomUuid.toString());
    QTest::addRow("random") << list << array;
}

void TestUtils::toJson_uuids_list()
{
    QFETCH(QList<QBluetoothUuid>, list);
    QFETCH(QJsonArray, expected);
    QCOMPARE(toJson(list), expected);
}

void TestUtils::toJson_uuids_vector_data()
{
    QTest::addColumn<QVector<QBluetoothUuid>>("vector");
    QTest::addColumn<QJsonArray>("expected");

    QVector<QBluetoothUuid> vector;
    QJsonArray array;
    QTest::addRow("empty") << vector << array;

    vector.append(QBluetoothUuid());
    array.append(QBluetoothUuid().toString());
    QTest::addRow("null") << vector << array;

    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    vector.append(QBluetoothUuid(QBluetoothUuid::DeviceName));
    array.append(QBluetoothUuid(QBluetoothUuid::DeviceName).toString());
    #else
    vector.append(QBluetoothUuid(QBluetoothUuid::CharacteristicType::DeviceName));
    array.append(QBluetoothUuid(QBluetoothUuid::CharacteristicType::DeviceName).toString());
    #endif
    QTest::addRow("deviceName") << vector << array;

    vector.append(QBluetoothUuid(QUuid(POKIT_SERVICE_MULTIMETER)));
    array.append(QUuid(POKIT_SERVICE_MULTIMETER).toString());
    QTest::addRow("multimeter") << vector << array;

    const QUuid randomUuid = QUuid::createUuid();
    vector.append(QBluetoothUuid(randomUuid));
    array.append(randomUuid.toString());
    QTest::addRow("random") << vector << array;
}

void TestUtils::toJson_uuids_vector()
{
    QFETCH(QVector<QBluetoothUuid>, vector);
    QFETCH(QJsonArray, expected);
    QCOMPARE(toJson(vector), expected);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
void TestUtils::toJson_manufacturerData_data()
{
    QTest::addColumn<ManufacturerData>("data");
    QTest::addColumn<QJsonObject>("expected");

    ManufacturerData data;
    QJsonObject object;
    QTest::newRow("empty") << data << object;

    data.insert(0, QByteArray("zero"));
    object.insert(QString::number(0), QJsonArray{
        QString::fromLatin1(QByteArray("zero").toBase64()),
    });
    QTest::newRow("zero") << data << object;

    data.insert(0, QByteArray("multi-zero"));
    object.insert(QString::number(0), QJsonArray{
        QString::fromLatin1(QByteArray("zero").toBase64()),
        QString::fromLatin1(QByteArray("multi-zero").toBase64()),
    });
    QTest::newRow("multi-zero") << data << object;

    data.insert(999, QByteArray(9, 3));
    object.insert(QString::number(999), QJsonArray{
        QString::fromLatin1(QByteArray(9, 3).toBase64()),
    });
    QTest::newRow("binary") << data << object;
}

void TestUtils::toJson_manufacturerData()
{
    QFETCH(ManufacturerData, data);
    QFETCH(QJsonObject, expected);
    QCOMPARE(toJson(data), expected);
}
#endif

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
