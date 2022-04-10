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
 * Defines some simple general purpose, utility functions used by QtPokit.
 */

#include <qtpokit/utils.h>

#include "logging_p.h"

#include <QBluetoothAddress>
#include <QDebug>

QTPOKIT_BEGIN_NAMESPACE

/*!
 * Returns \a info as a JSON object.
 */
QJsonObject toJson(const QBluetoothDeviceInfo &info) {
    if (!info.isValid()) {
        return QJsonObject();
    }
    QJsonObject json{
        { QLatin1String("address"), info.address().toString() },
        { QLatin1String("name"), info.name() },
        { QLatin1String("isCached"), info.isCached() },
        { QLatin1String("majorDeviceClass"), info.majorDeviceClass() },
        { QLatin1String("majorDeviceClass"), toJson(info.majorDeviceClass()) },
        { QLatin1String("minorDeviceClass"), toJson(info.majorDeviceClass(), info.minorDeviceClass()) },
        { QLatin1String("signalStrength"), info.rssi() },
    };
    if (info.coreConfigurations() != QBluetoothDeviceInfo::UnknownCoreConfiguration) {
        json.insert(QLatin1String("coreConfiguration"), toJson(info.coreConfigurations()));
    }
    if (!info.deviceUuid().isNull()) {
        json.insert(QLatin1String("deviceUuid"), info.deviceUuid().toString());
    }
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Added in Qt 5.12.
    if (!info.manufacturerData().isEmpty()) {
        json.insert(QLatin1String("manufacturerData"), toJson(info.manufacturerData()));
    }
    #endif
    if (info.serviceClasses() != QBluetoothDeviceInfo::NoService) {
        json.insert(QLatin1String("serviceClasses"), toJson(info.serviceClasses()));
    }
    if (!info.serviceUuids().isEmpty()) {
        json.insert(QLatin1String("serviceUuids"), toJson(info.serviceUuids()));
    }
    return json;
}

/*!
 * Returns \a configuration as a JSON array of strings.
 */
QJsonArray  toJson(const QBluetoothDeviceInfo::CoreConfigurations &configurations)
{
    QJsonArray array;
    #define QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(flag) \
        if (configurations.testFlag(QBluetoothDeviceInfo::flag)) \
            array.append(QLatin1String(#flag))
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(UnknownCoreConfiguration);
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(LowEnergyCoreConfiguration);
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(BaseRateCoreConfiguration);
  //QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(BaseRateAndLowEnergyCoreConfiguration); // Combination flag.
    #undef QTPOKIT_INTERNAL_IF_SET_THEN_APPEND
    return array;
}

/*!
 * Returns \a majorClass as a JSON value. This is equivalent to toString, except that if toString
 * does not recognise \a majorClass, then \a majorClass is returned as a JSON number (not a string).
 *
 * \see toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
 */
QJsonValue toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
{
    const QString string = toString(majorClass);
    return (string.isNull() ? QJsonValue(majorClass) : QJsonValue(string));
}

/*!
 * Returns \a minorClass as a JSON value. This is equivalent to toString, except that if toString
 * does not recognise \a minorClass as a sub-class of \a majorClass, then \a minorClass is returned
 * as a JSON number (not a string).
 *
 * \see toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass)
 */
QJsonValue toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass)
{
    const QString string = toString(majorClass, minorClass);
    return (string.isNull() ? QJsonValue(minorClass) : QJsonValue(string));
}

/*!
 * Returns \a classes as a JSON array of strings.
 */
QJsonArray toJson(const QBluetoothDeviceInfo::ServiceClasses &classes)
{
    QJsonArray array;
    #define QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(flag) \
        if (classes.testFlag(QBluetoothDeviceInfo::flag)) \
            array.append(QLatin1String(#flag))
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(PositioningService);
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(NetworkingService);
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(RenderingService);
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(CapturingService);
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(ObjectTransferService);
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(AudioService);
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(TelephonyService);
    QTPOKIT_INTERNAL_IF_SET_THEN_APPEND(InformationService);
    #undef QTPOKIT_INTERNAL_IF_SET_THEN_APPEND
    return array;
}

/*!
 * Returns \a uuids as a JSON array.
 */
QJsonArray toJson(const QList<QBluetoothUuid> &uuids)
{
    QJsonArray array;
    for (const QBluetoothUuid &uuid: uuids) {
        array.append(uuid.toString());
    }
    return array;
}

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)) // As of Qt6, QVector is an alias for QList.
/*!
 * Returns \a uuids as a JSON array.
 */
QJsonArray toJson(const QVector<QBluetoothUuid> &uuids)
{
    QJsonArray array;
    for (const QBluetoothUuid &uuid: uuids) {
        array.append(uuid.toString());
    }
    return array;
}
#endif

/*!
 * Returns Bluetooth manufacturer \a data as a JSON object that maps the manufacturer IDs (unsigned
 * integers as strings) to arrays of one or more values.
 */
QJsonObject toJson(const QMultiHash<quint16, QByteArray> &data)
{
    QJsonObject object;
    QList<quint16> keys = data.uniqueKeys();
    std::sort(keys.begin(), keys.end());
    for (const quint16 key: keys) {
        // Convert the key's values to a JSON array, reversing the order, because QMultiHash
        // guarantees that the values are orderer "from the most recently inserted to the least
        // recently inserted", which is the oppoosit of what we want.
        QList<QByteArray> values = data.values(key);
        std::reverse(values.begin(), values.end());
        QJsonArray array;
        for (const QByteArray &value: values) {
            array.append(QLatin1String(value.toBase64()));
        }
        object.insert(QString::number(key), array);
    }
    return object;
}

/*!
 * Returns \a majorClass as a human-readable string, or a null QString if \a majorClass is not
 * recognised.
 *
 * For example, if \a majorClass is \c QBluetoothDeviceInfo::ToyDevice, then the string `ToyDevice`
 * is returned.
 */
QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
{
    #define QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(value) \
        if (majorClass == QBluetoothDeviceInfo::value) \
            return QLatin1String(#value)
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(MiscellaneousDevice);
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ComputerDevice);
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(PhoneDevice);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(LANAccessDevice); // Deprecated since Qt 5.13.
    #else
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkDevice); // Added in Qt 5.13.
    #endif
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(AudioVideoDevice);
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(PeripheralDevice);
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImagingDevice);
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableDevice);
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyDevice);
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthDevice);
    QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedDevice);
    #undef QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN
    qCDebug(pokitUtils) << "unknown major class" << majorClass;
    return QString(); // Null QString indicates unknown minor class.
}

/*!
 * Returns \a minorClass as a human-readable string, or a null QString if \a minorClass is not
 * recognised as a sub-class of \a majorClass.
 *
 * For example, if \a majorClass is \c QBluetoothDeviceInfo::ToyDevice, and \a minorClass is
 * \c QBluetoothDeviceInfo::ToyRobot, then the string `ToyRobot` is returned.
 */
QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass)
{
    #define QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(value) \
        if (minorClass == QBluetoothDeviceInfo::value) \
            return QLatin1String(#value)
    switch (majorClass) {
    case QBluetoothDeviceInfo::MiscellaneousDevice:
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedMiscellaneous);
        break;
    case QBluetoothDeviceInfo::ComputerDevice:
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedComputer);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(DesktopComputer);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ServerComputer);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(LaptopComputer);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HandheldClamShellComputer);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HandheldComputer);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableComputer);
        break;
    case QBluetoothDeviceInfo::PhoneDevice:
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedPhone);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CellularPhone);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CordlessPhone);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(SmartPhone);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WiredModemOrVoiceGatewayPhone);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CommonIsdnAccessPhone);
        break;
    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    case QBluetoothDeviceInfo::LANAccessDevice: // Deprecated since Qt 5.13.
    #else
    case QBluetoothDeviceInfo::NetworkDevice: // Added in Qt 5.13.
    #endif
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkFullService);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorOne);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorTwo);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorThree);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorFour);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorFive);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorSix);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkNoService);
        break;
    case QBluetoothDeviceInfo::AudioVideoDevice:
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedAudioVideoDevice);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableHeadsetDevice);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HandsFreeDevice);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Microphone);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Loudspeaker);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Headphones);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(PortableAudioDevice);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CarAudio);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(SetTopBox);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HiFiAudioDevice);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Vcr);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(VideoCamera);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Camcorder);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(VideoMonitor);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(VideoDisplayAndLoudspeaker);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(VideoConferencing);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(GamingDevice);
        break;
    case QBluetoothDeviceInfo::PeripheralDevice:
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedPeripheral);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(KeyboardPeripheral);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(PointingDevicePeripheral);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(KeyboardWithPointingDevicePeripheral);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(JoystickPeripheral);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(GamepadPeripheral);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(RemoteControlPeripheral);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(SensingDevicePeripheral);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(DigitizerTabletPeripheral);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CardReaderPeripheral);
        break;
    case QBluetoothDeviceInfo::ImagingDevice:
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedImagingDevice);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImageDisplay);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImageCamera);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImageScanner);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImagePrinter);
        break;
    case QBluetoothDeviceInfo::WearableDevice:
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedWearableDevice);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableWristWatch);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearablePager);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableJacket);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableHelmet);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableGlasses);
        break;
    case QBluetoothDeviceInfo::ToyDevice:
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedToy);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyRobot);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyVehicle);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyDoll);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyController);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyGame);
        break;
    case QBluetoothDeviceInfo::HealthDevice:
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedHealthDevice);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthBloodPressureMonitor);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthThermometer);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthWeightScale);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthGlucoseMeter);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthPulseOximeter);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthDataDisplay);
        QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthStepCounter);
        break;
    case QBluetoothDeviceInfo::UncategorizedDevice:
        // There are no minor classes defined (in Qt) for uncategorized devices.
        break;
    }
    #undef QTPOKIT_INTERNAL_IF_EQUAL_THEN_RETURN
    qCDebug(pokitUtils) << "unknown minor class" << minorClass << "for major class" << majorClass;
    return QString(); // Null QString indicates unknown minor class.
}

QTPOKIT_END_NAMESPACE
