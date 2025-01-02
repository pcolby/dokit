// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestPokitMeter : public QObject
{
    Q_OBJECT

private slots:
    void toString_Current_data();
    void toString_Current();

    void maxValue_Current_data();
    void maxValue_Current();

    void toString_Resistance_data();
    void toString_Resistance();

    void maxValue_Resistance_data();
    void maxValue_Resistance();

    void toString_Voltage_data();
    void toString_Voltage();

    void maxValue_Voltage_data();
    void maxValue_Voltage();

    void operatorPlus_Current_data();
    void operatorPlus_Current();

    void operatorPlus_Resistance_data();
    void operatorPlus_Resistance();

    void operatorPlus_Voltage_data();
    void operatorPlus_Voltage();
};
