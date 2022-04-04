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

#include <QBluetoothDeviceDiscoveryAgent>

#include "qtpokit_global.h"

QTPOKIT_BEGIN_NAMESPACE

class PokitDeviceDiscoveryAgent : public QBluetoothDeviceDiscoveryAgent
{
    Q_OBJECT

public:
    explicit PokitDeviceDiscoveryAgent(const QBluetoothAddress &deviceAdapter, QObject *parent=nullptr);
    PokitDeviceDiscoveryAgent(QObject * parent=nullptr);

    static bool isPokitDevice(const QBluetoothDeviceInfo &info);

public slots:
    void start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethods methods);
    void start();

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &info);
    void onDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);

signals:
    void pokitDeviceDiscovered(const QBluetoothDeviceInfo &info);
    void pokitDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
};

QTPOKIT_END_NAMESPACE
