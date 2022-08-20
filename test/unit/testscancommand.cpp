// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testscancommand.h"

#include "scancommand.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

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

class BaseCommand : public AbstractCommand
{
public:
    BaseCommand() : AbstractCommand(nullptr) { }

    bool start() override { return true; }

    void deviceDiscovered(const QBluetoothDeviceInfo &info) override { Q_UNUSED(info); }

    void deviceDiscoveryFinished() override { }
};

void TestScanCommand::requiredOptions()
{
    ScanCommand command(this);
    QCommandLineParser parser;
    QVERIFY(command.requiredOptions(parser).isEmpty());
}

void TestScanCommand::supportedOptions()
{
    BaseCommand base;
    ScanCommand command(this);
    QCommandLineParser parser;
    QCOMPARE(command.supportedOptions(parser), base.supportedOptions(parser));
}

void TestScanCommand::processOptions()
{
    ScanCommand command(this);
    QCommandLineParser parser;
    QCOMPARE(command.processOptions(parser), QStringList{});
}

void TestScanCommand::start()
{
    ScanCommand command(this);
    QTest::ignoreMessage(QtInfoMsg, "Scanning for Pokit devices...");
    command.start();
}

void TestScanCommand::deviceDiscovered()
{
    /// \todo Verify the output format.
}

void TestScanCommand::deviceUpdated()
{
    /// \todo Verify the output format.
}

void TestScanCommand::deviceDiscoveryFinished()
{
    ScanCommand command(this);
    command.deviceDiscoveryFinished(); // Just logs a debug message, and exits.
}

void TestScanCommand::toJson_info_data()
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

void TestScanCommand::toJson_info()
{
    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(QJsonObject, expected);
    QCOMPARE(ScanCommand::toJson(info), expected);
}

void TestScanCommand::toJson_coreConfig_data()
{
    QTest::addColumn<QBluetoothDeviceInfo::CoreConfiguration>("config");
    QTest::addColumn<QJsonArray>("expected");

    #define QTPOKIT_ADD_TEST_ROW(config) QTest::newRow(#config) \
        << QBluetoothDeviceInfo::CoreConfiguration(QBluetoothDeviceInfo::config) \
        << QJsonArray{ QLatin1String(#config) }
    QTPOKIT_ADD_TEST_ROW(UnknownCoreConfiguration);
    QTPOKIT_ADD_TEST_ROW(LowEnergyCoreConfiguration);
    QTPOKIT_ADD_TEST_ROW(BaseRateCoreConfiguration);
    #undef QTPOKIT_ADD_TEST_ROW

    // BaseRateAndLowEnergy is a combination of LowEnergy and BaseRate core configurations.
    QTest::newRow("BaseRateAndLowEnergyCoreConfiguration") \
        << QBluetoothDeviceInfo::CoreConfiguration(
            QBluetoothDeviceInfo::BaseRateAndLowEnergyCoreConfiguration) \
        << QJsonArray{
            QLatin1String("LowEnergyCoreConfiguration"),
            QLatin1String("BaseRateCoreConfiguration"),
        };
}

void TestScanCommand::toJson_coreConfig()
{
    QFETCH(QBluetoothDeviceInfo::CoreConfiguration, config);
    QFETCH(QJsonArray, expected);
    QCOMPARE(ScanCommand::toJson(config), expected);
}

void TestScanCommand::toJson_majorClass_data()
{
    toString_majorClass_data();
}

void TestScanCommand::toJson_majorClass()
{
    QFETCH(int, majorClass);
    QFETCH(QString, expected);
    QCOMPARE(ScanCommand::toJson(static_cast<QBluetoothDeviceInfo::MajorDeviceClass>(majorClass)),
        (expected.isNull()) ? QJsonValue(majorClass) : QJsonValue(expected));
}

void TestScanCommand::toJson_minorClass_data()
{
    toString_minorClass_data();
}

void TestScanCommand::toJson_minorClass()
{
    QFETCH(int, majorClass);
    QFETCH(quint8, minorClass);
    QFETCH(QString, expected);
    QCOMPARE(ScanCommand::toJson(static_cast<QBluetoothDeviceInfo::MajorDeviceClass>(majorClass), minorClass),
             (expected.isNull()) ? QJsonValue(minorClass) : QJsonValue(expected));
}

void TestScanCommand::toJson_serviceClasses_data()
{
    QTest::addColumn<QBluetoothDeviceInfo::ServiceClasses>("classes");
    QTest::addColumn<QJsonArray>("expected");

    QTest::newRow("empty") << QBluetoothDeviceInfo::ServiceClasses() << QJsonArray();

    #define QTPOKIT_ADD_TEST_ROW(serviceClass) QTest::newRow(#serviceClass) \
        << QBluetoothDeviceInfo::ServiceClasses(QBluetoothDeviceInfo::serviceClass) \
        << QJsonArray{ QLatin1String(#serviceClass) }
    QTPOKIT_ADD_TEST_ROW(PositioningService);
    QTPOKIT_ADD_TEST_ROW(NetworkingService);
    QTPOKIT_ADD_TEST_ROW(RenderingService);
    QTPOKIT_ADD_TEST_ROW(CapturingService);
    QTPOKIT_ADD_TEST_ROW(ObjectTransferService);
    QTPOKIT_ADD_TEST_ROW(AudioService);
    QTPOKIT_ADD_TEST_ROW(TelephonyService);
    QTPOKIT_ADD_TEST_ROW(InformationService);
    #undef QTPOKIT_ADD_TEST_ROW

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

void TestScanCommand::toJson_serviceClasses()
{
    QFETCH(QBluetoothDeviceInfo::ServiceClasses, classes);
    QFETCH(QJsonArray, expected);
    QCOMPARE(ScanCommand::toJson(classes), expected);
}

void TestScanCommand::toJson_uuids_list_data()
{
    QTest::addColumn<QList<QBluetoothUuid>>("list");
    QTest::addColumn<QJsonArray>("expected");

    QList<QBluetoothUuid> list;
    QJsonArray array;
    QTest::addRow("empty") << list << array;

    list.append(QBluetoothUuid());
    array.append(QBluetoothUuid().toString());
    QTest::addRow("null") << list << array;

    list.append(QBluetoothUuid(QBluetoothUuid::CharacteristicType::DeviceName));
    array.append(QBluetoothUuid(QBluetoothUuid::CharacteristicType::DeviceName).toString());
    QTest::addRow("deviceName") << list << array;

    list.append(QBluetoothUuid(QUuid(POKIT_SERVICE_MULTIMETER)));
    array.append(QUuid(POKIT_SERVICE_MULTIMETER).toString());
    QTest::addRow("multimeter") << list << array;

    const QUuid randomUuid = QUuid::createUuid();
    list.append(QBluetoothUuid(randomUuid));
    array.append(randomUuid.toString());
    QTest::addRow("random") << list << array;
}

void TestScanCommand::toJson_uuids_list()
{
    QFETCH(QList<QBluetoothUuid>, list);
    QFETCH(QJsonArray, expected);
    QCOMPARE(ScanCommand::toJson(list), expected);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
void TestScanCommand::toJson_manufacturerData_data()
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

void TestScanCommand::toJson_manufacturerData()
{
    QFETCH(ManufacturerData, data);
    QFETCH(QJsonObject, expected);
    QCOMPARE(ScanCommand::toJson(data), expected);
}
#endif

void TestScanCommand::toString_majorClass_data()
{
    QTest::addColumn<int>("majorClass");
    QTest::addColumn<QString>("expected");

    // Test all happy paths.
    #define QTPOKIT_ADD_TEST_ROW(majorClass) QTest::addRow(#majorClass) \
        << (int)QBluetoothDeviceInfo::majorClass << QString::fromLatin1(#majorClass)
    QTPOKIT_ADD_TEST_ROW(MiscellaneousDevice);
    QTPOKIT_ADD_TEST_ROW(ComputerDevice);
    QTPOKIT_ADD_TEST_ROW(PhoneDevice);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    QTPOKIT_ADD_TEST_ROW(LANAccessDevice); // Deprecated since Qt 5.13.
    #else
    QTPOKIT_ADD_TEST_ROW(NetworkDevice); // Added in Qt 5.13.
    #endif
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice);
    QTPOKIT_ADD_TEST_ROW(ImagingDevice);
    QTPOKIT_ADD_TEST_ROW(WearableDevice);
    QTPOKIT_ADD_TEST_ROW(ToyDevice);
    QTPOKIT_ADD_TEST_ROW(HealthDevice);
    QTPOKIT_ADD_TEST_ROW(UncategorizedDevice);
    #undef QTPOKIT_ADD_TEST_ROW

    // Test unhappy paths; these should all safely return null strings.
    QTest::addRow("-999") << -999 << QString();
    QTest::addRow("-1")   <<   -1 << QString();
    QTest::addRow("999")  <<  999 << QString();
}

void TestScanCommand::toString_majorClass()
{
    QFETCH(int, majorClass);
    QFETCH(QString, expected);
    QCOMPARE(ScanCommand::toString(static_cast<QBluetoothDeviceInfo::MajorDeviceClass>(majorClass)), expected);
}

void TestScanCommand::toString_minorClass_data()
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
    #define QTPOKIT_ADD_TEST_ROW(majorClass, minorClass) QTest::addRow(#majorClass ":" #minorClass) \
        << (int)QBluetoothDeviceInfo::majorClass << (quint8)QBluetoothDeviceInfo::minorClass \
        << QString::fromLatin1(#minorClass)

    QTPOKIT_ADD_TEST_ROW(MiscellaneousDevice, UncategorizedMiscellaneous);

    QTPOKIT_ADD_TEST_ROW(ComputerDevice, UncategorizedComputer);
    QTPOKIT_ADD_TEST_ROW(ComputerDevice, DesktopComputer);
    QTPOKIT_ADD_TEST_ROW(ComputerDevice, ServerComputer);
    QTPOKIT_ADD_TEST_ROW(ComputerDevice, LaptopComputer);
    QTPOKIT_ADD_TEST_ROW(ComputerDevice, HandheldClamShellComputer);
    QTPOKIT_ADD_TEST_ROW(ComputerDevice, HandheldComputer);
    QTPOKIT_ADD_TEST_ROW(ComputerDevice, WearableComputer);

    QTPOKIT_ADD_TEST_ROW(PhoneDevice, UncategorizedPhone);
    QTPOKIT_ADD_TEST_ROW(PhoneDevice, CellularPhone);
    QTPOKIT_ADD_TEST_ROW(PhoneDevice, CordlessPhone);
    QTPOKIT_ADD_TEST_ROW(PhoneDevice, SmartPhone);
    QTPOKIT_ADD_TEST_ROW(PhoneDevice, WiredModemOrVoiceGatewayPhone);
    QTPOKIT_ADD_TEST_ROW(PhoneDevice, CommonIsdnAccessPhone);

    QTPOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkFullService);
    QTPOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorOne);
    QTPOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorTwo);
    QTPOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorThree);
    QTPOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorFour);
    QTPOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorFive);
    QTPOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorSix);
    QTPOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkNoService);

    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, UncategorizedAudioVideoDevice);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, WearableHeadsetDevice);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, HandsFreeDevice);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, Microphone);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, Loudspeaker);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, Headphones);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, PortableAudioDevice);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, CarAudio);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, SetTopBox);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, HiFiAudioDevice);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, Vcr);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, VideoCamera);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, Camcorder);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, VideoMonitor);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, VideoDisplayAndLoudspeaker);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, VideoConferencing);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice, GamingDevice);

    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, UncategorizedPeripheral);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, KeyboardPeripheral);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, PointingDevicePeripheral);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, KeyboardWithPointingDevicePeripheral);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, JoystickPeripheral);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, GamepadPeripheral);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, RemoteControlPeripheral);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, SensingDevicePeripheral);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, DigitizerTabletPeripheral);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice, CardReaderPeripheral);

    QTPOKIT_ADD_TEST_ROW(ImagingDevice, UncategorizedImagingDevice);
    QTPOKIT_ADD_TEST_ROW(ImagingDevice, ImageDisplay);
    QTPOKIT_ADD_TEST_ROW(ImagingDevice, ImageCamera);
    QTPOKIT_ADD_TEST_ROW(ImagingDevice, ImageScanner);
    QTPOKIT_ADD_TEST_ROW(ImagingDevice, ImagePrinter);

    QTPOKIT_ADD_TEST_ROW(WearableDevice, UncategorizedWearableDevice);
    QTPOKIT_ADD_TEST_ROW(WearableDevice, WearableWristWatch);
    QTPOKIT_ADD_TEST_ROW(WearableDevice, WearablePager);
    QTPOKIT_ADD_TEST_ROW(WearableDevice, WearableJacket);
    QTPOKIT_ADD_TEST_ROW(WearableDevice, WearableHelmet);
    QTPOKIT_ADD_TEST_ROW(WearableDevice, WearableGlasses);

    QTPOKIT_ADD_TEST_ROW(ToyDevice, UncategorizedToy);
    QTPOKIT_ADD_TEST_ROW(ToyDevice, ToyRobot);
    QTPOKIT_ADD_TEST_ROW(ToyDevice, ToyVehicle);
    QTPOKIT_ADD_TEST_ROW(ToyDevice, ToyDoll);
    QTPOKIT_ADD_TEST_ROW(ToyDevice, ToyController);
    QTPOKIT_ADD_TEST_ROW(ToyDevice, ToyGame);

    QTPOKIT_ADD_TEST_ROW(HealthDevice, UncategorizedHealthDevice);
    QTPOKIT_ADD_TEST_ROW(HealthDevice, HealthBloodPressureMonitor);
    QTPOKIT_ADD_TEST_ROW(HealthDevice, HealthThermometer);
    QTPOKIT_ADD_TEST_ROW(HealthDevice, HealthWeightScale);
    QTPOKIT_ADD_TEST_ROW(HealthDevice, HealthGlucoseMeter);
    QTPOKIT_ADD_TEST_ROW(HealthDevice, HealthPulseOximeter);
    QTPOKIT_ADD_TEST_ROW(HealthDevice, HealthDataDisplay);
    QTPOKIT_ADD_TEST_ROW(HealthDevice, HealthStepCounter);
    #undef QTPOKIT_ADD_TEST_ROW

    // Test that all major classs fall through on out-of-range minor classes.
    #define QTPOKIT_ADD_TEST_ROW(majorClass) QTest::addRow(#majorClass ".invalid") \
        << (int)QBluetoothDeviceInfo::majorClass << (quint8)999 << QString()
    QTPOKIT_ADD_TEST_ROW(MiscellaneousDevice);
    QTPOKIT_ADD_TEST_ROW(ComputerDevice);
    QTPOKIT_ADD_TEST_ROW(PhoneDevice);
    QTPOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE);
    QTPOKIT_ADD_TEST_ROW(AudioVideoDevice);
    QTPOKIT_ADD_TEST_ROW(PeripheralDevice);
    QTPOKIT_ADD_TEST_ROW(ImagingDevice);
    QTPOKIT_ADD_TEST_ROW(WearableDevice);
    QTPOKIT_ADD_TEST_ROW(ToyDevice);
    QTPOKIT_ADD_TEST_ROW(HealthDevice);
    QTPOKIT_ADD_TEST_ROW(UncategorizedDevice);
    #undef QTPOKIT_ADD_TEST_ROW
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

void TestScanCommand::toString_minorClass()
{
    QFETCH(int, majorClass);
    QFETCH(quint8, minorClass);
    QFETCH(QString, expected);
    QCOMPARE(ScanCommand::toString(static_cast<QBluetoothDeviceInfo::MajorDeviceClass>(majorClass), minorClass),
             expected);
}

void TestScanCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    ScanCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestScanCommand)
