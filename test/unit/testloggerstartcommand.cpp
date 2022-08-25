// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testloggerstartcommand.h"
#include "outputstreamcapture.h"
#include "testdata.h"

#include "loggerstartcommand.h"

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)
Q_DECLARE_METATYPE(DataLoggerService::Mode);
Q_DECLARE_METATYPE(DataLoggerService::VoltageRange);
Q_DECLARE_METATYPE(DataLoggerService::CurrentRange);
Q_DECLARE_METATYPE(DataLoggerService::Range);
Q_DECLARE_METATYPE(DataLoggerService::Settings);

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

void TestLoggerStartCommand::requiredOptions()
{
    LoggerStartCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = mock.requiredOptions(parser) +
        QStringList{ QStringLiteral("mode"), QStringLiteral("range") };
    QCOMPARE(command.requiredOptions(parser), expected);
}

void TestLoggerStartCommand::supportedOptions()
{
    LoggerStartCommand command(this);
    MockDeviceCommand mock;
    QCommandLineParser parser;
    const QStringList expected = command.requiredOptions(parser) + mock.supportedOptions(parser) +
        QStringList{ QStringLiteral("interval"), QStringLiteral("timestamp")};
    QCOMPARE(command.supportedOptions(parser), expected);
}

void TestLoggerStartCommand::processOptions_data()
{
    QTest::addColumn<QStringList>("arguments");
    QTest::addColumn<DataLoggerService::Settings>("expected");
    QTest::addColumn<bool>("expectNowish");
    QTest::addColumn<QStringList>("errors");

    QTest::addRow("missing-required-options")
        << QStringList{}
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_30V_to_60V, 60000, 0}
        << false
        << QStringList{
            QStringLiteral("Missing required option: mode"),
            QStringLiteral("Missing required option: range") };

    QTest::addRow("missing-required-mode")
        << QStringList{
           QStringLiteral("--range"), QStringLiteral("100")}
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
            DataLoggerService::VoltageRange::_30V_to_60V, 60000, 0}
        << false
        << QStringList{ QStringLiteral("Missing required option: mode") };

    QTest::addRow("missing-required-range")
        << QStringList{
           QStringLiteral("--mode"), QStringLiteral("Vdc") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_30V_to_60V, 60000, 0}
        << false
        << QStringList{ QStringLiteral("Missing required option: range") };

    QTest::addRow("Vdc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV") }
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
            DataLoggerService::VoltageRange::_300mV_to_2V, 60000, 0}
        << true
        << QStringList{ };

    QTest::addRow("Vac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vac"),
           QStringLiteral("--range"), QStringLiteral("5V") }
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::AcVoltage,
            DataLoggerService::VoltageRange::_2V_to_6V, 60000, 0}
        << true
        << QStringList{ };

    QTest::addRow("Adc")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Adc"),
           QStringLiteral("--range"), QStringLiteral("100mA") }
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcCurrent,
            DataLoggerService::CurrentRange::_30mA_to_150mA, 60000, 0}
        << true
        << QStringList{ };

    QTest::addRow("Aac")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Aac"),
           QStringLiteral("--range"), QStringLiteral("2A") }
        << DataLoggerService::Settings{
            DataLoggerService::Command::Start, 0, DataLoggerService::Mode::AcCurrent,
            DataLoggerService::CurrentRange::_300mA_to_3A, 60000, 0}
        << true
        << QStringList{ };

    QTest::addRow("invalid-mode")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("invalid"),
           QStringLiteral("--range"), QStringLiteral("123") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_30V_to_60V, 60000, 0}
        << false
        << QStringList{ QStringLiteral("Unknown logger mode: invalid") };

    QTest::addRow("invalid-range")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("invalid") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_30V_to_60V, 60000, 0}
        << true
        << QStringList{ QStringLiteral("Invalid range value: invalid") };

    QTest::addRow("interval:100") // Defaults to 100 seconds (ie 100,000ms).
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("100") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 100000, 0}
        << true
        << QStringList{ };

    QTest::addRow("interval:499") // Defaults to 499 seconds.
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("499") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 499000, 0}
        << true
        << QStringList{ };

    QTest::addRow("interval:500") // Defaults to 500ms.
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("500") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 500, 0}
        << true
        << QStringList{ };

    QTest::addRow("invalid-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("invalid") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 60000, 0}
        << true
        << QStringList{ QStringLiteral("Invalid interval value: invalid") };

    QTest::addRow("negative-interval")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--interval"), QStringLiteral("-123") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 60000, 0}
        << true
        << QStringList{ QStringLiteral("Invalid interval value: -123") };

    QTest::addRow("timestamp:0")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--timestamp"), QStringLiteral("0") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 60000, 0}
        << false
        << QStringList{ };

    QTest::addRow("timestamp:1")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--timestamp"), QStringLiteral("1") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 60000, 1}
        << false
        << QStringList{ };

    QTest::addRow("timestamp:now")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--timestamp"), QString::number(QDateTime::currentSecsSinceEpoch()) }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 60000, 0}
        << true
        << QStringList{ };

    QTest::addRow("invalid-timestamp")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--timestamp"), QStringLiteral("invalid") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 60000, 0}
        << true
        << QStringList{ QStringLiteral("Invalid timestamp value: invalid") };

    QTest::addRow("negative-timestamp")
        << QStringList{
           QStringLiteral("--mode"),  QStringLiteral("Vdc"),
           QStringLiteral("--range"), QStringLiteral("1000mV"),
           QStringLiteral("--timestamp"), QStringLiteral("-123") }
        << DataLoggerService::Settings{
           DataLoggerService::Command::Start, 0, DataLoggerService::Mode::DcVoltage,
           DataLoggerService::VoltageRange::_300mV_to_2V, 60000, 0}
        << true
        << QStringList{ QStringLiteral("Invalid timestamp value: -123") };
}

void TestLoggerStartCommand::processOptions()
{
    QFETCH(QStringList, arguments);
    QFETCH(DataLoggerService::Settings, expected);
    QFETCH(bool, expectNowish);
    QFETCH(QStringList, errors);

    arguments.prepend(QStringLiteral("pokit")); // The first argument is always the app name.

    QCommandLineParser parser;
    parser.addOption({QStringLiteral("mode"), QStringLiteral("description"), QStringLiteral("mode")});
    parser.addOption({QStringLiteral("range"), QStringLiteral("description"), QStringLiteral("range")});
    parser.addOption({QStringLiteral("interval"), QStringLiteral("description"), QStringLiteral("interval")});
    parser.addOption({QStringLiteral("timestamp"), QStringLiteral("description"), QStringLiteral("timestamp")});
    parser.process(arguments);

    LoggerStartCommand command(this);
    const quint32 now = (quint32)QDateTime::currentSecsSinceEpoch();
    QCOMPARE(command.processOptions(parser),  errors);
    QCOMPARE(command.settings.command,        expected.command);
    QCOMPARE(command.settings.arguments,      expected.arguments);
    QCOMPARE(command.settings.mode,           expected.mode);
    QCOMPARE(command.settings.range,          expected.range);
    QCOMPARE(command.settings.updateInterval, expected.updateInterval);

    // Recoginise if/when LoggerStartCommand::processOptions automatically uses 'now' for timestamp.
    if (expectNowish) {
        Q_ASSERT(expected.timestamp == 0);
        // Fuzzy compare, with up 3 seconds tolerance. Using qMin and qMax here instead of qAbs to
        // avoid negative wrap-around, since both values are unsigned.
        QVERIFY((qMax(now, command.settings.timestamp) -
                 qMin(now, command.settings.timestamp)) <= 3);
    } else {
        QCOMPARE(command.settings.timestamp,  expected.timestamp);
    }
}

void TestLoggerStartCommand::getService()
{
    // Unable to safely invoke LoggerStartCommand::getService() without a valid Bluetooth device.
}

void TestLoggerStartCommand::serviceDetailsDiscovered()
{
    // Unable to safely invoke LoggerStartCommand::serviceDetailsDiscovered() without a valid service.
}

void TestLoggerStartCommand::lowestRange_data()
{
    QTest::addColumn<DataLoggerService::Mode>("mode");
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<DataLoggerService::Range>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mVdc") \
            << DataLoggerService::Mode::DcVoltage << (quint32)value \
            << DataLoggerService::Range(DataLoggerService::VoltageRange::range); \
        QTest::addRow(#value "mVac") \
            << DataLoggerService::Mode::AcVoltage << (quint32)value \
            << DataLoggerService::Range(DataLoggerService::VoltageRange::range)
    QTPOKIT_ADD_TEST_ROW(    0, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  299, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  300, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  301, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 1999, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 2000, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 2001, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 5999, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 6000, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 6001, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(11999, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(12000, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(12001, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(29999, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(30000, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(30001, _30V_to_60V);
    QTPOKIT_ADD_TEST_ROW(99999, _30V_to_60V); // DataLogger has no 'auto', so use the max range available.
    /// \todo Pokit Pro supports up to 600V.
    #undef QTPOKIT_ADD_TEST_ROW

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mAdc") \
            << DataLoggerService::Mode::DcCurrent << (quint32)value \
            << DataLoggerService::Range(DataLoggerService::CurrentRange::range); \
        QTest::addRow(#value "mAac") \
            << DataLoggerService::Mode::AcCurrent << (quint32)value \
            << DataLoggerService::Range(DataLoggerService::CurrentRange::range)
    QTPOKIT_ADD_TEST_ROW(   0, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(   9, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(  10, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(  11, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  29, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  30, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  31, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 149, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 150, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 151, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 299, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 300, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 301, _300mA_to_3A);
    QTPOKIT_ADD_TEST_ROW(9999, _300mA_to_3A);
    /// \todo Pokit Pro supports up to 10A.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestLoggerStartCommand::lowestRange()
{
    QFETCH(DataLoggerService::Mode, mode);
    QFETCH(quint32, desiredMax);
    QFETCH(DataLoggerService::Range, expected);
    QCOMPARE(LoggerStartCommand::lowestRange(mode, desiredMax), expected);
}

void TestLoggerStartCommand::lowestCurrentRange_data()
{
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<DataLoggerService::CurrentRange>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mA") << (quint32)value << DataLoggerService::CurrentRange::range
    QTPOKIT_ADD_TEST_ROW(   0, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(   9, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(  10, _0_to_10mA);
    QTPOKIT_ADD_TEST_ROW(  11, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  29, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  30, _10mA_to_30mA);
    QTPOKIT_ADD_TEST_ROW(  31, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 149, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 150, _30mA_to_150mA);
    QTPOKIT_ADD_TEST_ROW( 151, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 299, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 300, _150mA_to_300mA);
    QTPOKIT_ADD_TEST_ROW( 301, _300mA_to_3A);
    QTPOKIT_ADD_TEST_ROW(9999, _300mA_to_3A);
    /// \todo Pokit Pro supports up to 10A.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestLoggerStartCommand::lowestCurrentRange()
{
    QFETCH(quint32, desiredMax);
    QFETCH(DataLoggerService::CurrentRange, expected);
    QCOMPARE(LoggerStartCommand::lowestCurrentRange(desiredMax), expected);
}

void TestLoggerStartCommand::lowestVoltageRange_data()
{
    QTest::addColumn<quint32>("desiredMax");
    QTest::addColumn<DataLoggerService::VoltageRange>("expected");

    #define QTPOKIT_ADD_TEST_ROW(value, range) \
        QTest::addRow(#value "mV") << (quint32)value << DataLoggerService::VoltageRange::range
    QTPOKIT_ADD_TEST_ROW(    0, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  299, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  300, _0_to_300mV);
    QTPOKIT_ADD_TEST_ROW(  301, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 1999, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 2000, _300mV_to_2V);
    QTPOKIT_ADD_TEST_ROW( 2001, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 5999, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 6000, _2V_to_6V);
    QTPOKIT_ADD_TEST_ROW( 6001, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(11999, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(12000, _6V_to_12V);
    QTPOKIT_ADD_TEST_ROW(12001, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(29999, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(30000, _12V_to_30V);
    QTPOKIT_ADD_TEST_ROW(30001, _30V_to_60V);
    QTPOKIT_ADD_TEST_ROW(99999, _30V_to_60V); // DataLogger has no 'auto', so use the max range available.
    /// \todo Pokit Pro supports up to 600V.
    #undef QTPOKIT_ADD_TEST_ROW
}

void TestLoggerStartCommand::lowestVoltageRange()
{
    QFETCH(quint32, desiredMax);
    QFETCH(DataLoggerService::VoltageRange, expected);
    QCOMPARE(LoggerStartCommand::lowestVoltageRange(desiredMax), expected);
}

void TestLoggerStartCommand::settingsWritten_data()
{
    QTest::addColumn<AbstractCommand::OutputFormat>("format");
    QTest::newRow("1.csv") << AbstractCommand::OutputFormat::Csv;
    QTest::newRow("1.json") << AbstractCommand::OutputFormat::Json;
    QTest::newRow("1.txt") << AbstractCommand::OutputFormat::Text;
}

void TestLoggerStartCommand::settingsWritten()
{
    QFETCH(AbstractCommand::OutputFormat, format);
    LOADTESTDATA(expected);

    OutputStreamCapture capture(&std::cout);
    LoggerStartCommand command(nullptr);
    command.format = format;
    command.settingsWritten();
    QCOMPARE(QByteArray::fromStdString(capture.data()), expected);
}

void TestLoggerStartCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    LoggerStartCommand command(nullptr);
    QVERIFY(!command.tr("ignored").isEmpty());
}

QTEST_MAIN(TestLoggerStartCommand)
