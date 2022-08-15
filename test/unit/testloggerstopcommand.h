// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestLoggerStopCommand : public QObject
{
    Q_OBJECT

private slots:
     void getService();

     void serviceDetailsDiscovered();

     void settingsWritten();
};
