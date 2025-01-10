// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testmetercommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"
#include "../github.h"

#include "metercommand.h"

#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(MultimeterService::Mode)
Q_DECLARE_METATYPE(MultimeterService::Reading)
Q_DECLARE_METATYPE(MultimeterService::Settings)

typedef quint8 (* minRangeFunc)(const PokitProduct product, const quint32 maxValue);
Q_DECLARE_METATYPE(minRangeFunc)

class MockDeviceCommand : public DeviceCommand
{
public:
    MockDeviceCommand(QObject * const parent = nullptr) : DeviceCommand(parent)
    {

    }

    AbstractPokitService * getService() override
    {
        return nullptr;
    }
};

void TestMeterCommand::requiredOptions()
{
    MeterCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) +
        QStringList{ QStringLiteral("mode") };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestMeterCommand::supportedOptions()
{
    MeterCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser) +
        QStringList{ QStringLiteral("interval"), QStringLiteral("range"), QStringLiteral("samples") };
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestMeterCommand::processOptions_data()
{
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<MultimeterService::Settings>("expectedSettings");
    QTest::addColumn<minRangeFunc>("expectedMinRangeFunc");
    QTest::addColumn<quint32>("expectedRangeOptionValue");
    QTest::addColumn<int>("expectedSamples");
    QTest::addColumn<QStringList>("expectedErrors");

    QTest::addRow("missing-required-mode")
        << QStringList{
           QStringLiteral("--range"), QStringLiteral("100")}
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ QStringLiteral("Missing required option: mode") };

    QTest::addRow("Vdc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::_2V, 1000}
        << &MeterCommand::minVoltageRange << 1000u
        << -1
        << QStringList{ };

    QTest::addRow("Vac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vac"),
           QStringLiteral("--range"), QStringLiteral("5V") }
        << MultimeterService::Settings{
            MultimeterService::Mode::AcVoltage, +PokitMeter::VoltageRange::_6V, 1000}
        << &MeterCommand::minVoltageRange << 5000u
        << -1
        << QStringList{ };

    QTest::addRow("Adc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Adc"),
           QStringLiteral("--range"), QStringLiteral("100mA") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcCurrent, +PokitMeter::CurrentRange::_150mA, 1000}
        << &MeterCommand::minCurrentRange << 100u
        << -1
        << QStringList{ };

    QTest::addRow("Aac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Aac"),
           QStringLiteral("--range"), QStringLiteral("2A") }
        << MultimeterService::Settings{
            MultimeterService::Mode::AcCurrent, +PokitMeter::CurrentRange::_2A, 1000}
        << &MeterCommand::minCurrentRange << 2000u
        << -1
        << QStringList{ };

    QTest::addRow("Resistance")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("res") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Resistance, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minResistanceRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("Diode")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("dio") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Diode, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ };

    QTest::addRow("Continuity")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("cont") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Continuity, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ };

    QTest::addRow("Temperature")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("temp") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Temperature, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ };

    QTest::addRow("Capacitance")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("cap"),
           QStringLiteral("--range"), QStringLiteral("500uF") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Capacitance, +PokitPro::CapacitanceRange::_100nF, 1000}
        << &MeterCommand::minCapacitanceRange << 500000u
        << -1
        << QStringList{ };

    QTest::addRow("invalid-mode")
        << QStringList{ QStringLiteral("--mode"),  QStringLiteral("invalid") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ QStringLiteral("Unknown meter mode: invalid") };

    QTest::addRow("interval:100") // Defaults to 100 seconds (ie 100,000ms).
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("100") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 100000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("interval:499") // Defaults to 499 seconds.
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("499") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 499000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("interval:500") // Defaults to 500ms.
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("500") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 500}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("invalid-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("invalid") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ QStringLiteral("Invalid interval value: invalid") };

    QTest::addRow("negative-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--interval"), QStringLiteral("-123") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ QStringLiteral("Invalid interval value: -123") };

    QTest::addRow("auto-range-voltage")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("auto") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("auto-range-current")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Adc"),
           QStringLiteral("--range"), QStringLiteral("auto") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcCurrent, +PokitMeter::CurrentRange::AutoRange, 1000}
        << &MeterCommand::minCurrentRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("auto-range-resistance")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("resis"),
           QStringLiteral("--range"), QStringLiteral("auto") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Resistance, +PokitMeter::ResistanceRange::AutoRange, 1000}
        << &MeterCommand::minResistanceRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("auto-range-capacitance")
        << QStringList{
                       QStringLiteral("--mode"),  QStringLiteral("capac"),
                       QStringLiteral("--range"), QStringLiteral("auto") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Capacitance, +PokitPro::CapacitanceRange::AutoRange, 1000}
        << &MeterCommand::minCapacitanceRange << 0u
        << -1
        << QStringList{ };

    QTest::addRow("invalid-range")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("invalid") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ QStringLiteral("Invalid range value: invalid") };

    QTest::addRow("rangeless-range")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("diode"),
           QStringLiteral("--range"), QStringLiteral("1000") }
        << MultimeterService::Settings{
            MultimeterService::Mode::Diode, +PokitMeter::VoltageRange::AutoRange, 1000}
        << static_cast<minRangeFunc>(nullptr) << 0u
        << -1
        << QStringList{ };

    QTest::addRow("samples:100")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--samples"), QStringLiteral("100") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << 100
        << QStringList{ };

    QTest::addRow("invalid-samples")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--samples"), QStringLiteral("invalid") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ QStringLiteral("Invalid samples value: invalid") };

    QTest::addRow("negative-samples")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--samples"), QStringLiteral("-123") }
        << MultimeterService::Settings{
            MultimeterService::Mode::DcVoltage, +PokitMeter::VoltageRange::AutoRange, 1000}
        << &MeterCommand::minVoltageRange << 0u
        << -1
        << QStringList{ QStringLiteral("Invalid samples value: -123") };
}

void TestMeterCommand::processOptions()
{
    QFETCH(QStringList, arguments);
    QFETCH(MultimeterService::Settings, expectedSettings);
    QFETCH(minRangeFunc, expectedMinRangeFunc);
    QFETCH(quint32, expectedRangeOptionValue);
    QFETCH(int, expectedSamples);
    QFETCH(QStringList, expectedErrors);

    arguments.prepend(QStringLiteral("dokit")); // The first argument is always the app name.

    QCommandLineParser parser;
    parser.addOption({QStringLiteral("mode"), QStringLiteral("description"), QStringLiteral("mode")});
    parser.addOption({QStringLiteral("range"), QStringLiteral("description"), QStringLiteral("range")});
    parser.addOption({QStringLiteral("interval"), QStringLiteral("description"), QStringLiteral("interval")});
    parser.addOption({QStringLiteral("samples"), QStringLiteral("description"), QStringLiteral("samples")});
    parser.process(arguments);

    MeterCommand command(this);
    QCOMPARE(command.processOptions(parser),  expectedErrors);
    QCOMPARE(command.settings.mode,           expectedSettings.mode);
    QCOMPARE(command.settings.range,          (quint8)255); // Always 255, because range is not set until services discovered.
    QCOMPARE(command.settings.updateInterval, expectedSettings.updateInterval);
    QCOMPARE(command.samplesToGo,             expectedSamples);
    QVERIFY (command.minRangeFunc      ==     expectedMinRangeFunc);
    QCOMPARE(command.rangeOptionValue,        expectedRangeOptionValue);
}

void TestMeterCommand::getService()
{
    // Unable to safely invoke MeterCommand::getService() without a valid Bluetooth device.
}

void TestMeterCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke MeterCommand::serviceDetailsDiscovered() without a valid service.
}

void TestMeterCommand::settingsWritten()
{
    // Unable to safely invoke MeterCommand::settingsWritten() without a valid Bluetooth service.
}

void TestMeterCommand::outputReading_data()
{
    if (gitHubActionsRunnerOsVersion() >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 14)) {
        QSKIP("BLE controller operations hang on GitHub Actions's macOS 14 runners");
    }

    QTest::addColumn<QList<MultimeterService::Reading>>("readings");
    QTest::addColumn<AbstractCommand::OutputFormat>("format");

    const QList<MultimeterService::Reading> readings{
        { MultimeterService::MeterStatus::Ok, 0.0f, MultimeterService::Mode::Idle,
              +PokitMeter::VoltageRange::AutoRange },
        { MultimeterService::MeterStatus::AutoRangeOn, 1.0f, MultimeterService::Mode::DcVoltage,
              +PokitMeter::VoltageRange::_2V },
        { MultimeterService::MeterStatus::AutoRangeOff, -1.0f, MultimeterService::Mode::AcVoltage,
              +PokitMeter::VoltageRange::_2V },
        { MultimeterService::MeterStatus::AutoRangeOn, 0.5f, MultimeterService::Mode::DcCurrent,
              +PokitMeter::CurrentRange::_150mA },
        { MultimeterService::MeterStatus::AutoRangeOff, 0.1f, MultimeterService::Mode::AcCurrent,
              +PokitMeter::CurrentRange::_2A },
        { MultimeterService::MeterStatus::AutoRangeOff, 123.4f, MultimeterService::Mode::Resistance,
              +PokitMeter::CurrentRange::_2A },
        { MultimeterService::MeterStatus::AutoRangeOff, 576.89f, MultimeterService::Mode::Diode,
              +PokitMeter::CurrentRange::_2A },
        { MultimeterService::MeterStatus::Continuity, 0.0f, MultimeterService::Mode::Continuity,
              +PokitMeter::CurrentRange::AutoRange },
        { MultimeterService::MeterStatus::NoContinuity, 0.0f, MultimeterService::Mode::Continuity,
              +PokitMeter::CurrentRange::AutoRange },
        { MultimeterService::MeterStatus::AutoRangeOff, 32.0f, MultimeterService::Mode::Temperature,
              +PokitMeter::CurrentRange::AutoRange },
        { MultimeterService::MeterStatus::AutoRangeOff, 0.0005f, MultimeterService::Mode::Capacitance,
              +PokitPro::CapacitanceRange::_1mF },
        { MultimeterService::MeterStatus::Error, 0.0f, MultimeterService::Mode::Idle,
              +PokitMeter::VoltageRange::AutoRange },
    };

    #define DOKIT_ADD_TEST_ROW(name, readings) \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".csv"))) \
            << readings << AbstractCommand::OutputFormat::Csv; \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".json"))) \
            << readings << AbstractCommand::OutputFormat::Json; \
        QTest::newRow(qUtf8Printable(name + QStringLiteral(".txt"))) \
            << readings << AbstractCommand::OutputFormat::Text

    DOKIT_ADD_TEST_ROW(QStringLiteral("null"),
                         QList<MultimeterService::Reading>{ MultimeterService::Reading() });

    for (const MultimeterService::Reading &reading: readings) {
        QString name = MultimeterService::toString(reading.mode)
            .replace(QLatin1Char(' '), QLatin1Char('-'));
        if ((reading.mode == MultimeterService::Mode::Continuity) &&
            (!((quint8)reading.status & (quint8)MultimeterService::MeterStatus::Continuity))) {
            name.prepend(QStringLiteral("No")); // ie NoContinuity.
        }
        if (reading.status == MultimeterService::MeterStatus::Error) {
            name = QStringLiteral("Error");
        }
        DOKIT_ADD_TEST_ROW(name, QList<MultimeterService::Reading>{ reading });
    }

    DOKIT_ADD_TEST_ROW(QStringLiteral("all"), readings);
    #undef DOKIT_ADD_TEST_ROW
}

void TestMeterCommand::outputReading()
{
    QFETCH(QList<MultimeterService::Reading>, readings);
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    const OutputStreamCapture capture(&std::cout);
    MeterCommand command;
    command.service = new MultimeterService(QLowEnergyController::createCentral(QBluetoothDeviceInfo()));
    command.service->setPokitProduct((readings.first().mode == MultimeterService::Mode::Capacitance)
        ? PokitProduct::PokitPro : PokitProduct::PokitMeter); // Only Pokit Pro supports capacitance.
    command.format = format;
    for (const MultimeterService::Reading &reading: readings) {
        command.outputReading(reading);
    }
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestMeterCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    MeterCommand command;
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestMeterCommand)
