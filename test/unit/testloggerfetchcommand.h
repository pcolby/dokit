// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestLoggerFetchCommand : public QObject
{
    Q_OBJECT

private slots:
    void getService();

    void serviceDetailsDiscovered();

    void metadataRead();

    void outputSamples();

    void tr();
};
