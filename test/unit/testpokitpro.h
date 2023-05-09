// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestPokitPro : public QObject
{
    Q_OBJECT

private slots:
    void toString_Capacitance_data();
    void toString_Capacitance();

    void maxValue_Capacitance_data();
    void maxValue_Capacitance();

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
};
