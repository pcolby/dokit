// SPDX-FileCopyrightText: 2022-2026 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestAbstractCommand : public QObject
{
    Q_OBJECT

private slots:
    void requiredOptions();

    void supportedOptions();

    void appendSiPrefix_data();
    void appendSiPrefix();

    void escapeCsvField_data();
    void escapeCsvField();

    void parseMicroValue_data();
    void parseMicroValue();

    void parseMilliValue_data();
    void parseMilliValue();

    void parseWholeValue_data();
    void parseWholeValue();

    void parseWholeValue_float_data();
    void parseWholeValue_float();

    void processOptions();

    void processOptions_device_data();
    void processOptions_device();

    void processOptions_output_data();
    void processOptions_output();

    void processOptions_timeout_data();
    void processOptions_timeout();

    void tr();
};
