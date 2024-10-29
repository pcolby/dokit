// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the DsoService and DsoServicePrivate classes.
 */

#include <qtpokit/dsoservice.h>
#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>
#include "dsoservice_p.h"
#include "pokitproducts_p.h"

#include <QDataStream>
#include <QIODevice>
#include <QtEndian>

/*!
 * \class DsoService
 *
 * The DsoService class accesses the `DSO` (Digital Storage Oscilloscope) service of Pokit devices.
 */

/// Returns \a mode as a user-friendly string.
QString DsoService::toString(const Mode &mode)
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

/// Returns \a range as a user-friendly string, or a null QString if \a mode has no ranges.
QString DsoService::toString(const PokitProduct product, const quint8 range, const Mode mode)
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
    }
    return QString();
}

/// Returns \a range as a user-friendly string, or a null QString if \a mode has no ranges.
QString DsoService::toString(const quint8 range, const Mode mode) const
{
    return toString(*pokitProduct(), range, mode);
}

/*!
 *  Returns the maximum value for \a range, or the string "Auto".
 *
 *  If \a range is not a known valid enumeration value for \a product's \a mode, then a null QVariant is returned.
 */
QVariant DsoService::maxValue(const PokitProduct product, const quint8 range, const Mode mode)
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
    }
    return QVariant();
}

/*!
 *  Returns the maximum value for \a range, or the string "Auto".
 *
 *  If \a range is not a known valid enumeration value for the current \a product's \a mode,
 *  then a null QVariant is returned.
 */
QVariant DsoService::maxValue(const quint8 range, const Mode mode) const
{
    return maxValue(*pokitProduct(), range, mode);
}

/*!
 * \typedef DsoService::Samples
 *
 * Raw samples from the `Reading` characteristic. These raw samples are (supposedly) wihtin the
 * range -2048 to +2047, and need to be multiplied by the Metadata::scale value from the `Metadata`
 * characteristc to get the true values.
 *
 * Also supposedly, there should be no more than 10 samples at a time, according to Pokit's current
 * API docs. There is not artificial limitation imposed by QtPokit, so devices may begin batching
 * more samples in future.
 */

/*!
 * Constructs a new Pokit service with \a parent.
 */
DsoService::DsoService(QLowEnergyController * const controller, QObject * parent)
    : AbstractPokitService(new DsoServicePrivate(controller, this), parent)
{

}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
DsoService::DsoService(
    DsoServicePrivate * const d, QObject * const parent)
    : AbstractPokitService(d, parent)
{

}
/// \endcond

bool DsoService::readCharacteristics()
{
    return readMetadataCharacteristic();
}

/*!
 * Reads the `DSO` service's `Metadata` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits metadataRead() if/when the characteristic has been read successfully.
 */
bool DsoService::readMetadataCharacteristic()
{
    Q_D(DsoService);
    return d->readCharacteristic(CharacteristicUuids::metadata);
}

/*!
 * Configures the Pokit device's DSO mode.
 *
 * Returns `true` if the write request was successfully queued, `false` otherwise.
 *
 * Emits settingsWritten() if/when the \a settings have been writtem successfully.
 */
bool DsoService::setSettings(const Settings &settings)
{
    Q_D(const DsoService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::settings);
    if (!characteristic.isValid()) {
        return false;
    }

    const QByteArray value = DsoServicePrivate::encodeSettings(settings);
    if (value.isNull()) {
        return false;
    }

    d->service->writeCharacteristic(characteristic, value);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}

/*!
 * Start the DSO with \a settings.
 *
 * This is just a synonym for setSettings() except makes the caller's intention more explicit, and
 * sanity-checks that the settings's command is not DsoService::Command::ResendData.
 */
bool DsoService::startDso(const Settings &settings)
{
    Q_D(const DsoService);
    Q_ASSERT(settings.command != DsoService::Command::ResendData);
    if (settings.command == DsoService::Command::ResendData) {
        qCWarning(d->lc).noquote() << tr("Settings command must not be 'ResendData'.");
        return false;
    }
    return setSettings(settings);
}

/*!
 * Fetch DSO samples.
 *
 * This is just a convenience function equivalent to calling setSettings() with the command set to
 * DsoService::Command::Refresh.
 *
 * Once the Pokit device has processed this request succesffully, the device will begin notifying
 * the `Metadata` and `Reading` characteristic, resulting in emits of metadataRead and samplesRead
 * respectively.
 */
bool DsoService::fetchSamples()
{
    // Note, only the Settings::command member need be set, since the others are all ignored by the
    // Pokit device when the command is Refresh. However, we still explicitly initialise all other
    // members just to ensure we're never exposing uninitialised RAM to an external device.
    return setSettings({ DsoService::Command::ResendData, 0, DsoService::Mode::Idle, 0, 0, 0 });
}

/*!
 * Returns the most recent value of the `DSO` service's `Metadata` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then the
 * returned DsoService::Metadata::scale member will be a quiet NaN, which can be checked like:
 *
 * ```
 * const DsoService::Metadata metadata = multimeterService->metadata();
 * if (qIsNaN(metadata.scale)) {
 *     // Handle failure.
 * }
 * ```
 */
DsoService::Metadata DsoService::metadata() const
{
    Q_D(const DsoService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::metadata);
    return (characteristic.isValid()) ? DsoServicePrivate::parseMetadata(characteristic.value())
        : Metadata{ DsoStatus::Error, std::numeric_limits<float>::quiet_NaN(), Mode::Idle, 0, 0, 0, 0 };
}

/*!
 * Enables client-side notifications of DSO metadata changes.
 *
 * This is an alternative to manually requesting individual reads via readMetadataCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 *
 * Successfully read values (if any) will be emitted via the metadataRead() signal.
 */
bool DsoService::enableMetadataNotifications()
{
    Q_D(DsoService);
    return d->enableCharacteristicNotificatons(CharacteristicUuids::metadata);
}

/*!
 * Disables client-side notifications of DSO metadata changes.
 *
 * Instantaneous reads can still be fetched by readMetadataCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool DsoService::disableMetadataNotifications()
{
    Q_D(DsoService);
    return d->disableCharacteristicNotificatons(CharacteristicUuids::metadata);
}

/*!
 * Enables client-side notifications of DSO readings.
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 *
 * Successfully read samples (if any) will be emitted via the samplesRead() signal.
 */
bool DsoService::enableReadingNotifications()
{
    Q_D(DsoService);
    return d->enableCharacteristicNotificatons(CharacteristicUuids::reading);
}

/*!
 * Disables client-side notifications of DSO readings.
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool DsoService::disableReadingNotifications()
{
    Q_D(DsoService);
    return d->disableCharacteristicNotificatons(CharacteristicUuids::reading);
}

/*!
 * \fn DsoService::settingsWritten
 *
 * This signal is emitted when the `Settings` characteristic has been written successfully.
 *
 * \see setSettings
 */

/*!
 * \fn DsoService::metadataRead
 *
 * This signal is emitted when the `Metadata` characteristic has been read successfully.
 *
 * \see readMetadataCharacteristic
 */

/*!
 * \fn DsoService::samplesRead
 *
 * This signal is emitted when the `Reading` characteristic has been notified.
 *
 * \see beginSampling
 * \see stopSampling
 */


/*!
 * \cond internal
 * \class DsoServicePrivate
 *
 * The DsoServicePrivate class provides private implementation for DsoService.
 */

/*!
 * \internal
 * Constructs a new DsoServicePrivate object with public implementation \a q.
 */
DsoServicePrivate::DsoServicePrivate(
    QLowEnergyController * controller, DsoService * const q)
    : AbstractPokitServicePrivate(DsoService::serviceUuid, controller, q)
{

}

/*!
 * Returns \a settings in the format Pokit devices expect.
 */
QByteArray DsoServicePrivate::encodeSettings(const DsoService::Settings &settings)
{
    static_assert(sizeof(settings.command)         == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.triggerLevel)    == 4, "Expected to be 2 bytes.");
    static_assert(sizeof(settings.mode)            == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.range)           == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.samplingWindow)  == 4, "Expected to be 4 bytes.");
    static_assert(sizeof(settings.numberOfSamples) == 2, "Expected to be 2 bytes.");

    QByteArray value;
    QDataStream stream(&value, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision); // 32-bit floats, not 64-bit.
    stream << (quint8)settings.command << settings.triggerLevel << (quint8)settings.mode
           << settings.range << settings.samplingWindow << settings.numberOfSamples;

    Q_ASSERT(value.size() == 13);
    return value;
}

/*!
 * Parses the `Metadata` \a value into a DsoService::Metatdata struct.
 */
DsoService::Metadata DsoServicePrivate::parseMetadata(const QByteArray &value)
{
    DsoService::Metadata metadata{
        DsoService::DsoStatus::Error, std::numeric_limits<float>::quiet_NaN(),
        DsoService::Mode::Idle, 0, 0, 0, 0
    };

    if (!checkSize(QLatin1String("Metadata"), value, 17, 17)) {
        return metadata;
    }

    metadata.status          = static_cast<DsoService::DsoStatus>(value.at(0));
    metadata.scale           = qFromLittleEndian<float>(value.mid(1,4).constData());
    metadata.mode            = static_cast<DsoService::Mode>(value.at(5));
    metadata.range           = static_cast<quint8>(value.at(6));
    metadata.samplingWindow  = qFromLittleEndian<quint32>(value.mid(7,4).constData());
    metadata.numberOfSamples = qFromLittleEndian<quint16>(value.mid(11,2).constData());
    metadata.samplingRate    = qFromLittleEndian<quint32>(value.mid(13,4).constData());
    return metadata;
}

/*!
 * Parses the `Reading` \a value into a DsoService::Samples vector.
 */
DsoService::Samples DsoServicePrivate::parseSamples(const QByteArray &value)
{
    DsoService::Samples samples;
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
void DsoServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    AbstractPokitServicePrivate::characteristicRead(characteristic, value);

    if (characteristic.uuid() == DsoService::CharacteristicUuids::settings) {
        qCWarning(lc).noquote() << tr("Settings characteristic is write-only, but somehow read")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    Q_Q(DsoService);
    if (characteristic.uuid() == DsoService::CharacteristicUuids::metadata) {
        Q_EMIT q->metadataRead(parseMetadata(value));
        return;
    }

    if (characteristic.uuid() == DsoService::CharacteristicUuids::reading) {
        qCWarning(lc).noquote() << tr("Reading characteristic is notify-only")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic read for DSO service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void DsoServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                     const QByteArray &newValue)
{
    AbstractPokitServicePrivate::characteristicWritten(characteristic, newValue);

    Q_Q(DsoService);
    if (characteristic.uuid() == DsoService::CharacteristicUuids::settings) {
        Q_EMIT q->settingsWritten();
        return;
    }

    if (characteristic.uuid() == DsoService::CharacteristicUuids::metadata) {
        qCWarning(lc).noquote() << tr("Metadata characteristic is read/notify, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == DsoService::CharacteristicUuids::reading) {
        qCWarning(lc).noquote() << tr("Reading characteristic is notify-only, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic written for DSO service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicChanged to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void DsoServicePrivate::characteristicChanged(const QLowEnergyCharacteristic &characteristic,
                                                     const QByteArray &newValue)
{
    AbstractPokitServicePrivate::characteristicChanged(characteristic, newValue);

    Q_Q(DsoService);
    if (characteristic.uuid() == DsoService::CharacteristicUuids::settings) {
        qCWarning(lc).noquote() << tr("Settings characteristic is write-only, but somehow updated")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == DsoService::CharacteristicUuids::metadata) {
        Q_EMIT q->metadataRead(parseMetadata(newValue));
        return;
    }

    if (characteristic.uuid() == DsoService::CharacteristicUuids::reading) {
        Q_EMIT q->samplesRead(parseSamples(newValue));
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic notified for DSO service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
