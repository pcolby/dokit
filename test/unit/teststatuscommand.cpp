// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "teststatuscommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

#include "statuscommand.h"

#include <qtpokit/pokitdevice.h>

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(StatusService::DeviceCharacteristics);

class MockDeviceCommand : public DeviceCommand
{
public:
    MockDeviceCommand() : DeviceCommand(nullptr)
    {

    }

    AbstractPokitService * getService() override
    {
        return nullptr;
    }
};

void TestStatusCommand::requiredOptions()
{
    StatusCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser);
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestStatusCommand::supportedOptions()
{
    StatusCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser);
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestStatusCommand::processOptions()
{
    StatusCommand command(this);
    QCommandLineParser parser;
    QCOMPARE(command.processOptions(parser), QStringList());
}

void TestStatusCommand::getService()
{
    // Unable to safely invoke StatusCommand::getService() without a valid Bluetooth device.
}

void TestStatusCommand::serviceDetailsDiscovered()
{
    StatusCommand command(this);
    command.device = new PokitDevice(QBluetoothDeviceInfo(), &command);
    command.service = command.device->status();
    command.serviceDetailsDiscovered();
}

void TestStatusCommand::outputDeviceStatus_data()
{
    QTest::addColumn<StatusService::DeviceCharacteristics>("chrs");
    QTest::addColumn<AbstractCommand::OutputFormat>("format");

    #define QTPOKIT_ADD_TEST_ROW(name, chrs) \
        QTest::newRow(name ".csv") << StatusService::DeviceCharacteristics chrs \
            << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(name ".json") << StatusService::DeviceCharacteristics chrs \
            << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(name ".txt") << StatusService::DeviceCharacteristics chrs \
            << AbstractCommand::OutputFormat::Text


    QTPOKIT_ADD_TEST_ROW("null", ({
        QVersionNumber(), 0, 0, 0, 0, 0, 0, QBluetoothAddress()
    }));

    // Sample from a real Pokit Meter device.
    QTPOKIT_ADD_TEST_ROW("PokitMeter", ({
        QVersionNumber(1,4), 60, 2, 1000, 1000, 8192, 0,
        QBluetoothAddress(QStringLiteral("84:2E:14:2C:03:A8"))
    }));

    // Sample from a real Pokit Pro device.
    QTPOKIT_ADD_TEST_ROW("PokitPro", ({
        QVersionNumber(1,3), 850, 10, 3000, 1000, 16384, 0,
        QBluetoothAddress(QStringLiteral("5C:02:72:09:AA:25"))
    }));

    #undef QTPOKIT_ADD_TEST_ROW
}

void TestStatusCommand::outputDeviceStatus()
{
    QFETCH(StatusService::DeviceCharacteristics, chrs);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    const OutputStreamCapture capture(&std::cout);
    StatusCommand command(this);
    command.device = new PokitDevice(QBluetoothDeviceInfo(), &command);
    command.service = command.device->status();
    command.format = format;
    command.outputDeviceStatus(chrs);
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestStatusCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    StatusCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestStatusCommand)
