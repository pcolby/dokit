// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testabstractcommand.h"
#include "../stringliterals_p.h"

#include "abstractcommand.h"

#include <qtpokit/pokitdiscoveryagent.h>

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)

DOKIT_USE_STRINGLITERALS

class MockCommand : public AbstractCommand
{
public:
    explicit MockCommand(QObject * const parent = nullptr) : AbstractCommand(parent)
    {

    }

    QStringList requiredOptions(const QCommandLineParser &parser) const override
    {
        return AbstractCommand::requiredOptions(parser) +
            QStringList{ u"mockRequired"_s };
    }

    bool start() override
    {
        return true;
    }

    void deviceDiscovered(const QBluetoothDeviceInfo &info) override
    {
        Q_UNUSED(info)
    }

    void deviceDiscoveryFinished() override
    {

    }
};

void TestAbstractCommand::requiredOptions()
{
    MockCommand mock;
    QCommandLineParser parser;
    QCOMPARE(mock.requiredOptions(parser), QStringList{ u"mockRequired"_s });
}

void TestAbstractCommand::supportedOptions()
{
    MockCommand mock;
    QCommandLineParser parser;
    QVERIFY(mock.supportedOptions(parser).contains(u"mockRequired"_s));
}

void TestAbstractCommand::escapeCsvField_data()
{
    QTest::addColumn<QString>("field");
    QTest::addColumn<QString>("expected");
    QTest::addRow("<empty>") << QString()   << QString();
    QTest::addRow("abc")     << u"abc"_s    << u"abc"_s;
    QTest::addRow("a,c")     << u"a,c"_s    << uR"("a,c")"_s;
    QTest::addRow(R"(a"c)")  << uR"(a"c)"_s << uR"("a""c")"_s;
}

void TestAbstractCommand::escapeCsvField()
{
    QFETCH(QString, field);
    QFETCH(QString, expected);
    QCOMPARE(AbstractCommand::escapeCsvField(field), expected);
}

void TestAbstractCommand::parseMicroValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("unit");
    QTest::addColumn<quint32>("sensibleMinimum");
    QTest::addColumn<quint32>("expected");

    // With valid units (so sensibleMinimum is ignored).
    QTest::addRow("123us") << u"123us"_s << u"s"_s << (quint32)100 << (quint32)123;
    QTest::addRow("123ms") << u"123ms"_s << u"s"_s << (quint32)100 << (quint32)123000;
    QTest::addRow("123s")  << u"123s"_s << u"s"_s << (quint32)100 << (quint32)123000000;

    // Without units, result depends on sensibleMinimum.
    QTest::addRow("bigger")  << u"123"_s << QString() << (quint32)122 << (quint32)123;
    QTest::addRow("equal")   << u"123"_s << QString() << (quint32)123 << (quint32)123;
    QTest::addRow("smaller") << u"123"_s << QString() << (quint32)124 << (quint32)123000;

    // Floats work the same.
    QTest::addRow("123.0")  << u"123.0"_s  << QString() << (quint32)100 << (quint32)123;
    QTest::addRow("1.230")  << u"1.230"_s  << QString() << (quint32)100 << (quint32)1230;
    QTest::addRow("123.0s") << u"123.0s"_s << u"s"_s << (quint32)100 << (quint32)123000000;
    QTest::addRow("1.230s") << u"1.230s"_s << u"s"_s << (quint32)100 << (quint32)1230000;

    // Invalid numbers return 0.
    QTest::addRow("xxx") << u"xxx"_s << QString() << (quint32)100 << (quint32)0;
    QTest::addRow("12x") << u"12x"_s << QString() << (quint32)100 << (quint32)0;

    // Negative numbers are invalid too (ie also return 0).
    QTest::addRow("-1")   << u"-1"_s   << QString() << (quint32)100 << (quint32)0;
    QTest::addRow("-0.1") << u"-0.1"_s << QString() << (quint32)100 << (quint32)0;
}

void TestAbstractCommand::parseMicroValue()
{
    QFETCH(QString, value);
    QFETCH(QString, unit);
    QFETCH(quint32, sensibleMinimum);
    QFETCH(quint32, expected);
    QCOMPARE(AbstractCommand::parseNumber<std::micro>(value, unit, sensibleMinimum), expected);
}

void TestAbstractCommand::parseMilliValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("unit");
    QTest::addColumn<quint32>("sensibleMinimum");
    QTest::addColumn<quint32>("expected");

    // With valid units (so sensibleMinimum is ignored).
    QTest::addRow("123ms") << u"123ms"_s << u"s"_s << (quint32)100 << (quint32)123;
    QTest::addRow("123s")  << u"123s"_s << u"s"_s << (quint32)100 << (quint32)123000;

    // Without units, result depends on sensibleMinimum.
    QTest::addRow("bigger")  << u"123"_s << QString() << (quint32)122 << (quint32)123;
    QTest::addRow("equal")   << u"123"_s << QString() << (quint32)123 << (quint32)123;
    QTest::addRow("smaller") << u"123"_s << QString() << (quint32)124 << (quint32)123000;

    // Floats work the same.
    QTest::addRow("123.0")  << u"123.0"_s  << QString() << (quint32)100 << (quint32)123;
    QTest::addRow("1.230")  << u"1.230"_s  << QString() << (quint32)100 << (quint32)1230;
    QTest::addRow("123.0s") << u"123.0s"_s << u"s"_s << (quint32)100 << (quint32)123000;
    QTest::addRow("1.230s") << u"1.230s"_s << u"s"_s << (quint32)100 << (quint32)1230;

    // Invalid numbers return 0.
    QTest::addRow("xxx") << u"xxx"_s << QString() << (quint32)100 << (quint32)0;
    QTest::addRow("12x") << u"12x"_s << QString() << (quint32)100 << (quint32)0;

    // Negative numbers are invalid too (ie also return 0).
    QTest::addRow("-1")   << u"-1"_s   << QString() << (quint32)100 << (quint32)0;
    QTest::addRow("-0.1") << u"-0.1"_s << QString() << (quint32)100 << (quint32)0;
}

void TestAbstractCommand::parseMilliValue()
{
    QFETCH(QString, value);
    QFETCH(QString, unit);
    QFETCH(quint32, sensibleMinimum);
    QFETCH(quint32, expected);
    QCOMPARE(AbstractCommand::parseNumber<std::milli>(value, unit, sensibleMinimum), expected);
}

void TestAbstractCommand::parseWholeValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("unit");
    QTest::addColumn<quint32>("expected");

    // With valid units.
    QTest::addRow("123V")  << u"123V"_s  << u"V"_s << (quint32)123;
    QTest::addRow("123KV") << u"123KV"_s << u"V"_s << (quint32)123000;
    QTest::addRow("123MV") << u"123MV"_s << u"V"_s << (quint32)123000000;

    // Without units.
    QTest::addRow("123")  << u"123"_s  << u"V"_s << (quint32)123;
    QTest::addRow("123k") << u"123k"_s << u"V"_s << (quint32)123000;
    QTest::addRow("123M") << u"123M"_s << u"V"_s << (quint32)123000000;

    // Floats work the same.
    QTest::addRow("123.0") << u"123.0"_s << QString() << (quint32)123;
    QTest::addRow("1.23K") << u"1.23K"_s << QString() << (quint32)1230;
    QTest::addRow("1.23M") << u"1.23M"_s << QString() << (quint32)1230000;

    // Invalid numbers return 0.
    QTest::addRow("xxx") << u"xxx"_s << QString() << (quint32)0;
    QTest::addRow("12x") << u"12x"_s << QString() << (quint32)0;

    // Negative numbers are invalid too (ie also return 0).
    QTest::addRow("-1")   << u"-1"_s   << QString() << (quint32)0;
    QTest::addRow("-0.1") << u"-0.1"_s << QString() << (quint32)0;
}

void TestAbstractCommand::parseWholeValue()
{
    QFETCH(QString, value);
    QFETCH(QString, unit);
    QFETCH(quint32, expected);
    const auto actual = AbstractCommand::parseNumber<std::ratio<1>>(value, unit);
    QCOMPARE(actual, expected);
}

void TestAbstractCommand::processOptions()
{
    QCommandLineParser parser;
    QVERIFY(parser.addOptions({
        { u"device"_s,       u"desc"_s, u"value"_s },
        { u"mockRequired"_s, u"desc"_s, u"value"_s },
        { u"output"_s,       u"desc"_s, u"value"_s },
        { u"timeout"_s,      u"desc"_s, u"value"_s },
    }));

    // With no options, processOptions should fail because our mockRequired option is required.
    QVERIFY(parser.parse(QStringList{
        u"executableName"_s,
    }));
    MockCommand mock;
    QStringList errors = mock.processOptions(parser);
    QCOMPARE(errors.length(), 1);
    QVERIFY(errors.first().contains(u"missing"_s, Qt::CaseInsensitive));
    QVERIFY(errors.first().contains(u"mockRequired"_s, Qt::CaseSensitive));

    // With our required option, processOptions should now pass.
    QVERIFY(parser.parse(QStringList{
        u"executableName"_s,
        u"--mockRequired=abc123"_s,
    }));
    errors = mock.processOptions(parser);
    QVERIFY(errors.isEmpty());

    // Extraneous options should not cause any errors (though warnings will be logged).
    QVERIFY(parser.addOption({ u"erroneous"_s, u"desc"_s }));
    QVERIFY(parser.parse(QStringList{
        u"executableName"_s,
        u"--mockRequired=abc123"_s,
        u"--erroneous"_s,
    }));
    QTest::ignoreMessage(QtInfoMsg, "Ignoring option: erroneous");
    errors = mock.processOptions(parser);
    QVERIFY(errors.isEmpty());
}

void TestAbstractCommand::processOptions_device_data()
{
    QTest::addColumn<QString>("device");
    QTest::addRow("<empty>")  << QString();
    QTest::addRow("valid")   << u"valid"_s;
}

void TestAbstractCommand::processOptions_device()
{
    QFETCH(QString, device);

    QCommandLineParser parser;
    QVERIFY(parser.addOptions({
        { u"device"_s,       u"desc"_s, u"value"_s },
        { u"mockRequired"_s, u"desc"_s, u"value"_s },
        { u"output"_s,       u"desc"_s, u"value"_s },
        { u"timeout"_s,      u"desc"_s, u"value"_s },
    }));
    QVERIFY(parser.parse(QStringList{
        u"executableName"_s,
        u"--mockRequired=abc123"_s,
        u"--device"_s, device
    }));

    MockCommand mock;
    QVERIFY(mock.deviceToScanFor.isEmpty());
    const QStringList errors = mock.processOptions(parser);
    QVERIFY(errors.isEmpty());
    QCOMPARE(mock.deviceToScanFor, device);
}

void TestAbstractCommand::processOptions_output_data()
{
    QTest::addColumn<QString>("argument");
    QTest::addColumn<AbstractCommand::OutputFormat>("expected");
    QTest::addColumn<bool>("expectErrors");

    // Valid values.
    QTest::addRow("csv" ) << u"csv"_s  << AbstractCommand::OutputFormat::Csv  << false;
    QTest::addRow("cSv" ) << u"cSv"_s  << AbstractCommand::OutputFormat::Csv  << false;
    QTest::addRow("CSV" ) << u"CSV"_s  << AbstractCommand::OutputFormat::Csv  << false;
    QTest::addRow("json") << u"json"_s << AbstractCommand::OutputFormat::Json << false;
    QTest::addRow("jSoN") << u"jSoN"_s << AbstractCommand::OutputFormat::Json << false;
    QTest::addRow("JSON") << u"JSON"_s << AbstractCommand::OutputFormat::Json << false;
    QTest::addRow("text") << u"text"_s << AbstractCommand::OutputFormat::Text << false;
    QTest::addRow("tExT") << u"tExT"_s << AbstractCommand::OutputFormat::Text << false;
    QTest::addRow("TEXT") << u"TEXT"_s << AbstractCommand::OutputFormat::Text << false;

    // Invalid values should all remain as the default Text.
    QTest::addRow("<empty>") << QString()             << AbstractCommand::OutputFormat::Text << true;
    QTest::addRow("foo")     << u"foo"_s << AbstractCommand::OutputFormat::Text << true;
    QTest::addRow("bar")     << u"bar"_s << AbstractCommand::OutputFormat::Text << true;
    QTest::addRow("tsv")     << u"tsv"_s << AbstractCommand::OutputFormat::Text << true;
}

void TestAbstractCommand::processOptions_output()
{
    QFETCH(QString, argument);
    QFETCH(AbstractCommand::OutputFormat, expected);
    QFETCH(bool, expectErrors);

    QCommandLineParser parser;
    QVERIFY(parser.addOptions({
        { u"device"_s,       u"desc"_s, u"value"_s },
        { u"mockRequired"_s, u"desc"_s, u"value"_s },
        { u"output"_s,       u"desc"_s, u"value"_s },
        { u"timeout"_s,      u"desc"_s, u"value"_s },
    }));
    QVERIFY(parser.parse(QStringList{
        u"executableName"_s,
        u"--mockRequired=abc123"_s,
        u"--output"_s, argument
    }));

    MockCommand mock;
    QCOMPARE(mock.format, AbstractCommand::OutputFormat::Text); // The default format is Text.
    const QStringList errors = mock.processOptions(parser);
    QCOMPARE(!errors.isEmpty(), expectErrors);
    QCOMPARE(mock.format, expected);
}

void TestAbstractCommand::processOptions_timeout_data()
{
    QTest::addColumn<QString>("argument");
    QTest::addColumn<int>("expected");
    QTest::addColumn<bool>("expectErrors");

    // Valid values.
    QTest::addRow(  "1ms") << u"1ms"_s <<      1 << false;
    QTest::addRow(    "1") << u"1"_s <<   1000 << false;
    QTest::addRow(  "123") << u"123"_s << 123000 << false;
    QTest::addRow( "1000") << u"1000"_s <<   1000 << false;
    QTest::addRow( "50ms") << u"50ms"_s <<     50 << false;
    QTest::addRow("500ms") << u"500ms"_s <<    500 << false;
    QTest::addRow(  "60s") << u"60s"_s <<  60000 << false;

    // Invalid values should all remain as the default Text.
    QTest::addRow(    "-10") << u"-10"_s << 0 << true;
    QTest::addRow(     "-1") << u"-1"_s << 0 << true;
    QTest::addRow(      "0") << u"0"_s << 0 << true;
    QTest::addRow("<empty>") << QString()               << 0 << true;
    QTest::addRow(    "foo") << u"foo"_s << 0 << true;
    QTest::addRow(    "bar") << u"bar"_s << 0 << true;
}

void TestAbstractCommand::processOptions_timeout()
{
    QFETCH(QString, argument);
    QFETCH(int, expected);
    QFETCH(bool, expectErrors);

    QCommandLineParser parser;
    QVERIFY(parser.addOptions({
        { u"device"_s,       u"desc"_s, u"value"_s },
        { u"mockRequired"_s, u"desc"_s, u"value"_s },
        { u"output"_s,       u"desc"_s, u"value"_s },
        { u"timeout"_s,      u"desc"_s, u"value"_s },
    }));
    QVERIFY(parser.parse(QStringList{
        u"executableName"_s,
        u"--mockRequired=abc123"_s,
        u"--timeout"_s, argument
    }));

    MockCommand mock;
    const int defaultTimeout = mock.discoveryAgent->lowEnergyDiscoveryTimeout(); // eg 20s on Linux.
    if ((!expectErrors) && (defaultTimeout == -1)) {
        QTest::ignoreMessage(QtWarningMsg, "Platform does not support Bluetooth scan timeout");
    }
    const QStringList errors = mock.processOptions(parser);
    QCOMPARE(!errors.isEmpty(), expectErrors);
    QCOMPARE(mock.discoveryAgent->lowEnergyDiscoveryTimeout(),
        ((expectErrors || defaultTimeout == -1)) ? defaultTimeout : expected);
}

void TestAbstractCommand::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    MockCommand mock;
    QVERIFY(!mock.tr("ignored").isEmpty());
}

QTEST_MAIN(TestAbstractCommand)
