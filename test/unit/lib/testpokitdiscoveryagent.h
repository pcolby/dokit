// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <qtpokit/qtpokit_global.h>

#include <QTest>

QTPOKIT_BEGIN_NAMESPACE

class TestPokitDiscoveryAgent : public QObject
{
    Q_OBJECT

private slots:
    //void start(); // Cannot test without impacting Bluetooth devices.

    void cancelled();

    void deviceDiscovered_data();
    void deviceDiscovered();

    void deviceUpdated_data();
    void deviceUpdated();

    void error();

    void finished();

    void tr();
};

QTPOKIT_END_NAMESPACE
