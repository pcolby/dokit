// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdevicecommand.h"
#include "../github.h"

#include "devicecommand.h"

#include <qtpokit/statusservice.h>
#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>

Q_DECLARE_METATYPE(PokitMeter::CurrentRange)
Q_DECLARE_METATYPE(PokitMeter::ResistanceRange)
Q_DECLARE_METATYPE(PokitMeter::VoltageRange)
Q_DECLARE_METATYPE(PokitPro::CapacitanceRange)
Q_DECLARE_METATYPE(PokitPro::CurrentRange)
Q_DECLARE_METATYPE(PokitPro::ResistanceRange)
Q_DECLARE_METATYPE(PokitPro::VoltageRange)

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

// Mock device command class that does nothing (but log) if real devices are actually discovered. That is, if testing
// while real (physical) Pokit devices are within BLE range. This never happes in CI environments of course.
class StartableDeviceCommand : public MockDeviceCommand
{
protected slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &info) override
    {
        qWarning() << "Ignoring discovered Pokit device:" << info.name() << info.deviceUuid() << info.address();
    }
};

void TestDeviceCommand::start()
{
    if (gitHubActionsRunnerOsVersion() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 14)) {
        QSKIP("BLE controller operations hang on GitHub Actions's macOS 14 runners");
    }

    StartableDeviceCommand command;

    QVERIFY(command.deviceToScanFor.isNull());
    QTest::ignoreMessage(QtInfoMsg, "Looking for first available Pokit device...");
    QVERIFY(command.start());

    command.deviceToScanFor = QStringLiteral("example");
    QTest::ignoreMessage(QtInfoMsg, R"(Looking for device "example"...)");
    QVERIFY(command.start());
}

void TestDeviceCommand::disconnect()
{
    // Cannot test DeviceCommand::disconnect() without a valid device controller.
}

void TestDeviceCommand::minRange_meter_current_data()
{
    QTest::addColumn<quint32>("maxValue");
    QTest::addColumn<PokitMeter::CurrentRange>("expected");

    QTest::addRow("0")       << (quint32)      0 << PokitMeter::CurrentRange::AutoRange;
    QTest::addRow("1μA")     << (quint32)      1 << PokitMeter::CurrentRange::_10mA;
    QTest::addRow("10mA")    << (quint32)  10000 << PokitMeter::CurrentRange::_10mA;
    QTest::addRow("10mA++")  << (quint32)  10001 << PokitMeter::CurrentRange::_30mA;
    QTest::addRow("30mA")    << (quint32)  30000 << PokitMeter::CurrentRange::_30mA;
    QTest::addRow("30mA++")  << (quint32)  30001 << PokitMeter::CurrentRange::_150mA;
    QTest::addRow("150mA")   << (quint32) 150000 << PokitMeter::CurrentRange::_150mA;
    QTest::addRow("150mA++") << (quint32) 150001 << PokitMeter::CurrentRange::_300mA;
    QTest::addRow("300mA")   << (quint32) 300000 << PokitMeter::CurrentRange::_300mA;
    QTest::addRow("300mA++") << (quint32) 300001 << PokitMeter::CurrentRange::_2A;
    QTest::addRow("2A")      << (quint32)2000000 << PokitMeter::CurrentRange::_2A;
    QTest::addRow("2A++")    << (quint32)2000001 << PokitMeter::CurrentRange::AutoRange;
}

void TestDeviceCommand::minRange_meter_current()
{
    QFETCH(quint32, maxValue);
    QFETCH(PokitMeter::CurrentRange, expected);
    const auto actual = DeviceCommand::minRange<PokitMeter::CurrentRange>(maxValue);
    QCOMPARE(PokitMeter::toString(actual), PokitMeter::toString(expected));
    QVERIFY(actual == expected);
}

void TestDeviceCommand::minRange_meter_resistance_data()
{
    QTest::addColumn<quint32>("maxValue");
    QTest::addColumn<PokitMeter::ResistanceRange>("expected");

    QTest::addRow("0")       << (quint32)      0 << PokitMeter::ResistanceRange::AutoRange;
    QTest::addRow("1Ω")      << (quint32)      1 << PokitMeter::ResistanceRange::_160;
    QTest::addRow("160Ω")    << (quint32)    160 << PokitMeter::ResistanceRange::_160;
    QTest::addRow("160Ω++")  << (quint32)    161 << PokitMeter::ResistanceRange::_330;
    QTest::addRow("330Ω")    << (quint32)    330 << PokitMeter::ResistanceRange::_330;
    QTest::addRow("330Ω++")  << (quint32)    331 << PokitMeter::ResistanceRange::_890;
    QTest::addRow("890Ω")    << (quint32)    890 << PokitMeter::ResistanceRange::_890;
    QTest::addRow("890Ω++")  << (quint32)    891 << PokitMeter::ResistanceRange::_1K5;
    QTest::addRow("1K5Ω")    << (quint32)   1500 << PokitMeter::ResistanceRange::_1K5;
    QTest::addRow("1K5++")   << (quint32)   1501 << PokitMeter::ResistanceRange::_10K;
    QTest::addRow("10KΩ")    << (quint32)  10000 << PokitMeter::ResistanceRange::_10K;
    QTest::addRow("10KΩ++")  << (quint32)  10001 << PokitMeter::ResistanceRange::_100K;
    QTest::addRow("100KΩ")   << (quint32) 100000 << PokitMeter::ResistanceRange::_100K;
    QTest::addRow("100KΩ++") << (quint32) 100001 << PokitMeter::ResistanceRange::_470K;
    QTest::addRow("470KΩ")   << (quint32) 470000 << PokitMeter::ResistanceRange::_470K;
    QTest::addRow("470KΩ++") << (quint32) 470001 << PokitMeter::ResistanceRange::_1M;
    QTest::addRow("1MΩ")     << (quint32)1000000 << PokitMeter::ResistanceRange::_1M;
    QTest::addRow("1MΩ++")   << (quint32)1000001 << PokitMeter::ResistanceRange::AutoRange;
}

void TestDeviceCommand::minRange_meter_resistance()
{
    QFETCH(quint32, maxValue);
    QFETCH(PokitMeter::ResistanceRange, expected);
    const auto actual = DeviceCommand::minRange<PokitMeter::ResistanceRange>(maxValue);
    QCOMPARE(PokitMeter::toString(actual), PokitMeter::toString(expected));
    QVERIFY(actual == expected);
}

void TestDeviceCommand::minRange_meter_voltage_data()
{
    QTest::addColumn<quint32>("maxValue");
    QTest::addColumn<PokitMeter::VoltageRange>("expected");

    QTest::addRow("0")       << (quint32)    0 << PokitMeter::VoltageRange::AutoRange;
    QTest::addRow("1mV")     << (quint32)    1 << PokitMeter::VoltageRange::_300mV;
    QTest::addRow("300mV")   << (quint32)  300 << PokitMeter::VoltageRange::_300mV;
    QTest::addRow("300mV++") << (quint32)  301 << PokitMeter::VoltageRange::_2V;
    QTest::addRow("2V")      << (quint32) 2000 << PokitMeter::VoltageRange::_2V;
    QTest::addRow("2V++")    << (quint32) 2001 << PokitMeter::VoltageRange::_6V;
    QTest::addRow("6V")      << (quint32) 6000 << PokitMeter::VoltageRange::_6V;
    QTest::addRow("6V++")    << (quint32) 6001 << PokitMeter::VoltageRange::_12V;
    QTest::addRow("12V")     << (quint32)12000 << PokitMeter::VoltageRange::_12V;
    QTest::addRow("12V++")   << (quint32)12001 << PokitMeter::VoltageRange::_30V;
    QTest::addRow("30V")     << (quint32)30000 << PokitMeter::VoltageRange::_30V;
    QTest::addRow("30V++")   << (quint32)30001 << PokitMeter::VoltageRange::_60V;
    QTest::addRow("60V")     << (quint32)60000 << PokitMeter::VoltageRange::_60V;
    QTest::addRow("60V++")   << (quint32)60001 << PokitMeter::VoltageRange::AutoRange;
}

void TestDeviceCommand::minRange_meter_voltage()
{
    QFETCH(quint32, maxValue);
    QFETCH(PokitMeter::VoltageRange, expected);
    const auto actual = DeviceCommand::minRange<PokitMeter::VoltageRange>(maxValue);
    QCOMPARE(PokitMeter::toString(actual), PokitMeter::toString(expected));
    QVERIFY(actual == expected);
}

void TestDeviceCommand::minRange_pro_capacitance_data()
{
    QTest::addColumn<quint32>("maxValue");
    QTest::addColumn<PokitPro::CapacitanceRange>("expected");

    QTest::addRow("0")       << (quint32)      0 << PokitPro::CapacitanceRange::AutoRange;
    QTest::addRow("1nF")     << (quint32)      1 << PokitPro::CapacitanceRange::_100nF;
    QTest::addRow("100nF")   << (quint32)    100 << PokitPro::CapacitanceRange::_100nF;
    QTest::addRow("100nF++") << (quint32)    101 << PokitPro::CapacitanceRange::_10uF;
    QTest::addRow("10μF")    << (quint32)  10000 << PokitPro::CapacitanceRange::_10uF;
    QTest::addRow("10μF++")  << (quint32)  10001 << PokitPro::CapacitanceRange::_1mF;
    QTest::addRow("1mF")     << (quint32)1000000 << PokitPro::CapacitanceRange::_1mF;
    QTest::addRow("1mF++")   << (quint32)1000001 << PokitPro::CapacitanceRange::AutoRange;
}

void TestDeviceCommand::minRange_pro_capacitance()
{
    QFETCH(quint32, maxValue);
    QFETCH(PokitPro::CapacitanceRange, expected);
    const auto actual = DeviceCommand::minRange<PokitPro::CapacitanceRange>(maxValue);
    QCOMPARE(PokitPro::toString(actual), PokitPro::toString(expected));
    QVERIFY(actual == expected);
}

void TestDeviceCommand::minRange_pro_current_data()
{
    QTest::addColumn<quint32>("maxValue");
    QTest::addColumn<PokitPro::CurrentRange>("expected");

    QTest::addRow("0")       << (quint32)       0 << PokitPro::CurrentRange::AutoRange;
    QTest::addRow("1μA")     << (quint32)       1 << PokitPro::CurrentRange::_500uA;
    QTest::addRow("500μA")   << (quint32)     500 << PokitPro::CurrentRange::_500uA;
    QTest::addRow("500μA++") << (quint32)     501 << PokitPro::CurrentRange::_2mA;
    QTest::addRow("2mA")     << (quint32)    2000 << PokitPro::CurrentRange::_2mA;
    QTest::addRow("2mA++")   << (quint32)    2001 << PokitPro::CurrentRange::_10mA;
    QTest::addRow("10mA")    << (quint32)   10000 << PokitPro::CurrentRange::_10mA;
    QTest::addRow("10mA++")  << (quint32)   10001 << PokitPro::CurrentRange::_125mA;
    QTest::addRow("125mA")    << (quint32) 125000 << PokitPro::CurrentRange::_125mA;
    QTest::addRow("125mA++") << (quint32)  125001 << PokitPro::CurrentRange::_300mA;
    QTest::addRow("300mA")   << (quint32)  300000 << PokitPro::CurrentRange::_300mA;
    QTest::addRow("300mA++") << (quint32)  300001 << PokitPro::CurrentRange::_3A;
    QTest::addRow("3A")      << (quint32) 3000000 << PokitPro::CurrentRange::_3A;
    QTest::addRow("3A++")    << (quint32) 3000001 << PokitPro::CurrentRange::_10A;
    QTest::addRow("10A")     << (quint32)10000000 << PokitPro::CurrentRange::_10A;
    QTest::addRow("10A++")   << (quint32)10000001 << PokitPro::CurrentRange::AutoRange;
}

void TestDeviceCommand::minRange_pro_current()
{
    QFETCH(quint32, maxValue);
    QFETCH(PokitPro::CurrentRange, expected);
    const auto actual = DeviceCommand::minRange<PokitPro::CurrentRange>(maxValue);
    QCOMPARE(PokitPro::toString(actual), PokitPro::toString(expected));
    QVERIFY(actual == expected);
}

void TestDeviceCommand::minRange_pro_resistance_data()
{
    QTest::addColumn<quint32>("maxValue");
    QTest::addColumn<PokitPro::ResistanceRange>("expected");

    QTest::addRow("0")       << (quint32)      0 << PokitPro::ResistanceRange::AutoRange;
    QTest::addRow("1Ω")      << (quint32)      1 << PokitPro::ResistanceRange::_30;
    QTest::addRow("30Ω")     << (quint32)     30 << PokitPro::ResistanceRange::_30;
    QTest::addRow("30Ω++")   << (quint32)     31 << PokitPro::ResistanceRange::_75;
    QTest::addRow("75Ω")     << (quint32)     75 << PokitPro::ResistanceRange::_75;
    QTest::addRow("75Ω++")   << (quint32)     76 << PokitPro::ResistanceRange::_400;
    QTest::addRow("400Ω")    << (quint32)    400 << PokitPro::ResistanceRange::_400;
    QTest::addRow("400Ω++")  << (quint32)    401 << PokitPro::ResistanceRange::_5K;
    QTest::addRow("5KΩ")     << (quint32)   5000 << PokitPro::ResistanceRange::_5K;
    QTest::addRow("5KΩ++")   << (quint32)   5001 << PokitPro::ResistanceRange::_10K;
    QTest::addRow("10KΩ")    << (quint32)  10000 << PokitPro::ResistanceRange::_10K;
    QTest::addRow("10KΩ++")  << (quint32)  10001 << PokitPro::ResistanceRange::_15K;
    QTest::addRow("15KΩ")    << (quint32)  15000 << PokitPro::ResistanceRange::_15K;
    QTest::addRow("15KΩ++")  << (quint32)  15001 << PokitPro::ResistanceRange::_40K;
    QTest::addRow("40KΩ")    << (quint32)  40000 << PokitPro::ResistanceRange::_40K;
    QTest::addRow("40KΩ++")  << (quint32)  40001 << PokitPro::ResistanceRange::_500K;
    QTest::addRow("500KΩ")   << (quint32) 500000 << PokitPro::ResistanceRange::_500K;
    QTest::addRow("500KΩ++") << (quint32) 500001 << PokitPro::ResistanceRange::_700K;
    QTest::addRow("700KΩ")   << (quint32) 700000 << PokitPro::ResistanceRange::_700K;
    QTest::addRow("700KΩ++") << (quint32) 700001 << PokitPro::ResistanceRange::_1M;
    QTest::addRow("1MΩ")     << (quint32)1000000 << PokitPro::ResistanceRange::_1M;
    QTest::addRow("1MΩ++")   << (quint32)1000001 << PokitPro::ResistanceRange::_3M;
    QTest::addRow("3MΩ")     << (quint32)3000000 << PokitPro::ResistanceRange::_3M;
    QTest::addRow("3MΩ++")   << (quint32)3000001 << PokitPro::ResistanceRange::AutoRange;
}

void TestDeviceCommand::minRange_pro_resistance()
{
    QFETCH(quint32, maxValue);
    QFETCH(PokitPro::ResistanceRange, expected);
    const auto actual = DeviceCommand::minRange<PokitPro::ResistanceRange>(maxValue);
    QCOMPARE(PokitPro::toString(actual), PokitPro::toString(expected));
    QVERIFY(actual == expected);
}

void TestDeviceCommand::minRange_pro_voltage_data()
{
    QTest::addColumn<quint32>("maxValue");
    QTest::addColumn<PokitPro::VoltageRange>("expected");

    QTest::addRow("0")       << (quint32)     0 << PokitPro::VoltageRange::AutoRange;
    QTest::addRow("1mV")     << (quint32)     1 << PokitPro::VoltageRange::_250mV;
    QTest::addRow("250mV")   << (quint32)   250 << PokitPro::VoltageRange::_250mV;
    QTest::addRow("250mV++") << (quint32)   251 << PokitPro::VoltageRange::_2V;
    QTest::addRow("2V")      << (quint32)  2000 << PokitPro::VoltageRange::_2V;
    QTest::addRow("2V++")    << (quint32)  2001 << PokitPro::VoltageRange::_10V;
    QTest::addRow("10V")     << (quint32) 10000 << PokitPro::VoltageRange::_10V;
    QTest::addRow("10V++")   << (quint32) 10001 << PokitPro::VoltageRange::_30V;
    QTest::addRow("30V")     << (quint32) 30000 << PokitPro::VoltageRange::_30V;
    QTest::addRow("30V++")   << (quint32) 30001 << PokitPro::VoltageRange::_60V;
    QTest::addRow("60V")     << (quint32) 60000 << PokitPro::VoltageRange::_60V;
    QTest::addRow("60V++")   << (quint32) 60001 << PokitPro::VoltageRange::_125V;
    QTest::addRow("125V")    << (quint32)125000 << PokitPro::VoltageRange::_125V;
    QTest::addRow("125V++")  << (quint32)125001 << PokitPro::VoltageRange::_400V;
    QTest::addRow("400V")    << (quint32)400000 << PokitPro::VoltageRange::_400V;
    QTest::addRow("400V++")  << (quint32)400001 << PokitPro::VoltageRange::_600V;
    QTest::addRow("600V")    << (quint32)600000 << PokitPro::VoltageRange::_600V;
    QTest::addRow("600V++")  << (quint32)600001 << PokitPro::VoltageRange::AutoRange;
}

void TestDeviceCommand::minRange_pro_voltage()
{
    QFETCH(quint32, maxValue);
    QFETCH(PokitPro::VoltageRange, expected);
    const auto actual = DeviceCommand::minRange<PokitPro::VoltageRange>(maxValue);
    QCOMPARE(PokitPro::toString(actual), PokitPro::toString(expected));
    QVERIFY(actual == expected);
}

void TestDeviceCommand::minCapacitanceRange()
{
    const quint8 actual = DeviceCommand::minCapacitanceRange(PokitProduct::PokitPro, 123456);
    QCOMPARE(actual, (quint8)PokitPro::CapacitanceRange::_1mF);
}

void TestDeviceCommand::minCurrentRange()
{
    quint8 actual = DeviceCommand::minCurrentRange(PokitProduct::PokitMeter, 123456);
    QCOMPARE(actual, (quint8)PokitMeter::CurrentRange::_150mA);
    actual = DeviceCommand::minCurrentRange(PokitProduct::PokitPro, 123456);
    QCOMPARE(actual, (quint8)PokitPro::CurrentRange::_125mA);
}

void TestDeviceCommand::minResistanceRange()
{
    quint8 actual = DeviceCommand::minResistanceRange(PokitProduct::PokitMeter, 123456);
    QCOMPARE(actual, (quint8)PokitMeter::ResistanceRange::_470K);
    actual = DeviceCommand::minResistanceRange(PokitProduct::PokitPro, 123456);
    QCOMPARE(actual, (quint8)PokitPro::ResistanceRange::_500K);
}

void TestDeviceCommand::minVoltageRange()
{
    quint8 actual = DeviceCommand::minVoltageRange(PokitProduct::PokitMeter, 12345);
    QCOMPARE(actual, (quint8)PokitMeter::VoltageRange::_30V);
    actual = DeviceCommand::minVoltageRange(PokitProduct::PokitPro, 123456);
    QCOMPARE(actual, (quint8)PokitPro::VoltageRange::_125V);
}

void TestDeviceCommand::controllerError()
{
    MockDeviceCommand command;

    QTest::ignoreMessage(QtWarningMsg,
    #if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
        "Bluetooth controller error: QLowEnergyController::Error(NetworkError)");
    #else
        "Bluetooth controller error: QLowEnergyController::NetworkError");
    #endif
    command.controllerError(QLowEnergyController::Error::NetworkError);

    QTest::ignoreMessage(QtWarningMsg,
    #if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
         "Bluetooth controller error: QLowEnergyController::Error(UnknownError)");
    #else
         "Bluetooth controller error: QLowEnergyController::UnknownError");
    #endif
    command.controllerError(QLowEnergyController::Error::UnknownError);
}

void TestDeviceCommand::deviceDisconnected()
{
    MockDeviceCommand command;
    command.deviceDisconnected(); // Just logs a debug message, and exits.
}

void TestDeviceCommand::serviceError()
{
    MockDeviceCommand command;

    QTest::ignoreMessage(QtWarningMsg,
    #if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
         "Bluetooth service error: QLowEnergyService::ServiceError(OperationError)");
    #else
         "Bluetooth service error: QLowEnergyService::OperationError");
    #endif
    command.serviceError(QLowEnergyService::ServiceError::OperationError);

    QTest::ignoreMessage(QtWarningMsg,
    #if (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))
         "Bluetooth service error: QLowEnergyService::ServiceError(UnknownError)");
    #else
         "Bluetooth service error: QLowEnergyService::UnknownError");
    #endif
    command.serviceError(QLowEnergyService::ServiceError::UnknownError);
}

void TestDeviceCommand::serviceDetailsDiscovered()
{
    MockDeviceCommand command;
    command.serviceDetailsDiscovered(); // Just logs a debug message.
}

void TestDeviceCommand::deviceDiscovered()
{
    MockDeviceCommand command;
    command.deviceToScanFor = QStringLiteral("example");
    QBluetoothDeviceInfo info;
    info.setServiceUuids({StatusService::ServiceUuids::pokitMeter}
        #if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
        , QBluetoothDeviceInfo::DataUnavailable // Required prior to 5.13, but deprecated in 5.13.
        #endif
    );
    command.deviceDiscovered(info); // Just logs a debug message, and ignores.
}

void TestDeviceCommand::deviceDiscoveryFinished()
{
    MockDeviceCommand command;

    QVERIFY(command.deviceToScanFor.isNull());
    QTest::ignoreMessage(QtWarningMsg, "Failed to find any Pokit device.");
    command.deviceDiscoveryFinished();

    command.deviceToScanFor = QStringLiteral("example");
    QTest::ignoreMessage(QtWarningMsg, R"(Failed to find device "example".)");
    command.deviceDiscoveryFinished();
}

void TestDeviceCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    MockDeviceCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestDeviceCommand)
