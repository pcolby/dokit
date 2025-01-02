// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testpokitpro.h"

#include <qtpokit/pokitpro.h>

Q_DECLARE_METATYPE(PokitPro::CapacitanceRange)
Q_DECLARE_METATYPE(PokitPro::CurrentRange)
Q_DECLARE_METATYPE(PokitPro::ResistanceRange)
Q_DECLARE_METATYPE(PokitPro::VoltageRange)

void TestPokitPro::toString_Capacitance_data()
{
    QTest::addColumn<PokitPro::CapacitanceRange>("range");
    QTest::addColumn<QString>("expected");

    QTest::addRow("100n") << PokitPro::CapacitanceRange::_100nF    << QStringLiteral("Up to 100nF");
    QTest::addRow("10μF") << PokitPro::CapacitanceRange::_10uF     << QString::fromUtf8("Up to 10μF");
    QTest::addRow("1mF")  << PokitPro::CapacitanceRange::_1mF      << QStringLiteral("Up to 1mF");
    QTest::addRow("auto") << PokitPro::CapacitanceRange::AutoRange << QStringLiteral("Auto-range");
}

void TestPokitPro::toString_Capacitance()
{
    QFETCH(PokitPro::CapacitanceRange, range);
    QFETCH(QString, expected);
    QCOMPARE(toString(range), expected);
}

void TestPokitPro::maxValue_Capacitance_data()
{
    QTest::addColumn<PokitPro::CapacitanceRange>("range");
    QTest::addColumn<quint32>("expected");

    QTest::addRow("100n") << PokitPro::CapacitanceRange::_100nF    << (quint32)     100;
    QTest::addRow("10μF") << PokitPro::CapacitanceRange::_10uF     << (quint32)  10'000;
    QTest::addRow("1mF")  << PokitPro::CapacitanceRange::_1mF      << (quint32)1000'000;
    QTest::addRow("auto") << PokitPro::CapacitanceRange::AutoRange << (quint32)1000'000;
}

void TestPokitPro::maxValue_Capacitance()
{
    QFETCH(PokitPro::CapacitanceRange, range);
    QFETCH(quint32, expected);
    QCOMPARE(maxValue(range), expected);
}

void TestPokitPro::toString_Current_data()
{
    QTest::addColumn<PokitPro::CurrentRange>("range");
    QTest::addColumn<QString>("expected");

    QTest::addRow("500μA") << PokitPro::CurrentRange::_500uA    << QString::fromUtf8("Up to 500μA");
    QTest::addRow("2mA")   << PokitPro::CurrentRange::_2mA      << QStringLiteral("Up to 2mA");
    QTest::addRow("10mA")  << PokitPro::CurrentRange::_10mA     << QStringLiteral("Up to 10mA");
    QTest::addRow("125mA") << PokitPro::CurrentRange::_125mA    << QStringLiteral("Up to 125mA");
    QTest::addRow("300mA") << PokitPro::CurrentRange::_300mA    << QStringLiteral("Up to 300mA");
    QTest::addRow("3A")    << PokitPro::CurrentRange::_3A       << QStringLiteral("Up to 3A");
    QTest::addRow("10A")   << PokitPro::CurrentRange::_10A      << QStringLiteral("Up to 10A");
    QTest::addRow("auto")  << PokitPro::CurrentRange::AutoRange << QStringLiteral("Auto-range");
}

void TestPokitPro::toString_Current()
{
    QFETCH(PokitPro::CurrentRange, range);
    QFETCH(QString, expected);
    QCOMPARE(toString(range), expected);
}

void TestPokitPro::maxValue_Current_data()
{
    QTest::addColumn<PokitPro::CurrentRange>("range");
    QTest::addColumn<quint32>("expected");

    QTest::addRow("500μA") << PokitPro::CurrentRange::_500uA    << (quint32)       500;
    QTest::addRow("2mA")   << PokitPro::CurrentRange::_2mA      << (quint32)     2'000;
    QTest::addRow("10mA")  << PokitPro::CurrentRange::_10mA     << (quint32)    10'000;
    QTest::addRow("125mA") << PokitPro::CurrentRange::_125mA    << (quint32)   125'000;
    QTest::addRow("300ma") << PokitPro::CurrentRange::_300mA    << (quint32)   300'000;
    QTest::addRow("3A")    << PokitPro::CurrentRange::_3A       << (quint32) 3'000'000;
    QTest::addRow("10A")   << PokitPro::CurrentRange::_10A      << (quint32)10'000'000;
    QTest::addRow("auto")  << PokitPro::CurrentRange::AutoRange << (quint32)10'000'000;
}

void TestPokitPro::maxValue_Current()
{
    QFETCH(PokitPro::CurrentRange, range);
    QFETCH(quint32, expected);
    QCOMPARE(maxValue(range), expected);
}

void TestPokitPro::toString_Resistance_data()
{
    QTest::addColumn<PokitPro::ResistanceRange>("range");
    QTest::addColumn<QString>("expected");

    QTest::addRow("30Ω")   << PokitPro::ResistanceRange::_30       << QString::fromUtf8("Up to 30Ω");
    QTest::addRow("75Ω")   << PokitPro::ResistanceRange::_75       << QString::fromUtf8("Up to 75Ω");
    QTest::addRow("400Ω")  << PokitPro::ResistanceRange::_400      << QString::fromUtf8("Up to 400Ω");
    QTest::addRow("5KΩ")   << PokitPro::ResistanceRange::_5K       << QString::fromUtf8("Up to 5KΩ");
    QTest::addRow("10KΩ")  << PokitPro::ResistanceRange::_10K      << QString::fromUtf8("Up to 10KΩ");
    QTest::addRow("15KΩ")  << PokitPro::ResistanceRange::_15K      << QString::fromUtf8("Up to 15KΩ");
    QTest::addRow("40KΩ")  << PokitPro::ResistanceRange::_40K      << QString::fromUtf8("Up to 40KΩ");
    QTest::addRow("500KΩ") << PokitPro::ResistanceRange::_500K     << QString::fromUtf8("Up to 500KΩ");
    QTest::addRow("700KΩ") << PokitPro::ResistanceRange::_700K     << QString::fromUtf8("Up to 700KΩ");
    QTest::addRow("1MΩ")   << PokitPro::ResistanceRange::_1M       << QString::fromUtf8("Up to 1MΩ");
    QTest::addRow("3MΩ")   << PokitPro::ResistanceRange::_3M       << QString::fromUtf8("Up to 3MΩ");
    QTest::addRow("auto")  << PokitPro::ResistanceRange::AutoRange << QStringLiteral("Auto-range");
}

void TestPokitPro::toString_Resistance()
{
    QFETCH(PokitPro::ResistanceRange, range);
    QFETCH(QString, expected);
    QCOMPARE(toString(range), expected);
}

void TestPokitPro::maxValue_Resistance_data()
{
    QTest::addColumn<PokitPro::ResistanceRange>("range");
    QTest::addColumn<quint32>("expected");

    QTest::addRow("30Ω")   << PokitPro::ResistanceRange::_30       << (quint32)       30;
    QTest::addRow("75Ω")   << PokitPro::ResistanceRange::_75       << (quint32)       75;
    QTest::addRow("400Ω")  << PokitPro::ResistanceRange::_400      << (quint32)      400;
    QTest::addRow("5KΩ")   << PokitPro::ResistanceRange::_5K       << (quint32)    5'000;
    QTest::addRow("10KΩ")  << PokitPro::ResistanceRange::_10K      << (quint32)   10'000;
    QTest::addRow("15KΩ")  << PokitPro::ResistanceRange::_15K      << (quint32)   15'000;
    QTest::addRow("40KΩ")  << PokitPro::ResistanceRange::_40K      << (quint32)   40'000;
    QTest::addRow("500KΩ") << PokitPro::ResistanceRange::_500K     << (quint32)  500'000;
    QTest::addRow("700KΩ") << PokitPro::ResistanceRange::_700K     << (quint32)  700'000;
    QTest::addRow("1MΩ")   << PokitPro::ResistanceRange::_1M       << (quint32)1'000'000;
    QTest::addRow("3MΩ")   << PokitPro::ResistanceRange::_3M       << (quint32)3'000'000;
    QTest::addRow("auto")  << PokitPro::ResistanceRange::AutoRange << (quint32)3'000'000;
}

void TestPokitPro::maxValue_Resistance()
{
    QFETCH(PokitPro::ResistanceRange, range);
    QFETCH(quint32, expected);
    QCOMPARE(maxValue(range), expected);
}

void TestPokitPro::toString_Voltage_data()
{
    QTest::addColumn<PokitPro::VoltageRange>("range");
    QTest::addColumn<QString>("expected");

    QTest::addRow("250mV") << PokitPro::VoltageRange::_250mV    << QStringLiteral("Up to 250mV");
    QTest::addRow("2V")    << PokitPro::VoltageRange::_2V       << QStringLiteral("Up to 2V");
    QTest::addRow("10V")   << PokitPro::VoltageRange::_10V      << QStringLiteral("Up to 10V");
    QTest::addRow("30V")   << PokitPro::VoltageRange::_30V      << QStringLiteral("Up to 30V");
    QTest::addRow("60V")   << PokitPro::VoltageRange::_60V      << QStringLiteral("Up to 60V");
    QTest::addRow("125V")  << PokitPro::VoltageRange::_125V     << QStringLiteral("Up to 125V");
    QTest::addRow("400V")  << PokitPro::VoltageRange::_400V     << QStringLiteral("Up to 400V");
    QTest::addRow("600V")  << PokitPro::VoltageRange::_600V     << QStringLiteral("Up to 600V");
    QTest::addRow("auto")  << PokitPro::VoltageRange::AutoRange << QStringLiteral("Auto-range");
}

void TestPokitPro::toString_Voltage()
{
    QFETCH(PokitPro::VoltageRange, range);
    QFETCH(QString, expected);
    QCOMPARE(toString(range), expected);
}

void TestPokitPro::maxValue_Voltage_data()
{
    QTest::addColumn<PokitPro::VoltageRange>("range");
    QTest::addColumn<quint32>("expected");

    QTest::addRow("250mV") << PokitPro::VoltageRange::_250mV    << (quint32)    250;
    QTest::addRow("2V")    << PokitPro::VoltageRange::_2V       << (quint32)  2'000;
    QTest::addRow("10V")   << PokitPro::VoltageRange::_10V      << (quint32) 10'000;
    QTest::addRow("30V")   << PokitPro::VoltageRange::_30V      << (quint32) 30'000;
    QTest::addRow("60V")   << PokitPro::VoltageRange::_60V      << (quint32) 60'000;
    QTest::addRow("125V")  << PokitPro::VoltageRange::_125V     << (quint32)125'000;
    QTest::addRow("400V")  << PokitPro::VoltageRange::_400V     << (quint32)400'000;
    QTest::addRow("600V")  << PokitPro::VoltageRange::_600V     << (quint32)600'000;
    QTest::addRow("auto")  << PokitPro::VoltageRange::AutoRange << (quint32)600'000;
}

void TestPokitPro::maxValue_Voltage()
{
    QFETCH(PokitPro::VoltageRange, range);
    QFETCH(quint32, expected);
    QCOMPARE(maxValue(range), expected);
}

void TestPokitPro::operatorPlus_Capacitance_data()
{
    maxValue_Capacitance_data(); // Either of the *_Capacitance_data() providers would do.
}

void TestPokitPro::operatorPlus_Capacitance()
{
    QFETCH(PokitPro::CapacitanceRange, range);
    QCOMPARE(+range, static_cast<quint8>(range));
}

void TestPokitPro::operatorPlus_Current_data()
{
    maxValue_Current_data(); // Either of the *_Current_data() providers would do.
}

void TestPokitPro::operatorPlus_Current()
{
    QFETCH(PokitPro::CurrentRange, range);
    QCOMPARE(+range, static_cast<quint8>(range));
}

void TestPokitPro::operatorPlus_Resistance_data()
{
    maxValue_Resistance_data(); // Either of the *_Resistance_data() providers would do.
}

void TestPokitPro::operatorPlus_Resistance()
{
    QFETCH(PokitPro::ResistanceRange, range);
    QCOMPARE(+range, static_cast<quint8>(range));
}

void TestPokitPro::operatorPlus_Voltage_data()
{
    maxValue_Voltage_data(); // Either of the *_Voltage_data() providers would do.
}

void TestPokitPro::operatorPlus_Voltage()
{
    QFETCH(PokitPro::VoltageRange, range);
    QCOMPARE(+range, static_cast<quint8>(range));
}

QTEST_MAIN(TestPokitPro)
