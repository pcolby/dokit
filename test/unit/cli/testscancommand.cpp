// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testscancommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

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
Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
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
    BaseCommand(QObject * const parent = nullptr) : AbstractCommand(parent) { }

    bool start() override { return true; }

    void deviceDiscovered(const QBluetoothDeviceInfo &info) override { Q_UNUSED(info) }

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

void TestScanCommand::deviceDiscovered_data()
{
    QTest::addColumn<QList<QBluetoothDeviceInfo>>("infos");
    QTest::addColumn<AbstractCommand::OutputFormat>("format");

    QBluetoothDeviceInfo rssi(QBluetoothUuid(
        QStringLiteral("661d1ed3-8e28-4f2b-88da-a1c02fe98aed")), QStringLiteral("rssi"), 0);
    rssi.setRssi(12345);

    const QList<QBluetoothDeviceInfo> list{
        { QBluetoothAddress(0), QStringLiteral("addr0"), 0},
        { QBluetoothAddress(0xFFFFFFFFFFFFFFFF), QStringLiteral("addrMax"), 0xFFFFFFFF},
        { QBluetoothAddress(QStringLiteral("0123456789ABC")), QStringLiteral("addrStr"), 0},
        { QBluetoothUuid(QStringLiteral("5c0625b1-a46b-44f1-a6aa-058424ce69b0")), QStringLiteral("uuid1"), 123},
        { QBluetoothUuid(QStringLiteral("e8ee1747-1e43-4699-bac6-88cab02c109c")), QStringLiteral("uuid2"), 456},
        { QBluetoothUuid(QStringLiteral("381b1e3c-c25a-44a3-9230-2a737cf3f206")), QStringLiteral("uuid3"), 678 },
        rssi
    };

    #define DOKIT_ADD_TEST_ROW(name, list) \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".csv")))  << list << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".json"))) << list << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".txt")))  << list << AbstractCommand::OutputFormat::Text

    DOKIT_ADD_TEST_ROW(QStringLiteral("null"), QList<QBluetoothDeviceInfo>{ QBluetoothDeviceInfo() });

    for (const QBluetoothDeviceInfo &info: list) {
        DOKIT_ADD_TEST_ROW(info.name(), QList<QBluetoothDeviceInfo>{ info });
    }

    DOKIT_ADD_TEST_ROW(QStringLiteral("all"), list );
    #undef DOKIT_ADD_TEST_ROW
}

void TestScanCommand::deviceDiscovered()
{
    QFETCH(QList<QBluetoothDeviceInfo>, infos);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    const OutputStreamCapture capture(&std::cout);
    ScanCommand command;
    command.format = format;
    for (const QBluetoothDeviceInfo &info: infos) {
        command.deviceDiscovered(info);
    }
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestScanCommand::deviceUpdated_data()
{
    deviceDiscovered_data();
}

void TestScanCommand::deviceUpdated()
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    QSKIP("Not applicable before Qt version 5.12.");
#else
    QFETCH(QList<QBluetoothDeviceInfo>, infos);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    const OutputStreamCapture capture(&std::cout);
    ScanCommand command;
    command.format = format;
    for (const QBluetoothDeviceInfo &info: infos) {
        command.deviceUpdated(info, QBluetoothDeviceInfo::Fields());
    }
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
#endif
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

    #define DOKIT_ADD_TEST_ROW(config) QTest::newRow(#config) \
        << QBluetoothDeviceInfo::CoreConfiguration(QBluetoothDeviceInfo::config) \
        << QJsonArray{ QLatin1String(#config) }
    DOKIT_ADD_TEST_ROW(UnknownCoreConfiguration);
    DOKIT_ADD_TEST_ROW(LowEnergyCoreConfiguration);
    DOKIT_ADD_TEST_ROW(BaseRateCoreConfiguration);
    #undef DOKIT_ADD_TEST_ROW

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

    #define DOKIT_ADD_TEST_ROW(serviceClass) QTest::newRow(#serviceClass) \
        << QBluetoothDeviceInfo::ServiceClasses(QBluetoothDeviceInfo::serviceClass) \
        << QJsonArray{ QLatin1String(#serviceClass) }
    DOKIT_ADD_TEST_ROW(PositioningService);
    DOKIT_ADD_TEST_ROW(NetworkingService);
    DOKIT_ADD_TEST_ROW(RenderingService);
    DOKIT_ADD_TEST_ROW(CapturingService);
    DOKIT_ADD_TEST_ROW(ObjectTransferService);
    DOKIT_ADD_TEST_ROW(AudioService);
    DOKIT_ADD_TEST_ROW(TelephonyService);
    DOKIT_ADD_TEST_ROW(InformationService);
    #undef DOKIT_ADD_TEST_ROW

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
    #define DOKIT_ADD_TEST_ROW(majorClass) QTest::addRow(#majorClass) \
        << (int)QBluetoothDeviceInfo::majorClass << QString::fromLatin1(#majorClass)
    DOKIT_ADD_TEST_ROW(MiscellaneousDevice);
    DOKIT_ADD_TEST_ROW(ComputerDevice);
    DOKIT_ADD_TEST_ROW(PhoneDevice);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    DOKIT_ADD_TEST_ROW(LANAccessDevice); // Deprecated since Qt 5.13.
    #else
    DOKIT_ADD_TEST_ROW(NetworkDevice); // Added in Qt 5.13.
    #endif
    DOKIT_ADD_TEST_ROW(AudioVideoDevice);
    DOKIT_ADD_TEST_ROW(PeripheralDevice);
    DOKIT_ADD_TEST_ROW(ImagingDevice);
    DOKIT_ADD_TEST_ROW(WearableDevice);
    DOKIT_ADD_TEST_ROW(ToyDevice);
    DOKIT_ADD_TEST_ROW(HealthDevice);
    DOKIT_ADD_TEST_ROW(UncategorizedDevice);
    #undef DOKIT_ADD_TEST_ROW

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
    #define DOKIT_ADD_TEST_ROW(majorClass, minorClass) QTest::addRow(#majorClass ":" #minorClass) \
        << (int)QBluetoothDeviceInfo::majorClass << (quint8)QBluetoothDeviceInfo::minorClass \
        << QString::fromLatin1(#minorClass)

    DOKIT_ADD_TEST_ROW(MiscellaneousDevice, UncategorizedMiscellaneous);

    DOKIT_ADD_TEST_ROW(ComputerDevice, UncategorizedComputer);
    DOKIT_ADD_TEST_ROW(ComputerDevice, DesktopComputer);
    DOKIT_ADD_TEST_ROW(ComputerDevice, ServerComputer);
    DOKIT_ADD_TEST_ROW(ComputerDevice, LaptopComputer);
    DOKIT_ADD_TEST_ROW(ComputerDevice, HandheldClamShellComputer);
    DOKIT_ADD_TEST_ROW(ComputerDevice, HandheldComputer);
    DOKIT_ADD_TEST_ROW(ComputerDevice, WearableComputer);

    DOKIT_ADD_TEST_ROW(PhoneDevice, UncategorizedPhone);
    DOKIT_ADD_TEST_ROW(PhoneDevice, CellularPhone);
    DOKIT_ADD_TEST_ROW(PhoneDevice, CordlessPhone);
    DOKIT_ADD_TEST_ROW(PhoneDevice, SmartPhone);
    DOKIT_ADD_TEST_ROW(PhoneDevice, WiredModemOrVoiceGatewayPhone);
    DOKIT_ADD_TEST_ROW(PhoneDevice, CommonIsdnAccessPhone);

    DOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkFullService);
    DOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorOne);
    DOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorTwo);
    DOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorThree);
    DOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorFour);
    DOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorFive);
    DOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkLoadFactorSix);
    DOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE, NetworkNoService);

    DOKIT_ADD_TEST_ROW(AudioVideoDevice, UncategorizedAudioVideoDevice);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, WearableHeadsetDevice);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, HandsFreeDevice);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, Microphone);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, Loudspeaker);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, Headphones);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, PortableAudioDevice);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, CarAudio);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, SetTopBox);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, HiFiAudioDevice);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, Vcr);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, VideoCamera);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, Camcorder);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, VideoMonitor);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, VideoDisplayAndLoudspeaker);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, VideoConferencing);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice, GamingDevice);

    DOKIT_ADD_TEST_ROW(PeripheralDevice, UncategorizedPeripheral);
    DOKIT_ADD_TEST_ROW(PeripheralDevice, KeyboardPeripheral);
    DOKIT_ADD_TEST_ROW(PeripheralDevice, PointingDevicePeripheral);
    DOKIT_ADD_TEST_ROW(PeripheralDevice, KeyboardWithPointingDevicePeripheral);
    DOKIT_ADD_TEST_ROW(PeripheralDevice, JoystickPeripheral);
    DOKIT_ADD_TEST_ROW(PeripheralDevice, GamepadPeripheral);
    DOKIT_ADD_TEST_ROW(PeripheralDevice, RemoteControlPeripheral);
    DOKIT_ADD_TEST_ROW(PeripheralDevice, SensingDevicePeripheral);
    DOKIT_ADD_TEST_ROW(PeripheralDevice, DigitizerTabletPeripheral);
    DOKIT_ADD_TEST_ROW(PeripheralDevice, CardReaderPeripheral);

    DOKIT_ADD_TEST_ROW(ImagingDevice, UncategorizedImagingDevice);
    DOKIT_ADD_TEST_ROW(ImagingDevice, ImageDisplay);
    DOKIT_ADD_TEST_ROW(ImagingDevice, ImageCamera);
    DOKIT_ADD_TEST_ROW(ImagingDevice, ImageScanner);
    DOKIT_ADD_TEST_ROW(ImagingDevice, ImagePrinter);

    DOKIT_ADD_TEST_ROW(WearableDevice, UncategorizedWearableDevice);
    DOKIT_ADD_TEST_ROW(WearableDevice, WearableWristWatch);
    DOKIT_ADD_TEST_ROW(WearableDevice, WearablePager);
    DOKIT_ADD_TEST_ROW(WearableDevice, WearableJacket);
    DOKIT_ADD_TEST_ROW(WearableDevice, WearableHelmet);
    DOKIT_ADD_TEST_ROW(WearableDevice, WearableGlasses);

    DOKIT_ADD_TEST_ROW(ToyDevice, UncategorizedToy);
    DOKIT_ADD_TEST_ROW(ToyDevice, ToyRobot);
    DOKIT_ADD_TEST_ROW(ToyDevice, ToyVehicle);
    DOKIT_ADD_TEST_ROW(ToyDevice, ToyDoll);
    DOKIT_ADD_TEST_ROW(ToyDevice, ToyController);
    DOKIT_ADD_TEST_ROW(ToyDevice, ToyGame);

    DOKIT_ADD_TEST_ROW(HealthDevice, UncategorizedHealthDevice);
    DOKIT_ADD_TEST_ROW(HealthDevice, HealthBloodPressureMonitor);
    DOKIT_ADD_TEST_ROW(HealthDevice, HealthThermometer);
    DOKIT_ADD_TEST_ROW(HealthDevice, HealthWeightScale);
    DOKIT_ADD_TEST_ROW(HealthDevice, HealthGlucoseMeter);
    DOKIT_ADD_TEST_ROW(HealthDevice, HealthPulseOximeter);
    DOKIT_ADD_TEST_ROW(HealthDevice, HealthDataDisplay);
    DOKIT_ADD_TEST_ROW(HealthDevice, HealthStepCounter);
    #undef DOKIT_ADD_TEST_ROW

    // Test that all major classs fall through on out-of-range minor classes.
    #define DOKIT_ADD_TEST_ROW(majorClass) QTest::addRow(#majorClass ".invalid") \
        << (int)QBluetoothDeviceInfo::majorClass << (quint8)999 << QString()
    DOKIT_ADD_TEST_ROW(MiscellaneousDevice);
    DOKIT_ADD_TEST_ROW(ComputerDevice);
    DOKIT_ADD_TEST_ROW(PhoneDevice);
    DOKIT_ADD_TEST_ROW(MAJOR_CLASS_NETWORK_DEVICE);
    DOKIT_ADD_TEST_ROW(AudioVideoDevice);
    DOKIT_ADD_TEST_ROW(PeripheralDevice);
    DOKIT_ADD_TEST_ROW(ImagingDevice);
    DOKIT_ADD_TEST_ROW(WearableDevice);
    DOKIT_ADD_TEST_ROW(ToyDevice);
    DOKIT_ADD_TEST_ROW(HealthDevice);
    DOKIT_ADD_TEST_ROW(UncategorizedDevice);
    #undef DOKIT_ADD_TEST_ROW
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
    ScanCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestScanCommand)
