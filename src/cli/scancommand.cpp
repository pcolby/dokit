// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "scancommand.h"
#include "../stringliterals_p.h"

#include <qtpokit/pokitdiscoveryagent.h>

#include <QBluetoothUuid>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

DOKIT_USE_STRINGLITERALS

/*!
 * \class ScanCommand
 *
 * The ScanCommand class implements the `scan` CLI command, by scanning for nearby Pokit Bluetooth
 * devices. When devices are found, they are logged to stdout in the chosen format.
 */

/*!
 * Construct a new ScanCommand object with \a parent.
 */
ScanCommand::ScanCommand(QObject * const parent) : AbstractCommand(parent)
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    connect(discoveryAgent, &PokitDiscoveryAgent::pokitDeviceUpdated,
            this, &ScanCommand::deviceUpdated);
    #endif
}

QStringList ScanCommand::requiredOptions(const QCommandLineParser &parser) const
{
    return AbstractCommand::requiredOptions(parser);
}

QStringList ScanCommand::supportedOptions(const QCommandLineParser &parser) const
{
    return AbstractCommand::supportedOptions(parser) + QStringList{
    };
}

/// \copydoc AbstractCommand::processOptions
QStringList ScanCommand::processOptions(const QCommandLineParser &parser)
{
    QStringList errors = AbstractCommand::processOptions(parser);
    if (!errors.isEmpty()) {
        return errors;
    }

    return errors;
}

/*!
 * Begins scanning for Pokit devices.
 */
bool ScanCommand::start()
{
    Q_ASSERT(discoveryAgent);
    qCInfo(lc).noquote() << tr("Scanning for Pokit devices...");
    discoveryAgent->start();
    return true;
}

/*!
 * Handles discovered Pokit devices, writing \a info to stdout.
 */
void ScanCommand::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    switch (format) {
    case OutputFormat::Csv:
        for (; showCsvHeader; showCsvHeader = false) {
            std::cout << qUtf8Printable(tr("uuid,address,name,major_class,minor_class,signal_strength\n"));
        }
        std::cout << qUtf8Printable(QString::fromLatin1("%1,%2,%3,%4,%5,%6\n").arg(info.deviceUuid().toString(),
            info.address().toString(), escapeCsvField(info.name()), toString(info.majorDeviceClass()),
            toString(info.majorDeviceClass(), info.minorDeviceClass())).arg(info.rssi()));
        break;
    case OutputFormat::Json:
        std::cout << QJsonDocument(toJson(info)).toJson().toStdString();
        break;
    case OutputFormat::Text:
        std::cout << qUtf8Printable(tr("%1 %2 %3 %4\n").arg(info.deviceUuid().toString(),
            info.address().toString(), info.name()).arg(info.rssi()));
        break;
    }
}

/*!
 * Handles updated Pokit devices, writing \a info to stdout. Currently \a updatedFields us unused.
 */
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
void ScanCommand::deviceUpdated(const QBluetoothDeviceInfo &info,
                                const QBluetoothDeviceInfo::Fields updatedFields)
{
    Q_UNUSED(updatedFields)
    deviceDiscovered(info);
}
#endif

/*!
 * Handles the completion of device discovery. In this override we simply exit, as the scan command
 * is nothing more than logging of discovered devices.
 */
void ScanCommand::deviceDiscoveryFinished()
{
    qCDebug(lc).noquote() << tr("Finished scanning for Pokit devices.");
    QCoreApplication::quit();
}

/*!
 * Returns \a info as a JSON object.
 */
QJsonObject ScanCommand::toJson(const QBluetoothDeviceInfo &info)
{
    if (!info.isValid()) {
        return QJsonObject();
    }
    QJsonObject json{
        { u"address"_s, info.address().toString() },
        { u"name"_s, info.name() },
        { u"isCached"_s, info.isCached() },
        { u"majorDeviceClass"_s, info.majorDeviceClass() },
        { u"majorDeviceClass"_s, toJson(info.majorDeviceClass()) },
        { u"minorDeviceClass"_s, toJson(info.majorDeviceClass(), info.minorDeviceClass()) },
        { u"signalStrength"_s, info.rssi() },
    };
    if (info.coreConfigurations() != QBluetoothDeviceInfo::UnknownCoreConfiguration) {
        json.insert(u"coreConfiguration"_s, toJson(info.coreConfigurations()));
    }
    if (!info.deviceUuid().isNull()) {
        json.insert(u"deviceUuid"_s, info.deviceUuid().toString());
    }
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Added in Qt 5.12.
    if (!info.manufacturerData().isEmpty()) {
        json.insert(u"manufacturerData"_s, toJson(info.manufacturerData()));
    }
    #endif
    if (info.serviceClasses() != QBluetoothDeviceInfo::NoService) {
        json.insert(u"serviceClasses"_s, toJson(info.serviceClasses()));
    }
    if (!info.serviceUuids().isEmpty()) {
        json.insert(u"serviceUuids"_s, toJson(info.serviceUuids()));
    }
    return json;
}

/*!
 * Returns \a configuration as a JSON array of strings.
 */
QJsonArray ScanCommand::toJson(const QBluetoothDeviceInfo::CoreConfigurations &configurations)
{
    QJsonArray array;
    #define DOKIT_INTERNAL_IF_SET_THEN_APPEND(flag) \
        if (configurations.testFlag(QBluetoothDeviceInfo::flag)) \
            array.append(QLatin1String(#flag))
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(UnknownCoreConfiguration);
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(LowEnergyCoreConfiguration);
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(BaseRateCoreConfiguration);
  //DOKIT_INTERNAL_IF_SET_THEN_APPEND(BaseRateAndLowEnergyCoreConfiguration); // Combination flag.
    #undef DOKIT_INTERNAL_IF_SET_THEN_APPEND
    return array;
}

/*!
 * Returns \a majorClass as a JSON value. This is equivalent to toString, except that if toString
 * does not recognise \a majorClass, then \a majorClass is returned as a JSON number (not a string).
 *
 * \see toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
 */
QJsonValue ScanCommand::toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
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
QJsonValue ScanCommand::toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass)
{
    const QString string = toString(majorClass, minorClass);
    return (string.isNull() ? QJsonValue(minorClass) : QJsonValue(string));
}

/*!
 * Returns \a classes as a JSON array of strings.
 */
QJsonArray ScanCommand::toJson(const QBluetoothDeviceInfo::ServiceClasses &classes)
{
    QJsonArray array;
    #define DOKIT_INTERNAL_IF_SET_THEN_APPEND(flag) \
        if (classes.testFlag(QBluetoothDeviceInfo::flag)) \
            array.append(QLatin1String(#flag))
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(PositioningService);
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(NetworkingService);
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(RenderingService);
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(CapturingService);
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(ObjectTransferService);
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(AudioService);
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(TelephonyService);
    DOKIT_INTERNAL_IF_SET_THEN_APPEND(InformationService);
    #undef DOKIT_INTERNAL_IF_SET_THEN_APPEND
    return array;
}

/*!
 * Returns \a uuids as a JSON array.
 */
QJsonArray ScanCommand::toJson(const QList<QBluetoothUuid> &uuids)
{
    QJsonArray array;
    for (const QBluetoothUuid &uuid: uuids) {
        array.append(uuid.toString());
    }
    return array;
}

/*!
 * Returns Bluetooth manufacturer \a data as a JSON object that maps the manufacturer IDs (unsigned
 * integers as strings) to arrays of one or more values.
 */
QJsonObject ScanCommand::toJson(const QMultiHash<quint16, QByteArray> &data)
{
    QJsonObject object;
    QList<quint16> keys = data.uniqueKeys();
    std::sort(keys.begin(), keys.end());
    for (const quint16 key: keys) {
        // Convert the key's values to a JSON array, reversing the order, because QMultiHash
        // guarantees that the values are order "from the most recently inserted to the least
        // recently inserted", which is the opposite of what we want.
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
QString ScanCommand::toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
{
    #define DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(value) \
        if (majorClass == QBluetoothDeviceInfo::value) \
            return QLatin1String(#value)
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(MiscellaneousDevice);
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ComputerDevice);
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(PhoneDevice);
    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(LANAccessDevice); // Deprecated since Qt 5.13.
    #else
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkDevice); // Added in Qt 5.13.
    #endif
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(AudioVideoDevice);
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(PeripheralDevice);
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImagingDevice);
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableDevice);
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyDevice);
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthDevice);
    DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedDevice);
    #undef DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN
    qCDebug(lc).noquote() << tr("Unknown major class %1.").arg(majorClass);
    return QString(); // Null QString indicates unknown minor class.
}

/*!
 * Returns \a minorClass as a human-readable string, or a null QString if \a minorClass is not
 * recognised as a sub-class of \a majorClass.
 *
 * For example, if \a majorClass is \c QBluetoothDeviceInfo::ToyDevice, and \a minorClass is
 * \c QBluetoothDeviceInfo::ToyRobot, then the string `ToyRobot` is returned.
 */
QString ScanCommand::toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass)
{
    #define DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(value) \
        if (minorClass == QBluetoothDeviceInfo::value) \
            return QLatin1String(#value)
    switch (majorClass) {
    case QBluetoothDeviceInfo::MiscellaneousDevice:
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedMiscellaneous);
        break;
    case QBluetoothDeviceInfo::ComputerDevice:
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedComputer);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(DesktopComputer);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ServerComputer);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(LaptopComputer);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HandheldClamShellComputer);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HandheldComputer);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableComputer);
        break;
    case QBluetoothDeviceInfo::PhoneDevice:
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedPhone);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CellularPhone);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CordlessPhone);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(SmartPhone);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WiredModemOrVoiceGatewayPhone);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CommonIsdnAccessPhone);
        break;
    #if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    case QBluetoothDeviceInfo::LANAccessDevice: // Deprecated since Qt 5.13.
    #else
    case QBluetoothDeviceInfo::NetworkDevice: // Added in Qt 5.13.
    #endif
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkFullService);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorOne);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorTwo);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorThree);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorFour);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorFive);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkLoadFactorSix);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(NetworkNoService);
        break;
    case QBluetoothDeviceInfo::AudioVideoDevice:
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedAudioVideoDevice);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableHeadsetDevice);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HandsFreeDevice);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Microphone);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Loudspeaker);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Headphones);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(PortableAudioDevice);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CarAudio);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(SetTopBox);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HiFiAudioDevice);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Vcr);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(VideoCamera);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(Camcorder);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(VideoMonitor);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(VideoDisplayAndLoudspeaker);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(VideoConferencing);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(GamingDevice);
        break;
    case QBluetoothDeviceInfo::PeripheralDevice:
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedPeripheral);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(KeyboardPeripheral);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(PointingDevicePeripheral);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(KeyboardWithPointingDevicePeripheral);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(JoystickPeripheral);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(GamepadPeripheral);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(RemoteControlPeripheral);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(SensingDevicePeripheral);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(DigitizerTabletPeripheral);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(CardReaderPeripheral);
        break;
    case QBluetoothDeviceInfo::ImagingDevice:
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedImagingDevice);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImageDisplay);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImageCamera);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImageScanner);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ImagePrinter);
        break;
    case QBluetoothDeviceInfo::WearableDevice:
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedWearableDevice);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableWristWatch);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearablePager);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableJacket);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableHelmet);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(WearableGlasses);
        break;
    case QBluetoothDeviceInfo::ToyDevice:
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedToy);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyRobot);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyVehicle);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyDoll);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyController);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(ToyGame);
        break;
    case QBluetoothDeviceInfo::HealthDevice:
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(UncategorizedHealthDevice);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthBloodPressureMonitor);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthThermometer);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthWeightScale);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthGlucoseMeter);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthPulseOximeter);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthDataDisplay);
        DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN(HealthStepCounter);
        break;
    case QBluetoothDeviceInfo::UncategorizedDevice:
        // There are no minor classes defined (in Qt) for uncategorized devices.
        break;
    }
    #undef DOKIT_INTERNAL_IF_EQUAL_THEN_RETURN
    qCDebug(lc).noquote() << tr("Unknown minor class %1 for major class %2.")
        .arg(minorClass).arg(majorClass);
    return QString(); // Null QString indicates unknown minor class.
}
