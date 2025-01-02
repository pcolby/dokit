// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testinfocommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"
#include "../github.h"

#include "infocommand.h"

#include <qtpokit/pokitdevice.h>

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)

class MockDeviceCommand : public DeviceCommand
{
public:
    explicit MockDeviceCommand(QObject * const parent = nullptr) : DeviceCommand(parent)
    {

    }

    AbstractPokitService * getService() override
    {
        return nullptr;
    }
};

void TestInfoCommand::requiredOptions()
{
    InfoCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser);
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestInfoCommand::supportedOptions()
{
    InfoCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser);
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestInfoCommand::processOptions()
{
    InfoCommand command(this);
    QCommandLineParser parser;
    QCOMPARE(command.processOptions(parser), QStringList());
}

void TestInfoCommand::getService()
{
    // Unable to safely invoke InfoCommand::getService() without a valid Bluetooth device.
}

void TestInfoCommand::serviceDetailsDiscovered_data()
{
    QTest::addColumn<QBluetoothDeviceInfo>("info");
    QTest::addColumn<AbstractCommand::OutputFormat>("format");

    QBluetoothDeviceInfo rssi(QBluetoothUuid(
        QStringLiteral("661d1ed3-8e28-4f2b-88da-a1c02fe98aed")), QStringLiteral("rssi"), 0);
    rssi.setRssi(12345);

    QTest::addRow("null.csv")  << QBluetoothDeviceInfo() << AbstractCommand::OutputFormat::Csv;
    QTest::addRow("null.json") << QBluetoothDeviceInfo() << AbstractCommand::OutputFormat::Json;
    QTest::addRow("null.txt")  << QBluetoothDeviceInfo() << AbstractCommand::OutputFormat::Text;

    #define DOKIT_ADD_TEST_ROW(info) \
        QTest::newRow(qUtf8Printable(info.name() + QStringLiteral(".csv"))) \
            << info << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(qUtf8Printable(info.name() + QStringLiteral(".json"))) \
            << info << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(qUtf8Printable(info.name() + QStringLiteral(".txt"))) \
            << info << AbstractCommand::OutputFormat::Text
    DOKIT_ADD_TEST_ROW(QBluetoothDeviceInfo(QBluetoothAddress(0), QStringLiteral("addr0"), 0));
    DOKIT_ADD_TEST_ROW(QBluetoothDeviceInfo(QBluetoothAddress(0xFFFFFFFFFFFFFFFF), QStringLiteral("addrMax"), 0xFFFFFFFF));
    DOKIT_ADD_TEST_ROW(QBluetoothDeviceInfo(QBluetoothAddress(QStringLiteral("0123456789ABC")), QStringLiteral("addrStr"), 0));
    DOKIT_ADD_TEST_ROW(QBluetoothDeviceInfo(QBluetoothUuid(QStringLiteral("5c0625b1-a46b-44f1-a6aa-058424ce69b0")), QStringLiteral("uuid1"), 123));
    DOKIT_ADD_TEST_ROW(QBluetoothDeviceInfo(QBluetoothUuid(QStringLiteral("e8ee1747-1e43-4699-bac6-88cab02c109c")), QStringLiteral("uuid2"), 456));
    DOKIT_ADD_TEST_ROW(QBluetoothDeviceInfo(QBluetoothUuid(QStringLiteral("381b1e3c-c25a-44a3-9230-2a737cf3f206")), QStringLiteral("uuid3"), 678 ));
    DOKIT_ADD_TEST_ROW(rssi);
    #undef DOKIT_ADD_TEST_ROW
}

void TestInfoCommand::serviceDetailsDiscovered()
{
    if (gitHubActionsRunnerOsVersion() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 14)) {
        QSKIP("BLE controller operations hang on GitHub Actions's macOS 14 runners");
    }

    QFETCH(QBluetoothDeviceInfo, info);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    #if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    if (!info.deviceUuid().isNull()) {
        // Fixed (though not called out) by https://bugreports.qt.io/browse/QTBUG-75348
        QSKIP("QLowEnergyController fails to track device UUIDs prior to Qt 5.14.");
    }
    #if defined(Q_OS_MACOS)
    if (!info.address().isNull()) {
        QSKIP("On macOS, QLowEnergyController fails to track device addresses prior to Qt 5.14.");
    }
    #endif // macOS
    #endif // < Qt 5.14

    const OutputStreamCapture capture(&std::cout);
    InfoCommand command(this);
    command.device = new PokitDevice(info, &command);
    command.service = command.device->deviceInformation();
    command.format = format;
    command.serviceDetailsDiscovered();
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestInfoCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    InfoCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestInfoCommand)
