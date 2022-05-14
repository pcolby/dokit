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

#include "testabstractcommand.h"

#include "abstractcommand.h"

#include <qtpokit/pokitdiscoveryagent.h>

Q_DECLARE_METATYPE(AbstractCommand::OutputFormat)

class MockCommand : public AbstractCommand
{
public:
    MockCommand() : AbstractCommand(nullptr)
    {

    }

    QStringList requiredOptions(const QCommandLineParser &parser) const override
    {
        return AbstractCommand::requiredOptions(parser) +
            QStringList{ QStringLiteral("mockRequired") };
    }

    bool start() override
    {
        return true;
    }

    void deviceDiscovered(const QBluetoothDeviceInfo &info) override
    {
        Q_UNUSED(info);
    }

    void deviceDiscoveryFinished() override
    {

    }
};

void TestAbstractCommand::requiredOptions()
{
    MockCommand mock;
    QCommandLineParser parser;
    QCOMPARE(mock.requiredOptions(parser), QStringList{ QStringLiteral("mockRequired") });
}

void TestAbstractCommand::supportedOptions()
{
    MockCommand mock;
    QCommandLineParser parser;
    QVERIFY(mock.supportedOptions(parser).contains(QStringLiteral("mockRequired")));
}

void TestAbstractCommand::escapeCsvField_data()
{
    QTest::addColumn<QString>("field");
    QTest::addColumn<QString>("expected");
    QTest::addRow("<empty>") << QString()              << QString();
    QTest::addRow("abc")     << QStringLiteral("abc")  << QStringLiteral("abc");
    QTest::addRow("a,c")     << QStringLiteral("a,c")  << QStringLiteral("\"a,c\"");
    QTest::addRow("a\"c")    << QStringLiteral("a\"c") << QStringLiteral("\"a\"\"c\"");
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
    QTest::addRow("123us") << QStringLiteral("123us") << QStringLiteral("s") << (quint32)100 << (quint32)123;
    QTest::addRow("123ms") << QStringLiteral("123ms") << QStringLiteral("s") << (quint32)100 << (quint32)123000;
    QTest::addRow("123s")  << QStringLiteral( "123s") << QStringLiteral("s") << (quint32)100 << (quint32)123000000;

    // Without units, result depends on sensibleMinimum.
    QTest::addRow("bigger")  << QStringLiteral("123") << QString() << (quint32)122 << (quint32)123;
    QTest::addRow("equal")   << QStringLiteral("123") << QString() << (quint32)123 << (quint32)123;
    QTest::addRow("smaller") << QStringLiteral("123") << QString() << (quint32)124 << (quint32)123000;

    // Floats work the same.
    QTest::addRow("123.0")  << QStringLiteral("123.0")  << QString() << (quint32)100 << (quint32)123;
    QTest::addRow("1.230")  << QStringLiteral("1.230")  << QString() << (quint32)100 << (quint32)1230;
    QTest::addRow("123.0s") << QStringLiteral("123.0s") << QStringLiteral("s") << (quint32)100 << (quint32)123000000;
    QTest::addRow("1.230s") << QStringLiteral("1.230s") << QStringLiteral("s") << (quint32)100 << (quint32)1230000;

    // Invalid numbers return 0.
    QTest::addRow("xxx") << QStringLiteral("xxx") << QString() << (quint32)100 << (quint32)0;
    QTest::addRow("12x") << QStringLiteral("12x") << QString() << (quint32)100 << (quint32)0;
}

void TestAbstractCommand::parseMicroValue()
{
    QFETCH(QString, value);
    QFETCH(QString, unit);
    QFETCH(quint32, sensibleMinimum);
    QFETCH(quint32, expected);
    QCOMPARE(AbstractCommand::parseMicroValue(value, unit, sensibleMinimum), expected);
}

void TestAbstractCommand::parseMilliValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("unit");
    QTest::addColumn<quint32>("sensibleMinimum");
    QTest::addColumn<quint32>("expected");

    // With valid units (so sensibleMinimum is ignored).
    QTest::addRow("123ms") << QStringLiteral("123ms") << QStringLiteral("s") << (quint32)100 << (quint32)123;
    QTest::addRow("123s")  << QStringLiteral( "123s") << QStringLiteral("s") << (quint32)100 << (quint32)123000;

    // Without units, result depends on sensibleMinimum.
    QTest::addRow("bigger")  << QStringLiteral("123") << QString() << (quint32)122 << (quint32)123;
    QTest::addRow("equal")   << QStringLiteral("123") << QString() << (quint32)123 << (quint32)123;
    QTest::addRow("smaller") << QStringLiteral("123") << QString() << (quint32)124 << (quint32)123000;

    // Floats work the same.
    QTest::addRow("123.0")  << QStringLiteral("123.0")  << QString() << (quint32)100 << (quint32)123;
    QTest::addRow("1.230")  << QStringLiteral("1.230")  << QString() << (quint32)100 << (quint32)1230;
    QTest::addRow("123.0s") << QStringLiteral("123.0s") << QStringLiteral("s") << (quint32)100 << (quint32)123000;
    QTest::addRow("1.230s") << QStringLiteral("1.230s") << QStringLiteral("s") << (quint32)100 << (quint32)1230;

    // Invalid numbers return 0.
    QTest::addRow("xxx") << QStringLiteral("xxx") << QString() << (quint32)100 << (quint32)0;
    QTest::addRow("12x") << QStringLiteral("12x") << QString() << (quint32)100 << (quint32)0;
}

void TestAbstractCommand::parseMilliValue()
{
    QFETCH(QString, value);
    QFETCH(QString, unit);
    QFETCH(quint32, sensibleMinimum);
    QFETCH(quint32, expected);
    QCOMPARE(AbstractCommand::parseMilliValue(value, unit, sensibleMinimum), expected);
}

void TestAbstractCommand::parseWholeValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("unit");
    QTest::addColumn<quint32>("expected");

    // With valid units.
    QTest::addRow("123V")  << QStringLiteral("123V")  << QStringLiteral("V") << (quint32)123;
    QTest::addRow("123KV") << QStringLiteral("123KV") << QStringLiteral("V") << (quint32)123000;
    QTest::addRow("123MV") << QStringLiteral("123MV") << QStringLiteral("V") << (quint32)123000000;

    // Without units.
    QTest::addRow("123")  << QStringLiteral("123")  << QStringLiteral("V") << (quint32)123;
    QTest::addRow("123k") << QStringLiteral("123k") << QStringLiteral("V") << (quint32)123000;
    QTest::addRow("123M") << QStringLiteral("123M") << QStringLiteral("V") << (quint32)123000000;

    // Floats work the same.
    QTest::addRow("123.0") << QStringLiteral("123.0") << QString() << (quint32)123;
    QTest::addRow("1.23K") << QStringLiteral("1.23K") << QString() << (quint32)1230;
    QTest::addRow("1.23M") << QStringLiteral("1.23M") << QString() << (quint32)1230000;

    // Invalid numbers return 0.
    QTest::addRow("xxx") << QStringLiteral("xxx") << QString() << (quint32)0;
    QTest::addRow("12x") << QStringLiteral("12x") << QString() << (quint32)0;
}

void TestAbstractCommand::parseWholeValue()
{
    QFETCH(QString, value);
    QFETCH(QString, unit);
    QFETCH(quint32, expected);
    QCOMPARE(AbstractCommand::parseWholeValue(value, unit), expected);
}

void TestAbstractCommand::processOptions()
{
    QCommandLineParser parser;
    QVERIFY(parser.addOptions({
        { QStringLiteral("device"),       QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("mockRequired"), QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("output"),       QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("timeout"),      QStringLiteral("desc"), QStringLiteral("value") },
    }));

    // With no options, processOptions should fail because our mockRequired option is required.
    QVERIFY(parser.parse(QStringList{
        QStringLiteral("executableName"),
    }));
    MockCommand mock;
    QStringList errors = mock.processOptions(parser);
    QCOMPARE(errors.length(), 1);
    QVERIFY(errors.first().contains(QStringLiteral("missing"), Qt::CaseInsensitive));
    QVERIFY(errors.first().contains(QStringLiteral("mockRequired"), Qt::CaseSensitive));

    // With our required option, processOptions should now pass.
    QVERIFY(parser.parse(QStringList{
        QStringLiteral("executableName"),
        QStringLiteral("--mockRequired=abc123"),
    }));
    errors = mock.processOptions(parser);
    QVERIFY(errors.isEmpty());

    // Extraneous options should not cause any errors (though warnings will be logged).
    QVERIFY(parser.addOption({ QStringLiteral("erroneous"), QStringLiteral("desc") }));
    QVERIFY(parser.parse(QStringList{
        QStringLiteral("executableName"),
        QStringLiteral("--mockRequired=abc123"),
        QStringLiteral("--erroneous"),
    }));
    errors = mock.processOptions(parser);
    QVERIFY(errors.isEmpty());
}

void TestAbstractCommand::processOptions_device_data()
{
    QTest::addColumn<QString>("device");
    QTest::addRow("<empty>")  << QString();
    QTest::addRow("valid")   << QStringLiteral("valid");
}

void TestAbstractCommand::processOptions_device()
{
    QFETCH(QString, device);

    QCommandLineParser parser;
    QVERIFY(parser.addOptions({
        { QStringLiteral("device"),       QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("mockRequired"), QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("output"),       QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("timeout"),      QStringLiteral("desc"), QStringLiteral("value") },
    }));
    QVERIFY(parser.parse(QStringList{
        QStringLiteral("executableName"),
        QStringLiteral("--mockRequired=abc123"),
        QStringLiteral("--device"), device
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
    QTest::addRow("csv" ) << QStringLiteral("csv" ) << AbstractCommand::OutputFormat::Csv  << false;
    QTest::addRow("cSv" ) << QStringLiteral("cSv" ) << AbstractCommand::OutputFormat::Csv  << false;
    QTest::addRow("CSV" ) << QStringLiteral("CSV" ) << AbstractCommand::OutputFormat::Csv  << false;
    QTest::addRow("json") << QStringLiteral("json") << AbstractCommand::OutputFormat::Json << false;
    QTest::addRow("jSoN") << QStringLiteral("jSoN") << AbstractCommand::OutputFormat::Json << false;
    QTest::addRow("JSON") << QStringLiteral("JSON") << AbstractCommand::OutputFormat::Json << false;
    QTest::addRow("text") << QStringLiteral("text") << AbstractCommand::OutputFormat::Text << false;
    QTest::addRow("tExT") << QStringLiteral("tExT") << AbstractCommand::OutputFormat::Text << false;
    QTest::addRow("TEXT") << QStringLiteral("TEXT") << AbstractCommand::OutputFormat::Text << false;

    // Invalid values should all remain as the default Text.
    QTest::addRow("<empty>") << QString()             << AbstractCommand::OutputFormat::Text << true;
    QTest::addRow("foo")     << QStringLiteral("foo") << AbstractCommand::OutputFormat::Text << true;
    QTest::addRow("bar")     << QStringLiteral("bar") << AbstractCommand::OutputFormat::Text << true;
    QTest::addRow("tsv")     << QStringLiteral("tsv") << AbstractCommand::OutputFormat::Text << true;
}

void TestAbstractCommand::processOptions_output()
{
    QFETCH(QString, argument);
    QFETCH(AbstractCommand::OutputFormat, expected);
    QFETCH(bool, expectErrors);

    QCommandLineParser parser;
    QVERIFY(parser.addOptions({
        { QStringLiteral("device"),       QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("mockRequired"), QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("output"),       QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("timeout"),      QStringLiteral("desc"), QStringLiteral("value") },
    }));
    QVERIFY(parser.parse(QStringList{
        QStringLiteral("executableName"),
        QStringLiteral("--mockRequired=abc123"),
        QStringLiteral("--output"), argument
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
    QTest::addRow(  "1ms") << QStringLiteral(  "1ms") <<      1 << false;
    QTest::addRow(    "1") << QStringLiteral(    "1") <<   1000 << false;
    QTest::addRow(  "123") << QStringLiteral(  "123") << 123000 << false;
    QTest::addRow( "1000") << QStringLiteral( "1000") <<   1000 << false;
    QTest::addRow( "50ms") << QStringLiteral( "50ms") <<     50 << false;
    QTest::addRow("500ms") << QStringLiteral("500ms") <<    500 << false;
    QTest::addRow(  "60s") << QStringLiteral(  "60s") <<  60000 << false;

    // Invalid values should all remain as the default Text.
    QTest::addRow(    "-10") << QStringLiteral(  "-10") << 0 << true;
    QTest::addRow(     "-1") << QStringLiteral(   "-1") << 0 << true;
    QTest::addRow(      "0") << QStringLiteral(    "0") << 0 << true;
    QTest::addRow("<empty>") << QString()               << 0 << true;
    QTest::addRow(    "foo") << QStringLiteral(  "foo") << 0 << true;
    QTest::addRow(    "bar") << QStringLiteral(  "bar") << 0 << true;
}

void TestAbstractCommand::processOptions_timeout()
{
    QFETCH(QString, argument);
    QFETCH(int, expected);
    QFETCH(bool, expectErrors);

    QCommandLineParser parser;
    QVERIFY(parser.addOptions({
        { QStringLiteral("device"),       QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("mockRequired"), QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("output"),       QStringLiteral("desc"), QStringLiteral("value") },
        { QStringLiteral("timeout"),      QStringLiteral("desc"), QStringLiteral("value") },
    }));
    QVERIFY(parser.parse(QStringList{
        QStringLiteral("executableName"),
        QStringLiteral("--mockRequired=abc123"),
        QStringLiteral("--timeout"), argument
    }));

    MockCommand mock;
    const int defaultTimeout = mock.discoveryAgent->lowEnergyDiscoveryTimeout(); // eg 20s on Linux.
    if (defaultTimeout == -1) {
        QSKIP("This platform does not support timeout for BLE device searches.");
        // See https://doc.qt.io/qt-5/qbluetoothdevicediscoveryagent.html#lowEnergyDiscoveryTimeout
    }
    const QStringList errors = mock.processOptions(parser);
    QCOMPARE(!errors.isEmpty(), expectErrors);
    if (expectErrors) {
        QCOMPARE(mock.discoveryAgent->lowEnergyDiscoveryTimeout(), defaultTimeout);
    } else {
        QCOMPARE(mock.discoveryAgent->lowEnergyDiscoveryTimeout(), expected);
    }
}

QTEST_MAIN(TestAbstractCommand)
