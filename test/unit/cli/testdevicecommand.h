// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestDeviceCommand : public QObject
{
    Q_OBJECT

private slots:
    void start();

    void disconnect();

    void minRange_meter_current_data();
    void minRange_meter_current();

    void minRange_meter_resistance_data();
    void minRange_meter_resistance();

    void minRange_meter_voltage_data();
    void minRange_meter_voltage();

    void minRange_pro_capacitance_data();
    void minRange_pro_capacitance();

    void minRange_pro_current_data();
    void minRange_pro_current();

    void minRange_pro_resistance_data();
    void minRange_pro_resistance();

    void minRange_pro_voltage_data();
    void minRange_pro_voltage();

    void minCapacitanceRange();

    void minCurrentRange();

    void minResistanceRange();

    void minVoltageRange();

    void controllerError();

    void deviceDisconnected();

    void serviceError();

    void serviceDetailsDiscovered();

    void deviceDiscovered();

    void deviceDiscoveryFinished();

    void tr();
};
