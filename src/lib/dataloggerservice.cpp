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
 * Defines the DataLoggerService and DataLoggerServicePrivate classes.
 */

#include <qtpokit/dataloggerservice.h>
#include "dataloggerservice_p.h"

#include <QDataStream>
#include <QIODevice>
#include <QtEndian>

/*!
 * \class DataLoggerService
 *
 * The DataLoggerService class accesses the `Pokit Status` service of Pokit devices.
 */

/// UUID of the "DataLogger" service.
const QBluetoothUuid DataLoggerService::
    serviceUuid(QLatin1String("a5ff3566-1fd8-4e10-8362-590a578a4121"));

/// \struct DataLoggerService::CharacteristicUuids
/// \brief Characteristics available via the `DataLogger` service.

/// UUID of the `DataLogger` service's `Settings` characterstic.
const QBluetoothUuid DataLoggerService::CharacteristicUuids::
    settings(QLatin1String("5f97c62b-a83b-46c6-b9cd-cac59e130a78"));

/// UUID of the `DataLogger` service's `Metadata` characterstic.
const QBluetoothUuid DataLoggerService::CharacteristicUuids::
    metadata(QLatin1String("9acada2e-3936-430b-a8f7-da407d97ca6e"));

/// UUID of the `DataLogger` service's `Reading` characterstic.
const QBluetoothUuid DataLoggerService::CharacteristicUuids::
    reading(QLatin1String("3c669dab-fc86-411c-9498-4f9415049cc0"));

/// \enum DataLoggerService::Command
/// \brief Values supported by the `Command` attribute of the `Settings` characteristic.

/// \enum DataLoggerService::Mode
/// \brief Values supported by the `Mode` attribute of the `Settings` and `Metadata` characteristics.

/// Returns \a mode as a user-friendly string.
QString DataLoggerService::toString(const Mode &mode)
{
    switch (mode) {
    case Mode::Idle:        return tr("Idle");
    case Mode::DcVoltage:   return tr("DC voltage");
    case Mode::AcVoltage:   return tr("AC voltage");
    case Mode::DcCurrent:   return tr("DC current");
    case Mode::AcCurrent:   return tr("AC current");
    default:                return QString();
    }
}

/// \enum DataLoggerService::VoltageRange
/// \brief Values supported by the `Range` attribute of the `Settings` and `Metadata` characteristics,
/// when `Mode` is AC or DC voltage.

/// Returns \a range as a user-friendly string.
QString DataLoggerService::toString(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_0_to_300mV:  return tr("0 to 300mV");
    case VoltageRange::_300mV_to_2V: return tr("300mV to 2V");
    case VoltageRange::_2V_to_6V:    return tr("2V to 6V");
    case VoltageRange::_6V_to_12V:   return tr("6V to 12V");
    case VoltageRange::_12V_to_30V:  return tr("12V to 30V");
    case VoltageRange::_30V_to_60V:  return tr("30V to 60V");
    default:                         return QString();
    }
}

/*!
 *  Returns the minimum value for \a range in (integer) millivolts, or null QVariant if \a range is
 *  not valid.
 *
 *  Note, this is an *absolute* minimum. That is, the true range for DC measurements is from
 *  `-maxValue(range)` to `+maxValue(range)`. In this sense, `minValue(range)` indicates the
 *  magnitude (ignore signs) that can be measured accurately for the given \a range. As AC voltage
 *  can never be negative, this is relevant for DC voltage only.
 */
QVariant DataLoggerService::minValue(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_0_to_300mV:  return     0;
    case VoltageRange::_300mV_to_2V: return   300;
    case VoltageRange::_2V_to_6V:    return  2000;
    case VoltageRange::_6V_to_12V:   return  6000;
    case VoltageRange::_12V_to_30V:  return 12000;
    case VoltageRange::_30V_to_60V:  return 30000;
    default:                         return QVariant();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) millivolts, or null QVariant if \a range is
 *  not valid.
 */
QVariant DataLoggerService::maxValue(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_0_to_300mV:  return   300;
    case VoltageRange::_300mV_to_2V: return  2000;
    case VoltageRange::_2V_to_6V:    return  6000;
    case VoltageRange::_6V_to_12V:   return 12000;
    case VoltageRange::_12V_to_30V:  return 30000;
    case VoltageRange::_30V_to_60V:  return 60000;
    default:                         return QVariant();
    }
}

/// \enum DataLoggerService::CurrentRange
/// \brief Values supported by the `Range` attribute of the `Settings` and `Metadata` characteristics,
/// when `Mode` is AC or DC current.

/// Returns \a range as a user-friendly string.
QString DataLoggerService::toString(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_0_to_10mA:      return tr("0 to 10mA");
    case CurrentRange::_10mA_to_30mA:   return tr("10mA to 30mA");
    case CurrentRange::_30mA_to_150mA:  return tr("30mA to 150mA");
    case CurrentRange::_150mA_to_300mA: return tr("150mA to 300mA");
    case CurrentRange::_300mA_to_3A:    return tr("300mA to 3A");
    default:                            return QString();
    }
}

/*!
 *  Returns the minimum value for \a range in (integer) milliamps, or null QVariant if \a range is
 *  not valid.
 *
 *  Note, this is an *absolute* minimum. That is, the true range for DC measurements is from
 *  `-maxValue(range)` to `+maxValue(range)`. In this sense, `minValue(range)` indicates the
 *  magnitude (ignore signs) that can be measured accurately for the given \a range. As AC current
 *  can never be negative, this is relevant for DC current only.
 */
QVariant DataLoggerService::minValue(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_0_to_10mA:      return   0;
    case CurrentRange::_10mA_to_30mA:   return  10;
    case CurrentRange::_30mA_to_150mA:  return  30;
    case CurrentRange::_150mA_to_300mA: return 150;
    case CurrentRange::_300mA_to_3A:    return 300;
    default:                            return QVariant();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) milliamps, or null QVariant if \a range is
 *  not valid.
 */
QVariant DataLoggerService::maxValue(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_0_to_10mA:      return   10;
    case CurrentRange::_10mA_to_30mA:   return   30;
    case CurrentRange::_30mA_to_150mA:  return  150;
    case CurrentRange::_150mA_to_300mA: return  300;
    case CurrentRange::_300mA_to_3A:    return 3000;
    default:                            return QVariant();
    }
}

/// \union DataLoggerService::Range
/// \brief Values supported by the `Range` attribute of the `Settings` and `Metadata` characteristics.

/// Returns \a range as a user-friendly string, or a null QString if \a mode has no ranges.
QString DataLoggerService::toString(const Range &range, const Mode &mode)
{
    switch (mode) {
    case Mode::DcVoltage:
    case Mode::AcVoltage:
        return toString(range.voltageRange);
    case Mode::DcCurrent:
    case Mode::AcCurrent:
        return toString(range.currentRange);
    default:
        return QString();
    }
}

/// \struct DataLoggerService::Settings
/// \brief Attributes included in the `Settings` characterstic.

/// \enum DataLoggerService::LoggerStatus
/// \brief Values supported by the `Status` attribute of the `Metadata` characteristic.

/// \struct DataLoggerService::Metadata
/// \brief Attributes included in the `Metadata` characterstic.

/*!
 * \typedef DataLoggerService::Samples
 *
 * Raw samples from the `Reading` characteristic. These raw samples are (supposedly) within the
 * range -2048 to +2047, and need to be multiplied by the Metadata::scale value from the `Metadata`
 * characteristc to get the true values.
 *
 * Also supposedly, there should be no more than 10 samples at a time, according to Pokit's current
 * API docs. There is not artificial limitation imposed by QtPokit, so devices may begin batching
 * more samples in future. Specifically, the Pokit Pro seems to send 88 samples (in 176 bytes) at a
 * time.
 */

/*!
 * Constructs a new Pokit service with \a parent.
 */
DataLoggerService::DataLoggerService(QLowEnergyController * const controller, QObject * parent)
    : AbstractPokitService(new DataLoggerServicePrivate(controller, this), parent)
{

}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
DataLoggerService::DataLoggerService(
    DataLoggerServicePrivate * const d, QObject * const parent)
    : AbstractPokitService(d, parent)
{

}
/// \endcond

/*!
 * Destroys this DataLoggerService object.
 */
DataLoggerService::~DataLoggerService()
{

}

bool DataLoggerService::readCharacteristics()
{
    return readMetadataCharacteristic();
}

/*!
 * Read the `DataLogger` service's `Metadat` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits metadataRead() if/when the characteristic has been read successfully.
 */
bool DataLoggerService::readMetadataCharacteristic()
{
    Q_D(DataLoggerService);
    return d->readCharacteristic(CharacteristicUuids::metadata);
}

/*!
 * Configures the Pokit device's data logger mode.
 *
 * Returns `true` if the write request was successfully queued, `false` otherwise.
 *
 * Emits settingsWritten() if/when the \a settings have been writtem successfully.
 */
bool DataLoggerService::setSettings(const Settings &settings)
{
    Q_D(const DataLoggerService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::settings);
    if (!characteristic.isValid()) {
        return false;
    }

    static_assert(sizeof(settings.command)        == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.arguments)      == 2, "Expected to be 2 bytes.");
    static_assert(sizeof(settings.mode)           == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.range)          == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.updateInterval) == 2, "Expected to be 2 bytes.");
    static_assert(sizeof(settings.timestamp)      == 4, "Expected to be 4 bytes.");

    QByteArray value;
    QDataStream stream(&value, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision); // 32-bit floats, not 64-bit.
    stream << (quint8)settings.command << settings.arguments << (quint8)settings.mode
           << (quint8)settings.range.voltageRange << (quint32)settings.updateInterval << settings.timestamp;
    /*!
     * \pokitApi Despit the Pokit API docs saying `Update Interval` is `uint16`, Pokit Pro devices
     * error if try to set such. However, if we switch `Update Interval` to `uint32` then the fields
     * are set properly (according to the resulting `Metadata`). Not sure if this is a Pokit Pro
     * difference, or general API doc error.
     */
    Q_ASSERT(value.size() == 13);
    d->service->writeCharacteristic(characteristic, value);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}

/*!
 * Start the data logger with \a settings.
 *
 * This is just a synonym for setSettings() except makes the caller's intention more explicit, and
 * sanity-checks that the settings's command is DataLoggerService::Command::Start.
 */
bool DataLoggerService::startLogger(const Settings &settings)
{
    Q_D(const DataLoggerService);
    Q_ASSERT(settings.command == DataLoggerService::Command::Start);
    if (settings.command != DataLoggerService::Command::Start) {
        qCWarning(d->lc).noquote() << tr("Settings command must be 'Start'.");
        return false;
    }
    return setSettings(settings);
}

/*!
 * Stop the data logger.
 *
 * This is just a convenience function equivalent to calling setSettings() with the command set to
 * DataLoggerService::Command::Stop.
 */
bool DataLoggerService::stopLogger()
{
    // Note, only the Settings::command member need be set, since the others are all ignored by the
    // Pokit device when the command is Stop. However, we still explicitly initialise all other
    // members just to ensure we're never exposing uninitialised RAM to an external device.
    return setSettings({
        DataLoggerService::Command::Stop,
        0, DataLoggerService::Mode::Idle,
        { DataLoggerService::VoltageRange::_0_to_300mV }, 0, 0
    });
}

/*!
 * Start the data logger.
 *
 * This is just a convenience function equivalent to calling setSettings() with the command set to
 * DataLoggerService::Command::Refresh.
 *
 * Once the Pokit device has processed this request succesffully, the device will begin notifying
 * the `Metadata` and `Reading` characteristic, resulting in emits of metadataRead and samplesRead
 * respectively.
 */
bool DataLoggerService::fetchSamples()
{
    // Note, only the Settings::command member need be set, since the others are all ignored by the
    // Pokit device when the command is Refresh. However, we still explicitly initialise all other
    // members just to ensure we're never exposing uninitialised RAM to an external device.
    return setSettings({
        DataLoggerService::Command::Refresh,
        0, DataLoggerService::Mode::Idle,
        { DataLoggerService::VoltageRange::_0_to_300mV }, 0, 0
    });
}

/*!
 * Returns the most recent value of the `DataLogger` service's `Metadata` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then the
 * returned DataLoggerService::Metadata::scale member will be a quiet NaN, which can be checked like:
 *
 * ```
 * const DataLoggerService::Metadata metadata = multimeterService->metadata();
 * if (qIsNaN(metadata.scale)) {
 *     // Handle failure.
 * }
 * ```
 */
DataLoggerService::Metadata DataLoggerService::metadata() const
{
    Q_D(const DataLoggerService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::metadata);
    return (characteristic.isValid()) ? DataLoggerServicePrivate::parseMetadata(characteristic.value())
        : Metadata{ LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(),
                    Mode::Idle, { VoltageRange::_0_to_300mV }, 0, 0, 0 };
}

/*!
 * Enables client-side notifications of Data Logger metadata changes.
 *
 * This is an alternative to manually requesting individual reads via readMetadataCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 *
 * Successfully read values (if any) will be emitted via the metadataRead() signal.
 */
bool DataLoggerService::enableMetadataNotifications()
{
    Q_D(DataLoggerService);
    return d->enableCharacteristicNotificatons(CharacteristicUuids::metadata);
}

/*!
 * Disables client-side notifications of Data Logger metadata changes.
 *
 * Instantaneous reads can still be fetched by readMetadatCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool DataLoggerService::disableMetadatNotifications()
{
    Q_D(DataLoggerService);
    return d->disableCharacteristicNotificatons(CharacteristicUuids::metadata);
}

/*!
 * Enables client-side notifications of Data Logger readings.
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 *
 * Successfully read samples (if any) will be emitted via the samplesRead() signal.
 */
bool DataLoggerService::enableReadingNotifications()
{
    Q_D(DataLoggerService);
    return d->enableCharacteristicNotificatons(CharacteristicUuids::reading);
}

/*!
 * Disables client-side notifications of Data Logger readings.
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool DataLoggerService::disableReadingNotifications()
{
    Q_D(DataLoggerService);
    return d->disableCharacteristicNotificatons(CharacteristicUuids::reading);
}

/*!
 * \fn DataLoggerService::settingsWritten
 *
 * This signal is emitted when the `Settings` characteristic has been written successfully.
 *
 * \see setSettings
 */

/*!
 * \fn DataLoggerService::metadataRead
 *
 * This signal is emitted when the `Metadata` characteristic has been read successfully.
 *
 * \see readMetadataCharacteristic
 */

/*!
 * \fn DataLoggerService::samplesRead
 *
 * This signal is emitted when the `Reading` characteristic has been notified.
 *
 * \see beginSampling
 * \see stopSampling
 */


/*!
 * \cond internal
 * \class DataLoggerServicePrivate
 *
 * The DataLoggerServicePrivate class provides private implementation for DataLoggerService.
 */

/*!
 * \internal
 * Constructs a new DataLoggerServicePrivate object with public implementation \a q.
 */
DataLoggerServicePrivate::DataLoggerServicePrivate(
    QLowEnergyController * controller, DataLoggerService * const q)
    : AbstractPokitServicePrivate(DataLoggerService::serviceUuid, controller, q)
{

}

/*!
 * Parses the `Metadata` \a value into a DataLoggerService::Metatdata struct.
 */
DataLoggerService::Metadata DataLoggerServicePrivate::parseMetadata(const QByteArray &value)
{
    DataLoggerService::Metadata metadata{
        DataLoggerService::LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(),
        DataLoggerService::Mode::Idle, { DataLoggerService::VoltageRange::_0_to_300mV },
        0, 0, 0
    };

    if (!checkSize(QLatin1String("Metadata"), value, 15, 23)) {
        return metadata;
    }

    qCDebug(lc) << value.mid(7,12).toHex(',');
    metadata.status             = static_cast<DataLoggerService::LoggerStatus>(value.at(0));
    metadata.scale              = qFromLittleEndian<float>(value.mid(1,4));
    metadata.mode               = static_cast<DataLoggerService::Mode>(value.at(5));
    metadata.range.voltageRange = static_cast<DataLoggerService::VoltageRange>(value.at(6));
    /// \todo All good to here. After this, (Pokit Pro) reality differs to the API docs.

    /*!
     * \pokitApi Pokit API 1.00 (and 0.02) claim that `updateInterval` is `uint16`, but Pokit Pro
     * returns the value as `uint32`. Indeed, the maxium value (24 hours) does not fit in 16-bits.
     * Also note, the doc claims 'microseconds' (ie 10^-6), but clearly the value is 'milliseconds'
     * (ie 10^-3).
     */

    /// \todo No idea (yet) what 8 bytes get inserted *before* the timestamp attribute.
    if (value.size() < 23) {
        metadata.updateInterval  = qFromLittleEndian<quint16>(value.mid(7,2));
        metadata.numberOfSamples = qFromLittleEndian<quint16>(value.mid(9,2));
        metadata.timestamp       = qFromLittleEndian<quint32>(value.mid(11,4));
    } else {
        metadata.updateInterval  = qFromLittleEndian<quint16>(value.mid(7,4));
        metadata.numberOfSamples = qFromLittleEndian<quint16>(value.mid(11,4));
        metadata.timestamp       = qFromLittleEndian<quint32>(value.mid(19,4));
    }
    return metadata;
}

/*!
 * Parses the `Reading` \a value into a DataLoggerService::Samples vector.
 */
DataLoggerService::Samples DataLoggerServicePrivate::parseSamples(const QByteArray &value)
{
    DataLoggerService::Samples samples;
    if ((value.size()%2) != 0) {
        qCWarning(lc).noquote() << tr("Samples value has odd size (should be even): 0x%2")
            .arg(value.size()).arg(QLatin1String(value.toHex()));
        return samples;
    }
    for (; (samples.size()*2) < value.size();) {
        samples.append(qFromLittleEndian<qint16>(value.mid(samples.size()*2,2)));
    }
    qCDebug(lc).noquote() << tr("Read %1 samples from %2-bytes.")
        .arg(samples.size()).arg(value.size());
    return samples;
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicRead to parse \a value, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void DataLoggerServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    AbstractPokitServicePrivate::characteristicRead(characteristic, value);

    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::settings) {
        qCWarning(lc).noquote() << tr("Settings characteristic is write-only, but somehow read")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    Q_Q(DataLoggerService);
    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::metadata) {
        emit q->metadataRead(parseMetadata(value));
        return;
    }

    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::reading) {
        qCWarning(lc).noquote() << tr("Reading characteristic is notify-only")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic read for Data Logger Service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void DataLoggerServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                     const QByteArray &newValue)
{
    AbstractPokitServicePrivate::characteristicWritten(characteristic, newValue);

    Q_Q(DataLoggerService);
    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::settings) {
        emit q->settingsWritten();
        return;
    }

    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::metadata) {
        qCWarning(lc).noquote() << tr("Metadata characteristic is read/notify, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::reading) {
        qCWarning(lc).noquote() << tr("Reading characteristic is notify-only, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic written for Data Logger service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicChanged to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void DataLoggerServicePrivate::characteristicChanged(const QLowEnergyCharacteristic &characteristic,
                                                     const QByteArray &newValue)
{
    AbstractPokitServicePrivate::characteristicChanged(characteristic, newValue);

    Q_Q(DataLoggerService);
    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::settings) {
        qCWarning(lc).noquote() << tr("Settings characteristic is write-only, but somehow updated")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::metadata) {
        emit q->metadataRead(parseMetadata(newValue));
        return;
    }

    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::reading) {
        emit q->samplesRead(parseSamples(newValue));
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic notified for Data Logger service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
