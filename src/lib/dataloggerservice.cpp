// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the DataLoggerService and DataLoggerServicePrivate classes.
 */

#include <qtpokit/dataloggerservice.h>
#include "dataloggerservice_p.h"
#include "pokitproducts_p.h"

#include <qtpokit/statusservice.h>

#include <QDataStream>
#include <QIODevice>
#include <QLowEnergyController>
#include <QtEndian>

/*!
 * \class DataLoggerService
 *
 * The DataLoggerService class accesses the `Data Logger` service of Pokit devices.
 */

/// Returns \a mode as a user-friendly string.
QString DataLoggerService::toString(const Mode &mode)
{
    switch (mode) {
    case Mode::Idle:        return tr("Idle");
    case Mode::DcVoltage:   return tr("DC voltage");
    case Mode::AcVoltage:   return tr("AC voltage");
    case Mode::DcCurrent:   return tr("DC current");
    case Mode::AcCurrent:   return tr("AC current");
    case Mode::Temperature: return tr("Temperature");
    default:                return QString();
    }
}

/// Returns \a range as a user-friendly string, or a null QString if \a mode has no ranges.
QString DataLoggerService::toString(const PokitProduct product, const quint8 range, const Mode mode)
{
    switch (mode) {
    case Mode::Idle:
        break;
    case Mode::DcVoltage:
    case Mode::AcVoltage:
        return VoltageRange::toString(product, range);
    case Mode::DcCurrent:
    case Mode::AcCurrent:
        return CurrentRange::toString(product, range);
    case Mode::Temperature:
        break;
    }
    return QString();
}

/// Returns \a range as a user-friendly string, or a null QString if \a mode has no ranges.
QString DataLoggerService::toString(const quint8 range, const Mode mode) const
{
    return toString(*pokitProduct(), range, mode);
}

/*!
 * Returns the maximum value for \a range, or 0 if \a range is not a known value for \a product's \a mode.
 */
quint32 DataLoggerService::maxValue(const PokitProduct product, const quint8 range, const Mode mode)
{
    switch (mode) {
    case Mode::Idle:
        break;
    case Mode::DcVoltage:
    case Mode::AcVoltage:
        return VoltageRange::maxValue(product, range);
    case Mode::DcCurrent:
    case Mode::AcCurrent:
        return CurrentRange::maxValue(product, range);
    case Mode::Temperature:
        break;
    }
    return 0;
}

/*!
 * Returns the maximum value for \a range, or 0 \a range is not a known value for the current \a product's \a mode.
 */
quint32 DataLoggerService::maxValue(const quint8 range, const Mode mode) const
{
    return maxValue(*pokitProduct(), range, mode);
}

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

bool DataLoggerService::readCharacteristics()
{
    return readMetadataCharacteristic();
}

/*!
 * Reads the `DataLogger` service's `Metadata` characteristic.
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

    const bool updateIntervalIs32bit =
        (d->getCharacteristic(CharacteristicUuids::metadata).value().size() >= 23);
    const QByteArray value = DataLoggerServicePrivate::encodeSettings(settings, updateIntervalIs32bit);
    if (value.isNull()) {
        return false;
    }

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
    return setSettings({ DataLoggerService::Command::Stop,  0, DataLoggerService::Mode::Idle, 0, 0, 0 });
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
    return setSettings({ DataLoggerService::Command::Refresh, 0, DataLoggerService::Mode::Idle, 0, 0, 0 });
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
        : Metadata{ LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(), Mode::Idle, 0, 0, 0, 0 };
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
 * Instantaneous reads can still be fetched by readMetadataCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool DataLoggerService::disableMetadataNotifications()
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
 * Returns \a settings in the format Pokit devices expect. If \a updateIntervalIs32bit is \c true
 * then the `Update Interval` field will be encoded in 32-bit instead of 16.
 */
QByteArray DataLoggerServicePrivate::encodeSettings(const DataLoggerService::Settings &settings,
                                                    const bool updateIntervalIs32bit)
{
    static_assert(sizeof(settings.command)        == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.arguments)      == 2, "Expected to be 2 bytes.");
    static_assert(sizeof(settings.mode)           == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.range)          == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.updateInterval) == 4, "Expected to be 4 bytes.");
    static_assert(sizeof(settings.timestamp)      == 4, "Expected to be 4 bytes.");

    QByteArray value;
    QDataStream stream(&value, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision); // 32-bit floats, not 64-bit.
    stream << (quint8)settings.command << settings.arguments << (quint8)settings.mode << settings.range;

    /*!
     * \pokitApi For Pokit Meter, `updateInterval` is `uint16` seconds (as per the Pokit API 1.00),
     * however for Pokit Pro it's `uint32` milliseconds, even though that's not officially
     * documented anywhere.
     */

    if (!updateIntervalIs32bit) {
        stream << (quint16)((settings.updateInterval+500)/1000) << settings.timestamp;
        Q_ASSERT(value.size() == 11); // According to Pokit API 1.00.
    } else {
        stream << settings.updateInterval << settings.timestamp;
        Q_ASSERT(value.size() == 13); // According to testing / experimentation.
    }
    return value;
}

/*!
 * Parses the `Metadata` \a value into a DataLoggerService::Metatdata struct.
 */
DataLoggerService::Metadata DataLoggerServicePrivate::parseMetadata(const QByteArray &value)
{
    DataLoggerService::Metadata metadata{
        DataLoggerService::LoggerStatus::Error, std::numeric_limits<float>::quiet_NaN(),
        DataLoggerService::Mode::Idle, 0, 0, 0, 0
    };

    // Pokit Meter: 15 bytes, Pokit Pro: 23 bytes.
    if (!checkSize(QLatin1String("Metadata"), value, 15, 23)) {
        return metadata;
    }

    qCDebug(lc) << value.mid(7,12).toHex(',');
    metadata.status = static_cast<DataLoggerService::LoggerStatus>(value.at(0));
    metadata.scale  = qFromLittleEndian<float>(value.mid(1,4).constData());
    metadata.mode   = static_cast<DataLoggerService::Mode>(value.at(5));
    metadata.range  = static_cast<quint8>(value.at(6));

    /*!
     * \pokitApi For Pokit Meter, `updateInterval` is `uint16` (as per the Pokit API 1.00), however
     * for Pokit Pro it's `uint32`, even though that's not officially documented anywhere.
     * Also note, the doc claims 'microseconds' (ie 10^-6), but clearly the value is 'milliseconds'
     * (ie 10^-3) for Pokit Pro, and whole seconds for Pokit Meter.
     */

    if (value.size() == 15) {
        metadata.updateInterval  = qFromLittleEndian<quint16>(value.mid(7,2).constData())*1000;
        metadata.numberOfSamples = qFromLittleEndian<quint16>(value.mid(9,2).constData());
        metadata.timestamp       = qFromLittleEndian<quint32>(value.mid(11,4).constData());
    } else if (value.size() == 23) {
        metadata.updateInterval  = qFromLittleEndian<quint32>(value.mid(7,4).constData());
        metadata.numberOfSamples = qFromLittleEndian<quint16>(value.mid(11,2).constData());
        metadata.timestamp       = qFromLittleEndian<quint32>(value.mid(19,4).constData());
    } else {
        qCWarning(lc).noquote() << tr("Cannot decode metadata of %n byte/s: %1", nullptr, value.size())
            .arg(toHexString(value));
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
        qCWarning(lc).noquote() << tr("Samples value has odd size %1 (should be even): %2")
            .arg(value.size()).arg(toHexString(value));
        return samples;
    }
    while ((samples.size()*2) < value.size()) {
        samples.append(qFromLittleEndian<qint16>(value.mid(samples.size()*2,2).constData()));
    }
    qCDebug(lc).noquote() << tr("Read %n sample/s from %1-bytes.", nullptr, samples.size()).arg(value.size());
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
        Q_EMIT q->metadataRead(parseMetadata(value));
        return;
    }

    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::reading) {
        qCWarning(lc).noquote() << tr("Reading characteristic is notify-only")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic read for Data Logger service")
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
        Q_EMIT q->settingsWritten();
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
        Q_EMIT q->metadataRead(parseMetadata(newValue));
        return;
    }

    if (characteristic.uuid() == DataLoggerService::CharacteristicUuids::reading) {
        Q_EMIT q->samplesRead(parseSamples(newValue));
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic notified for Data Logger service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
