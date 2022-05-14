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

void TestAbstractCommand::processOptions_data()
{
    QTest::addColumn<int>("input");
    QTest::addColumn<int>("expected");

    QTest::addRow("example") << 1 << 2;
}

void TestAbstractCommand::processOptions()
{
    QFETCH(int, input);
    QFETCH(int, expected);
    const int actual = input * 2;
    QCOMPARE(actual, expected);
}

QTEST_MAIN(TestAbstractCommand)
