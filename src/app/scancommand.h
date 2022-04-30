/*
    Copyright 2022 Paul Colby

    This file is part of QtPokit.

    QtPokit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QtPokit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QtPokit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "abstractcommand.h"

class ScanCommand : public AbstractCommand
{
public:
    explicit ScanCommand(QObject * const parent);

    QStringList requiredOptions() const override;
    QStringList supportedOptions() const override;

public slots:
    QStringList processOptions(const QCommandLineParser &parser) override;
    bool start() override;

protected slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &info) override;
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    void deviceUpdated(const QBluetoothDeviceInfo &info,
        const QBluetoothDeviceInfo::Fields updatedFields);
    #endif
    void deviceDiscoveryFinished() override;

private:
    static QJsonObject toJson(const QBluetoothDeviceInfo &info);
    static QJsonArray  toJson(const QBluetoothDeviceInfo::CoreConfigurations &configurations);
    static QJsonValue  toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass);
    static QJsonValue  toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass);
    static QJsonArray  toJson(const QBluetoothDeviceInfo::ServiceClasses &classes);
    static QJsonArray  toJson(const QList<QBluetoothUuid> &uuids);
    static QJsonObject toJson(const QMultiHash<quint16, QByteArray> &data);

    static QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass);
    static QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass);

    friend class TestScanCommand;
};
