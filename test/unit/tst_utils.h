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

class TestUtils : public QObject
{
    Q_OBJECT

private slots:
    void toJson_info_data();
    void toJson_info();

    void toJson_coreConfig_data();
    void toJson_coreConfig();

    void toJson_majorClass_data();
    void toJson_majorClass();

    void toJson_minorClass_data();
    void toJson_minorClass();

    void toJson_serviceClasses_data();
    void toJson_serviceClasses();

    void toJson_uuids_list_data();
    void toJson_uuids_list();

    void toJson_uuids_vector_data();
    void toJson_uuids_vector();

    // QBluetoothDeviceInfo::manufacturerData added in Qt 5.12.
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    void toJson_manufacturerData_data();
    void toJson_manufacturerData();
    #endif

    void toString_majorClass_data();
    void toString_majorClass();

    void toString_minorClass_data();
    void toString_minorClass();
};
