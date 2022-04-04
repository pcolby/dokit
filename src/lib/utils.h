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

/*!
 * \file
 * Simple, general purpose, utility functions.
 */

#ifndef QTPOKIT_UTILS_H
#define QTPOKIT_UTILS_H

#include "qtpokit_global.h"

#include "logging.h"

#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

QTPOKIT_BEGIN_NAMESPACE

inline QJsonArray toJsonArray(const QBluetoothDeviceInfo::ServiceClasses &classes)
{
    QJsonArray array;
    #define QTPOKIT_IF_SET_THEN_APPEND(flag) \
        if (classes.testFlag(QBluetoothDeviceInfo::flag)) \
            array.append(QLatin1String(#flag))
    QTPOKIT_IF_SET_THEN_APPEND(PositioningService);
    QTPOKIT_IF_SET_THEN_APPEND(NetworkingService);
    QTPOKIT_IF_SET_THEN_APPEND(RenderingService);
    QTPOKIT_IF_SET_THEN_APPEND(CapturingService);
    QTPOKIT_IF_SET_THEN_APPEND(ObjectTransferService);
    QTPOKIT_IF_SET_THEN_APPEND(AudioService);
    QTPOKIT_IF_SET_THEN_APPEND(TelephonyService);
    QTPOKIT_IF_SET_THEN_APPEND(InformationService);
    #undef QTPOKIT_IF_SET_THEN_APPEND
    return array;
}

inline QJsonArray toJsonArray(const QList<QBluetoothUuid> &uuids)
{
    QJsonArray array;
    for (const QBluetoothUuid &uuid: uuids) {
        array.append(uuid.toString());
    }
    return array;
}

inline QJsonArray toJsonArray(const QMultiHash<quint16, QByteArray> &data)
{
    QJsonArray array;
    for (auto iter = data.cbegin(); iter != data.cend(); ++iter) {
        array.append(QJsonObject{
            { QLatin1String("manufacturerId"), iter.key() },
            { QLatin1String("data"), QString::fromLatin1(iter.value().toBase64()) },
        });
    }
    return array;
}

inline QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
{
    #define QTPOKIT_IF_EQUAL_THEN_RETURN(value) \
        if (majorClass == QBluetoothDeviceInfo::value) \
            return QLatin1String(#value)
    QTPOKIT_IF_EQUAL_THEN_RETURN(MiscellaneousDevice);
    QTPOKIT_IF_EQUAL_THEN_RETURN(ComputerDevice);
    QTPOKIT_IF_EQUAL_THEN_RETURN(PhoneDevice);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    QTPOKIT_IF_EQUAL_THEN_RETURN(LANAccessDevice): // Deprecated since Qt 5.13.
    #else
    QTPOKIT_IF_EQUAL_THEN_RETURN(NetworkDevice); // Added in Qt 5.13.
    #endif
    QTPOKIT_IF_EQUAL_THEN_RETURN(AudioVideoDevice);
    QTPOKIT_IF_EQUAL_THEN_RETURN(PeripheralDevice);
    QTPOKIT_IF_EQUAL_THEN_RETURN(ImagingDevice);
    QTPOKIT_IF_EQUAL_THEN_RETURN(WearableDevice);
    QTPOKIT_IF_EQUAL_THEN_RETURN(ToyDevice);
    QTPOKIT_IF_EQUAL_THEN_RETURN(HealthDevice);
    QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedDevice);
    #undef QTPOKIT_IF_EQUAL_THEN_RETURN
    qCDebug(pokitUtils) << "unknown major class" << majorClass;
    return QString(); // Null QString indicates unknown minor class.
}

inline QJsonValue toJsonValue(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
{
    const QString string = toString(majorClass);
    return (string.isNull() ? QJsonValue(majorClass) : QJsonValue(string));
}

inline QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass)
{
    #define QTPOKIT_IF_EQUAL_THEN_RETURN(value) \
        if (minorClass == QBluetoothDeviceInfo::value) \
            return QLatin1String(#value)
    switch (majorClass) {
    case QBluetoothDeviceInfo::MiscellaneousDevice:
        QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedMiscellaneous);
        break;
    case QBluetoothDeviceInfo::ComputerDevice:
        QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedComputer);
        QTPOKIT_IF_EQUAL_THEN_RETURN(DesktopComputer);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ServerComputer);
        QTPOKIT_IF_EQUAL_THEN_RETURN(LaptopComputer);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HandheldClamShellComputer);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HandheldComputer);
        QTPOKIT_IF_EQUAL_THEN_RETURN(WearableComputer);
        break;
    case QBluetoothDeviceInfo::PhoneDevice:
        QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedPhone);
        QTPOKIT_IF_EQUAL_THEN_RETURN(CellularPhone);
        QTPOKIT_IF_EQUAL_THEN_RETURN(CordlessPhone);
        QTPOKIT_IF_EQUAL_THEN_RETURN(SmartPhone);
        QTPOKIT_IF_EQUAL_THEN_RETURN(WiredModemOrVoiceGatewayPhone);
        QTPOKIT_IF_EQUAL_THEN_RETURN(CommonIsdnAccessPhone);
        break;
    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    case QBluetoothDeviceInfo::LANAccessDevice: // Deprecated since Qt 5.13.
    #else
    case QBluetoothDeviceInfo::NetworkDevice: // Added in Qt 5.13.
    #endif
        QTPOKIT_IF_EQUAL_THEN_RETURN(NetworkFullService);
        QTPOKIT_IF_EQUAL_THEN_RETURN(NetworkLoadFactorOne);
        QTPOKIT_IF_EQUAL_THEN_RETURN(NetworkLoadFactorTwo);
        QTPOKIT_IF_EQUAL_THEN_RETURN(NetworkLoadFactorThree);
        QTPOKIT_IF_EQUAL_THEN_RETURN(NetworkLoadFactorFour);
        QTPOKIT_IF_EQUAL_THEN_RETURN(NetworkLoadFactorFive);
        QTPOKIT_IF_EQUAL_THEN_RETURN(NetworkLoadFactorSix);
        QTPOKIT_IF_EQUAL_THEN_RETURN(NetworkNoService);
        break;
    case QBluetoothDeviceInfo::AudioVideoDevice:
        QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedAudioVideoDevice);
        QTPOKIT_IF_EQUAL_THEN_RETURN(WearableHeadsetDevice);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HandsFreeDevice);
        QTPOKIT_IF_EQUAL_THEN_RETURN(Microphone);
        QTPOKIT_IF_EQUAL_THEN_RETURN(Loudspeaker);
        QTPOKIT_IF_EQUAL_THEN_RETURN(Headphones);
        QTPOKIT_IF_EQUAL_THEN_RETURN(PortableAudioDevice);
        QTPOKIT_IF_EQUAL_THEN_RETURN(CarAudio);
        QTPOKIT_IF_EQUAL_THEN_RETURN(SetTopBox);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HiFiAudioDevice);
        QTPOKIT_IF_EQUAL_THEN_RETURN(Vcr);
        QTPOKIT_IF_EQUAL_THEN_RETURN(VideoCamera);
        QTPOKIT_IF_EQUAL_THEN_RETURN(Camcorder);
        QTPOKIT_IF_EQUAL_THEN_RETURN(VideoMonitor);
        QTPOKIT_IF_EQUAL_THEN_RETURN(VideoDisplayAndLoudspeaker);
        QTPOKIT_IF_EQUAL_THEN_RETURN(VideoConferencing);
        QTPOKIT_IF_EQUAL_THEN_RETURN(GamingDevice);
        break;
    case QBluetoothDeviceInfo::PeripheralDevice:
        QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedPeripheral);
        QTPOKIT_IF_EQUAL_THEN_RETURN(KeyboardPeripheral);
        QTPOKIT_IF_EQUAL_THEN_RETURN(PointingDevicePeripheral);
        QTPOKIT_IF_EQUAL_THEN_RETURN(KeyboardWithPointingDevicePeripheral);
        QTPOKIT_IF_EQUAL_THEN_RETURN(JoystickPeripheral);
        QTPOKIT_IF_EQUAL_THEN_RETURN(GamepadPeripheral);
        QTPOKIT_IF_EQUAL_THEN_RETURN(RemoteControlPeripheral);
        QTPOKIT_IF_EQUAL_THEN_RETURN(SensingDevicePeripheral);
        QTPOKIT_IF_EQUAL_THEN_RETURN(DigitizerTabletPeripheral);
        QTPOKIT_IF_EQUAL_THEN_RETURN(CardReaderPeripheral);
        break;
    case QBluetoothDeviceInfo::ImagingDevice:
        QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedImagingDevice);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ImageDisplay);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ImageCamera);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ImageScanner);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ImagePrinter);
        break;
    case QBluetoothDeviceInfo::WearableDevice:
        QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedWearableDevice);
        QTPOKIT_IF_EQUAL_THEN_RETURN(WearableWristWatch);
        QTPOKIT_IF_EQUAL_THEN_RETURN(WearablePager);
        QTPOKIT_IF_EQUAL_THEN_RETURN(WearableJacket);
        QTPOKIT_IF_EQUAL_THEN_RETURN(WearableHelmet);
        QTPOKIT_IF_EQUAL_THEN_RETURN(WearableGlasses);
        break;
    case QBluetoothDeviceInfo::ToyDevice:
        QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedToy);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ToyRobot);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ToyVehicle);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ToyDoll);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ToyController);
        QTPOKIT_IF_EQUAL_THEN_RETURN(ToyGame);
        break;
    case QBluetoothDeviceInfo::HealthDevice:
        QTPOKIT_IF_EQUAL_THEN_RETURN(UncategorizedHealthDevice);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HealthBloodPressureMonitor);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HealthThermometer);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HealthWeightScale);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HealthGlucoseMeter);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HealthPulseOximeter);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HealthDataDisplay);
        QTPOKIT_IF_EQUAL_THEN_RETURN(HealthStepCounter);
        break;
    case QBluetoothDeviceInfo::UncategorizedDevice:
        // There are no minor classes defined (in Qt) for uncategorized devices.
        break;
    }
    #undef QTPOKIT_IF_EQUAL_THEN_RETURN
    qCDebug(pokitUtils) << "unknown minor class" << minorClass << "for major class" << majorClass;
    return QString(); // Null QString indicates unknown minor class.
}

inline QJsonValue toJsonValue(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass)
{
    const QString string = toString(majorClass, minorClass);
    return (string.isNull() ? QJsonValue(minorClass) : QJsonValue(string));
}

inline QJsonObject toJsonObject(const QBluetoothDeviceInfo &info) {
    QJsonObject json{
        { QLatin1String("address"), info.address().toString() },
        { QLatin1String("name"), info.name() },
        { QLatin1String("isCached"), info.isCached() },
        { QLatin1String("majorDeviceClass"), info.majorDeviceClass() },
        { QLatin1String("majorDeviceClass"), toJsonValue(info.majorDeviceClass()) },
        { QLatin1String("minorDeviceClass"), toJsonValue(info.majorDeviceClass(), info.minorDeviceClass()) },
        { QLatin1String("signalStrength"), info.rssi() },
        { QLatin1String("serviceUuids"), toJsonArray(info.serviceUuids()) },

    };
    if (!info.deviceUuid().isNull()) {
        json.insert(QLatin1String("deviceUuid"), info.deviceUuid().toString());
    }
    if (!info.manufacturerData().isEmpty()) {
        json.insert(QLatin1String("manufacturerData"), toJsonArray(info.manufacturerData()));
    }
    if (info.serviceClasses() != QBluetoothDeviceInfo::NoService) {
        json.insert(QLatin1String("serviceClasses"), toJsonArray(info.serviceClasses()));
    }
    return json;
}

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_UTILS_H
