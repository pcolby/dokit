// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestScanCommand : public QObject
{
    Q_OBJECT

private slots:
    void requiredOptions();

    void supportedOptions();

    void processOptions();

    void start();

    void deviceDiscovered();

    void deviceUpdated();

    void deviceDiscoveryFinished();

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
