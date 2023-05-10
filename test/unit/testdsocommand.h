// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestDsoCommand : public QObject
{
    Q_OBJECT

private slots:
    void requiredOptions();

    void supportedOptions();

    void processOptions_data();
    void processOptions();

    void getService();

    void serviceDetailsDiscovered();

//    void lowestRange_data();
//    void lowestRange();

//    void lowestCurrentRange_data();
//    void lowestCurrentRange();

//    void lowestVoltageRange_data();
//    void lowestVoltageRange();

    void settingsWritten();

    void metadataRead();

    void outputSamples_data();
    void outputSamples();

    void tr();
};
