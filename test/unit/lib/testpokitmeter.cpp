// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testpokitmeter.h"
#include "../stringliterals_p.h"

#include <qtpokit/pokitmeter.h>

Q_DECLARE_METATYPE(QTPOKIT_PREPEND_NAMESPACE(PokitMeter::CurrentRange))
Q_DECLARE_METATYPE(QTPOKIT_PREPEND_NAMESPACE(PokitMeter::ResistanceRange))
Q_DECLARE_METATYPE(QTPOKIT_PREPEND_NAMESPACE(PokitMeter::VoltageRange))

QTPOKIT_BEGIN_NAMESPACE
DOKIT_USE_STRINGLITERALS

void TestPokitMeter::toString_Current_data()
{
    QTest::addColumn<PokitMeter::CurrentRange>("range");
    QTest::addColumn<QString>("expected");

    QTest::addRow("10mA")  << PokitMeter::CurrentRange::_10mA     << u"Up to 10mA"_s;
    QTest::addRow("30mA")  << PokitMeter::CurrentRange::_30mA     << u"Up to 30mA"_s;
    QTest::addRow("150mA") << PokitMeter::CurrentRange::_150mA    << u"Up to 150mA"_s;
    QTest::addRow("300mA") << PokitMeter::CurrentRange::_300mA    << u"Up to 300mA"_s;
    QTest::addRow("2A")    << PokitMeter::CurrentRange::_2A       << u"Up to 2A"_s;
    QTest::addRow("auto")  << PokitMeter::CurrentRange::AutoRange << u"Auto-range"_s;
}

void TestPokitMeter::toString_Current()
{
    QFETCH(PokitMeter::CurrentRange, range);
    QFETCH(QString, expected);
    QCOMPARE(toString(range), expected);
}

void TestPokitMeter::maxValue_Current_data()
{
    QTest::addColumn<PokitMeter::CurrentRange>("range");
    QTest::addColumn<quint32>("expected");

    QTest::addRow("10mA")  << PokitMeter::CurrentRange::_10mA     << (quint32)   10'000;
    QTest::addRow("30mA")  << PokitMeter::CurrentRange::_30mA     << (quint32)   30'000;
    QTest::addRow("150mA") << PokitMeter::CurrentRange::_150mA    << (quint32)  150'000;
    QTest::addRow("300ma") << PokitMeter::CurrentRange::_300mA    << (quint32)  300'000;
    QTest::addRow("2A")    << PokitMeter::CurrentRange::_2A       << (quint32)2'000'000;
    QTest::addRow("auto")  << PokitMeter::CurrentRange::AutoRange << (quint32)2'000'000;
}

void TestPokitMeter::maxValue_Current()
{
    QFETCH(PokitMeter::CurrentRange, range);
    QFETCH(quint32, expected);
    QCOMPARE(maxValue(range), expected);
}

void TestPokitMeter::toString_Resistance_data()
{
    QTest::addColumn<PokitMeter::ResistanceRange>("range");
    QTest::addColumn<QString>("expected");

    QTest::addRow("160Ω")  << PokitMeter::ResistanceRange::_160      << QString::fromUtf8("Up to 160Ω");
    QTest::addRow("330Ω")  << PokitMeter::ResistanceRange::_330      << QString::fromUtf8("Up to 330Ω");
    QTest::addRow("890Ω")  << PokitMeter::ResistanceRange::_890      << QString::fromUtf8("Up to 890Ω");
    QTest::addRow("1.5KΩ") << PokitMeter::ResistanceRange::_1K5      << QString::fromUtf8("Up to 1.5KΩ");
    QTest::addRow("10KΩ")  << PokitMeter::ResistanceRange::_10K      << QString::fromUtf8("Up to 10KΩ");
    QTest::addRow("100KΩ") << PokitMeter::ResistanceRange::_100K     << QString::fromUtf8("Up to 100KΩ");
    QTest::addRow("470KΩ") << PokitMeter::ResistanceRange::_470K     << QString::fromUtf8("Up to 470KΩ");
    QTest::addRow("1MΩ")   << PokitMeter::ResistanceRange::_1M       << QString::fromUtf8("Up to 1MΩ");
    QTest::addRow("auto")  << PokitMeter::ResistanceRange::AutoRange << u"Auto-range"_s;
}

void TestPokitMeter::toString_Resistance()
{
    QFETCH(PokitMeter::ResistanceRange, range);
    QFETCH(QString, expected);
    QCOMPARE(toString(range), expected);
}

void TestPokitMeter::maxValue_Resistance_data()
{
    QTest::addColumn<PokitMeter::ResistanceRange>("range");
    QTest::addColumn<quint32>("expected");

    QTest::addRow("160Ω")  << PokitMeter::ResistanceRange::_160      << (quint32)      160;
    QTest::addRow("330Ω")  << PokitMeter::ResistanceRange::_330      << (quint32)      330;
    QTest::addRow("890Ω")  << PokitMeter::ResistanceRange::_890      << (quint32)      890;
    QTest::addRow("1.5KΩ") << PokitMeter::ResistanceRange::_1K5      << (quint32)    1'500;
    QTest::addRow("10KΩ")  << PokitMeter::ResistanceRange::_10K      << (quint32)   10'000;
    QTest::addRow("100KΩ") << PokitMeter::ResistanceRange::_100K     << (quint32)  100'000;
    QTest::addRow("470KΩ") << PokitMeter::ResistanceRange::_470K     << (quint32)  470'000;
    QTest::addRow("1MΩ")   << PokitMeter::ResistanceRange::_1M       << (quint32)1'000'000;
    QTest::addRow("auto")  << PokitMeter::ResistanceRange::AutoRange << (quint32)1'000'000;
}

void TestPokitMeter::maxValue_Resistance()
{
    QFETCH(PokitMeter::ResistanceRange, range);
    QFETCH(quint32, expected);
    QCOMPARE(maxValue(range), expected);
}

void TestPokitMeter::toString_Voltage_data()
{
    QTest::addColumn<PokitMeter::VoltageRange>("range");
    QTest::addColumn<QString>("expected");

    QTest::addRow("300mV") << PokitMeter::VoltageRange::_300mV    << u"Up to 300mV"_s;
    QTest::addRow("2V")    << PokitMeter::VoltageRange::_2V       << u"Up to 2V"_s;
    QTest::addRow("6V")    << PokitMeter::VoltageRange::_6V       << u"Up to 6V"_s;
    QTest::addRow("12V")   << PokitMeter::VoltageRange::_12V      << u"Up to 12V"_s;
    QTest::addRow("30V")   << PokitMeter::VoltageRange::_30V      << u"Up to 30V"_s;
    QTest::addRow("60V")   << PokitMeter::VoltageRange::_60V      << u"Up to 60V"_s;
    QTest::addRow("auto")  << PokitMeter::VoltageRange::AutoRange << u"Auto-range"_s;
}

void TestPokitMeter::toString_Voltage()
{
    QFETCH(PokitMeter::VoltageRange, range);
    QFETCH(QString, expected);
    QCOMPARE(toString(range), expected);
}

void TestPokitMeter::maxValue_Voltage_data()
{
    QTest::addColumn<PokitMeter::VoltageRange>("range");
    QTest::addColumn<quint32>("expected");

    QTest::addRow("300mV") << PokitMeter::VoltageRange::_300mV    << (quint32)   300;
    QTest::addRow("2V")    << PokitMeter::VoltageRange::_2V       << (quint32) 2'000;
    QTest::addRow("6V")    << PokitMeter::VoltageRange::_6V       << (quint32) 6'000;
    QTest::addRow("12V")   << PokitMeter::VoltageRange::_12V      << (quint32)12'000;
    QTest::addRow("30V")   << PokitMeter::VoltageRange::_30V      << (quint32)30'000;
    QTest::addRow("60V")   << PokitMeter::VoltageRange::_60V      << (quint32)60'000;
    QTest::addRow("auto")  << PokitMeter::VoltageRange::AutoRange << (quint32)60'000;
}

void TestPokitMeter::maxValue_Voltage()
{
    QFETCH(PokitMeter::VoltageRange, range);
    QFETCH(quint32, expected);
    QCOMPARE(maxValue(range), expected);
}

void TestPokitMeter::operatorPlus_Current_data()
{
    maxValue_Current_data(); // Either of the *_Current_data() providers would do.
}

void TestPokitMeter::operatorPlus_Current()
{
    QFETCH(PokitMeter::CurrentRange, range);
    QCOMPARE(+range, static_cast<quint8>(range));
}

void TestPokitMeter::operatorPlus_Resistance_data()
{
    maxValue_Resistance_data(); // Either of the *_Resistance_data() providers would do.
}

void TestPokitMeter::operatorPlus_Resistance()
{
    QFETCH(PokitMeter::ResistanceRange, range);
    QCOMPARE(+range, static_cast<quint8>(range));
}

void TestPokitMeter::operatorPlus_Voltage_data()
{
    maxValue_Voltage_data(); // Either of the *_Voltage_data() providers would do.
}

void TestPokitMeter::operatorPlus_Voltage()
{
    QFETCH(PokitMeter::VoltageRange, range);
    QCOMPARE(+range, static_cast<quint8>(range));
}

QTPOKIT_END_NAMESPACE

QTEST_MAIN(QTPOKIT_PREPEND_NAMESPACE(TestPokitMeter))
