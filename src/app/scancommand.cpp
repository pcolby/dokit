// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "scancommand.h"

#include <qtpokit/pokitdiscoveryagent.h>

#include <QBluetoothUuid>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

/*!
 * \class ScanCommand
 *
 * The ScanCommand class implements the `scan` CLI command, by scanning for nearby Pokit Bluetooth
 * devices. When devices are found, they are logged to stdout in the chosen format.
 */

/*!
 * Construct a new ScanCommand object with \a parent.
 */
ScanCommand::ScanCommand(QObject * const parent) : AbstractCommand(parent), showCsvHeader(true)
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
            std::cout << qPrintable(tr("uuid,address,name,major_class,minor_class,signal_strength\n"));
        }
        std::cout << qPrintable(QString::fromLatin1("%1,%2,%3,%4,%5,%6\n").arg(info.deviceUuid().toString(),
            info.address().toString(), escapeCsvField(info.name()), toString(info.majorDeviceClass()),
            toString(info.majorDeviceClass(), info.minorDeviceClass())).arg(info.rssi()));
        break;
    case OutputFormat::Json:
        std::cout << QJsonDocument(toJson(info)).toJson().toStdString();
        break;
    case OutputFormat::Text:
        std::cout << qPrintable(tr("%1 %2 %3 %4\n").arg(info.deviceUuid().toString(),
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
QJsonArray ScanCommand::toJson(const QBluetoothDeviceInfo::CoreConfigurations &configurations)
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
QString ScanCommand::toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass)
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
    qCDebug(lc).noquote() << QObject::tr("Unknown major class %1.").arg(majorClass);
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
    qCDebug(lc).noquote() << QObject::tr("Unknown minor class %1 for major class %2.")
        .arg(minorClass).arg(majorClass);
    return QString(); // Null QString indicates unknown minor class.
}

QTPOKIT_END_NAMESPACE
