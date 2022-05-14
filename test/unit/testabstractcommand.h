/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QTest>

class TestAbstractCommand : public QObject
{
    Q_OBJECT

private slots:
    void requiredOptions();

    void supportedOptions();

    void escapeCsvField_data();
    void escapeCsvField();

    void parseMicroValue_data();
    void parseMicroValue();

    void parseMilliValue_data();
    void parseMilliValue();

    void parseWholeValue_data();
    void parseWholeValue();

    void processOptions();

    void processOptions_device_data();
    void processOptions_device();

    void processOptions_output_data();
    void processOptions_output();

    void processOptions_timeout_data();
    void processOptions_timeout();
};
