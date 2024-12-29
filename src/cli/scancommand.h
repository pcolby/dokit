// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "abstractcommand.h"

class ScanCommand : public AbstractCommand
{
    Q_OBJECT

public:
    explicit ScanCommand(QObject * const parent = nullptr);

    QStringList requiredOptions(const QCommandLineParser &parser) const override;
    QStringList supportedOptions(const QCommandLineParser &parser) const override;

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
    bool showCsvHeader { true }; ///< Whether or not to show a header as the first line of CSV output.

    static QJsonObject toJson(const QBluetoothDeviceInfo &info);
    static QJsonArray  toJson(const QBluetoothDeviceInfo::CoreConfigurations &configurations);
    static QJsonValue  toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass);
    static QJsonValue  toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass);
    static QJsonArray  toJson(const QBluetoothDeviceInfo::ServiceClasses &classes);
    static QJsonArray  toJson(const QList<QBluetoothUuid> &uuids);
    static QJsonObject toJson(const QMultiHash<quint16, QByteArray> &data);

    static QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass);
    static QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass);

    QTPOKIT_BEFRIEND_TEST(ScanCommand)
};
