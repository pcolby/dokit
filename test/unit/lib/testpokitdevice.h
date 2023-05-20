// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestPokitDevice : public QObject
{
    Q_OBJECT

private slots:
    void controller();
    void controller_const();

    void calibration();
    void dataLogger();
    void deviceInformation();
    void dso();
    void genericAccess();
    void multimeter();
    void status();

    void serviceToString_data();
    void serviceToString();

    void charcteristicToString_data();
    void charcteristicToString();

    void setController();

    void connected();
    void connectionUpdated();
    void disconnected();
    void discoveryFinished();
    void errorOccurred();
    void serviceDiscovered();
    void stateChanged();

    void tr();
};
