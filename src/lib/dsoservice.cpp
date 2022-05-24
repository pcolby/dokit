// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the DsoService and DsoServicePrivate classes.
 */

#include <qtpokit/dsoservice.h>
#include "dsoservice_p.h"

#include <QDataStream>
#include <QIODevice>
#include <QtEndian>

/*!
 * \class DsoService
 *
 * The DsoService class accesses the `Pokit Status` service of Pokit devices.
 */

/// UUID of the "DSO" service.
const QBluetoothUuid DsoService::
    serviceUuid(QLatin1String("1569801e-1425-4a7a-b617-a4f4ed719de6"));

/// \struct DsoService::CharacteristicUuids
/// \brief Characteristics available via the `DSO` service.

/// UUID of the `DSO` service's `Settings` characterstic.
const QBluetoothUuid DsoService::CharacteristicUuids::
    settings(QLatin1String("a81af1b6-b8b3-4244-8859-3da368d2be39"));

/// UUID of the `DSO` service's `Metadata` characterstic.
const QBluetoothUuid DsoService::CharacteristicUuids::
    metadata(QLatin1String("970f00ba-f46f-4825-96a8-153a5cd0cda9"));

/// UUID of the `DSO` service's `Reading` characterstic.
const QBluetoothUuid DsoService::CharacteristicUuids::
    reading(QLatin1String("98e14f8e-536e-4f24-b4f4-1debfed0a99e"));

/// \enum DsoService::Command
/// \brief Values supported by the `Command` attribute of the `Settings` characteristic.

/// \enum DsoService::Mode
/// \brief Values supported by the `Mode` attribute of the `Settings` and `Metadata` characteristics.

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

/// \enum DsoService::VoltageRange
/// \brief Values supported by the `Range` attribute of the `Settings` and `Metadata` characteristics,
/// when `Mode` is AC or DC voltage.

/// Returns \a range as a user-friendly string.
QString DsoService::toString(const VoltageRange &range)
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
QVariant DsoService::minValue(const VoltageRange &range)
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
QVariant DsoService::maxValue(const VoltageRange &range)
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

/// \enum DsoService::CurrentRange
/// \brief Values supported by the `Range` attribute of the `Settings` and `Metadata` characteristics,
/// when `Mode` is AC or DC current.

/// Returns \a range as a user-friendly string.
QString DsoService::toString(const CurrentRange &range)
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
QVariant DsoService::minValue(const CurrentRange &range)
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
QVariant DsoService::maxValue(const CurrentRange &range)
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

/// \union DsoService::Range
/// \brief Values supported by the `Range` attribute of the `Settings` and `Metadata` characteristics.

/// Returns \a range as a user-friendly string, or a null QString if \a mode has no ranges.
QString DsoService::toString(const Range &range, const Mode &mode)
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

/// \struct DsoService::Settings
/// \brief Attributes included in the `Settings` characterstic.

/// \enum DsoService::DsoStatus
/// \brief Values supported by the `Status` attribute of the `Metadata` characteristic.

/// \struct DsoService::Metadata
/// \brief Attributes included in the `Metadata` characterstic.

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

/*!
 * Destroys this DsoService object.
 */
DsoService::~DsoService()
{

}

bool DsoService::readCharacteristics()
{
    return readMetadataCharacteristic();
}

/*!
 * Read the `DSO` service's `Metadat` characteristic.
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
           << (quint8)settings.range.voltageRange << settings.samplingWindow
           << settings.numberOfSamples;
    Q_ASSERT(value.size() == 13);
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
    if (settings.command != DsoService::Command::ResendData) {
        qCWarning(d->lc).noquote() << tr("Settings command must not be 'ResendData'.");
        return false;
    }
    return setSettings(settings);
}

/*!
 * Start the DSO.
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
    return setSettings({
        DsoService::Command::ResendData,
        0, DsoService::Mode::Idle,
        { DsoService::VoltageRange::_0_to_300mV }, 0, 0
    });
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
        : Metadata{ DsoStatus::Error, std::numeric_limits<float>::quiet_NaN(),
                    Mode::Idle, { VoltageRange::_0_to_300mV }, 0, 0, 0 };
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
 * Instantaneous reads can still be fetched by readMetadatCharacteristic().
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
 * Parses the `Metadata` \a value into a DsoService::Metatdata struct.
 */
DsoService::Metadata DsoServicePrivate::parseMetadata(const QByteArray &value)
{
    DsoService::Metadata metadata{
        DsoService::DsoStatus::Error, std::numeric_limits<float>::quiet_NaN(),
        DsoService::Mode::Idle, { DsoService::VoltageRange::_0_to_300mV },
        0, 0, 0
    };

    if (!checkSize(QLatin1String("Metadata"), value, 17, 17)) {
        return metadata;
    }

    metadata.status             = static_cast<DsoService::DsoStatus>(value.at(0));
    metadata.scale              = qFromLittleEndian<float>(value.mid(1,4));
    metadata.mode               = static_cast<DsoService::Mode>(value.at(5));
    metadata.range.voltageRange = static_cast<DsoService::VoltageRange>(value.at(6));
    metadata.samplingWindow     = qFromLittleEndian<quint32>(value.mid(7,4));
    metadata.numberOfSamples    = qFromLittleEndian<quint16>(value.mid(11,2));
    metadata.samplingRate       = qFromLittleEndian<quint32>(value.mid(13,4));
    return metadata;
}

/*!
 * Parses the `Reading` \a value into a DsoService::Samples vector.
 */
DsoService::Samples DsoServicePrivate::parseSamples(const QByteArray &value)
{
    DsoService::Samples samples;
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
        emit q->metadataRead(parseMetadata(value));
        return;
    }

    if (characteristic.uuid() == DsoService::CharacteristicUuids::reading) {
        qCWarning(lc).noquote() << tr("Reading characteristic is notify-only")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic read for DSO Service")
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
        emit q->settingsWritten();
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
        emit q->metadataRead(parseMetadata(newValue));
        return;
    }

    if (characteristic.uuid() == DsoService::CharacteristicUuids::reading) {
        emit q->samplesRead(parseSamples(newValue));
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic notified for DSO service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
