// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the MultimeterService and MultimeterServicePrivate classes.
 */

#include <qtpokit/multimeterservice.h>
#include "multimeterservice_p.h"
#include "pokitproducts_p.h"

#include <QDataStream>
#include <QIODevice>
#include <QtEndian>

/*!
 * \class MultimeterService
 *
 * The MultimeterService class accesses the `Multimeter` service of Pokit devices.
 */

/// UUID of the "Multimeter" service.
const QBluetoothUuid MultimeterService::
    serviceUuid(QLatin1String("e7481d2f-5781-442e-bb9a-fd4e3441dadc"));

/// \struct MultimeterService::CharacteristicUuids
/// \brief Characteristics available via the `Multimeter` service.

/// UUID of the `Multimeter` service's `Settings` characterstic.
const QBluetoothUuid MultimeterService::CharacteristicUuids::
    settings(QLatin1String("53dc9a7a-bc19-4280-b76b-002d0e23b078"));

/// UUID of the `Multimeter` service's `Reading` characterstic.
const QBluetoothUuid MultimeterService::CharacteristicUuids::
    reading(QLatin1String("047d3559-8bee-423a-b229-4417fa603b90"));

/// \enum MultimeterService::Mode
/// \brief Values supported by the `Mode` attribute of the `Settings` and `Reading` characteristics.

/// Returns \a mode as a user-friendly string.
QString MultimeterService::toString(const Mode &mode)
{
    switch (mode) {
    case Mode::Idle:        return tr("Idle");
    case Mode::DcVoltage:   return tr("DC voltage");
    case Mode::AcVoltage:   return tr("AC voltage");
    case Mode::DcCurrent:   return tr("DC current");
    case Mode::AcCurrent:   return tr("AC current");
    case Mode::Resistance:  return tr("Resistance");
    case Mode::Diode:       return tr("Diode");
    case Mode::Continuity:  return tr("Continuity");
    case Mode::Temperature: return tr("Temperature");
    case Mode::Capacitance: return tr("Capacitance");
    case Mode::ExternalTemperature: return tr("External temperature");
    }
    return QString();
}

/// Returns \a range as a user-friendly string, or a null QString if \a mode has no ranges.
QString MultimeterService::toString(const PokitProduct product, const quint8 range, const Mode mode)
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
    case Mode::Resistance:
        return ResistanceRange::toString(product, range);
    case Mode::Diode:
    case Mode::Continuity:
    case Mode::Temperature:
        break;
    case Mode::Capacitance:
        return CapacitanceRange::toString(product, range);
    case Mode::ExternalTemperature:
        break;
    }
    return QString();
}

/// Returns \a range as a user-friendly string, or a null QString if \a mode has no ranges.
QString MultimeterService::toString(const quint8 range, const Mode mode) const
{
    return toString(pokitProduct(), range, mode);
}

QVariant MultimeterService::maxValue(const PokitProduct product, const quint8 range, const Mode mode)
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
    case Mode::Resistance:
        return ResistanceRange::maxValue(product, range);
    case Mode::Diode:
    case Mode::Continuity:
    case Mode::Temperature:
        break;
    case Mode::Capacitance:
        return CapacitanceRange::maxValue(product, range);
    case Mode::ExternalTemperature:
        break;
    }
    return QVariant();
}

QVariant MultimeterService::maxValue(const quint8 range, const Mode mode) const
{
    return maxValue(pokitProduct(), range, mode);
}

/// \struct MultimeterService::Settings
/// \brief Attributes included in the `Settings` characterstic.

/// \enum MultimeterService::MeterStatus
/// \brief Values supported by the `Status` attribute of the `Settings` characteristic.

/// \struct MultimeterService::Reading
/// \brief Attributes included in the `Reading` characterstic.

/*!
 * Constructs a new Pokit service with \a parent.
 */
MultimeterService::MultimeterService(QLowEnergyController * const controller, QObject * parent)
    : AbstractPokitService(new MultimeterServicePrivate(controller, this), parent)
{

}

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
MultimeterService::MultimeterService(
    MultimeterServicePrivate * const d, QObject * const parent)
    : AbstractPokitService(d, parent)
{

}
/// \endcond

/*!
 * Destroys this MultimeterService object.
 */
MultimeterService::~MultimeterService()
{

}

bool MultimeterService::readCharacteristics()
{
    return readReadingCharacteristic();
}

/*!
 * Read the `Multimeter` service's `Reading` characteristic.
 *
 * Returns `true` is the read request is succesfully queued, `false` otherwise (ie if the
 * underlying controller it not yet connected to the Pokit device, or the device's services have
 * not yet been discovered).
 *
 * Emits readingRead() if/when the characteristic has been read successfully.
 */
bool MultimeterService::readReadingCharacteristic()
{
    Q_D(MultimeterService);
    return d->readCharacteristic(CharacteristicUuids::reading);
}

/*!
 * Configures the Pokit device's multimeter mode.
 *
 * Returns `true` if the write request was successfully queued, `false` otherwise.
 *
 * Emits settingsWritten() if/when the \a settings have been writtem successfully.
 */
bool MultimeterService::setSettings(const Settings &settings)
{
    Q_D(const MultimeterService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::settings);
    if (!characteristic.isValid()) {
        return false;
    }

    const QByteArray value = MultimeterServicePrivate::encodeSettings(settings);
    if (value.isNull()) {
        return false;
    }

    d->service->writeCharacteristic(characteristic, value);
    return (d->service->error() != QLowEnergyService::ServiceError::CharacteristicWriteError);
}

/*!
 * Returns the most recent value of the `Multimeter` service's `Reading` characteristic.
 *
 * The returned value, if any, is from the underlying Bluetooth stack's cache. If no such value is
 * currently available (ie the serviceDetailsDiscovered signal has not been emitted yet), then the
 * returned MultimeterService::Reading::value member will be a quiet NaN, which can be checked like:
 *
 * ```
 * const MultimeterService::Reading reading = multimeterService->reading();
 * if (qIsNaN(reading.value)) {
 *     // Handle failure.
 * }
 * ```
 */
MultimeterService::Reading MultimeterService::reading() const
{
    Q_D(const MultimeterService);
    const QLowEnergyCharacteristic characteristic =
        d->getCharacteristic(CharacteristicUuids::reading);
    return (characteristic.isValid()) ? MultimeterServicePrivate::parseReading(characteristic.value())
        : Reading{ MeterStatus::Error, std::numeric_limits<float>::quiet_NaN(), Mode::Idle, 0 };
}

/*!
 * Enables client-side notifications of meter readings.
 *
 * This is an alternative to manually requesting individual reads via readReadingCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 *
 * Successfully read values (if any) will be emitted via the readingRead() signal.
 */
bool MultimeterService::enableReadingNotifications()
{
    Q_D(MultimeterService);
    return d->enableCharacteristicNotificatons(CharacteristicUuids::reading);
}

/*!
 * Disables client-side notifications of meter readings.
 *
 * Instantaneous reads can still be fetched by readReadingCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool MultimeterService::disableReadingNotifications()
{
    Q_D(MultimeterService);
    return d->disableCharacteristicNotificatons(CharacteristicUuids::reading);
}

/*!
 * \fn MultimeterService::readingRead
 *
 * This signal is emitted when the `Reading` characteristic has been read successfully.
 *
 * \see readReadingCharacteristic
 */

/*!
 * \fn MultimeterService::settingsWritten
 *
 * This signal is emitted when the `Settings` characteristic has been written successfully.
 *
 * \see setSettings
 */

/*!
 * \cond internal
 * \class MultimeterServicePrivate
 *
 * The MultimeterServicePrivate class provides private implementation for MultimeterService.
 */

/*!
 * \internal
 * Constructs a new MultimeterServicePrivate object with public implementation \a q.
 */
MultimeterServicePrivate::MultimeterServicePrivate(
    QLowEnergyController * controller, MultimeterService * const q)
    : AbstractPokitServicePrivate(MultimeterService::serviceUuid, controller, q)
{

}

/*!
 * Returns \a settings in the format Pokit devices expect.
 */
QByteArray MultimeterServicePrivate::encodeSettings(const MultimeterService::Settings &settings)
{
    static_assert(sizeof(settings.mode)           == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.range)          == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.updateInterval) == 4, "Expected to be 4 bytes.");

    QByteArray value;
    QDataStream stream(&value, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision); // 32-bit floats, not 64-bit.
    stream << (quint8)settings.mode << settings.range << settings.updateInterval;

    Q_ASSERT(value.size() == 6);
    return value;
}

/*!
 * Parses the `Reading` \a value into a MultimeterService::Reading struct.
 */
MultimeterService::Reading MultimeterServicePrivate::parseReading(const QByteArray &value)
{
    MultimeterService::Reading reading{
        MultimeterService::MeterStatus::Error,
        std::numeric_limits<float>::quiet_NaN(),
        MultimeterService::Mode::Idle, 0
    };

    if (!checkSize(QLatin1String("Reading"), value, 7, 7)) {
        return reading;
    }

    reading.status = MultimeterService::MeterStatus(value.at(0));
    reading.value  = qFromLittleEndian<float>(value.mid(1,4));
    reading.mode   = static_cast<MultimeterService::Mode>(value.at(5));
    reading.range  = static_cast<quint8>(value.at(6));
    return reading;
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicRead to parse \a value, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void MultimeterServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    AbstractPokitServicePrivate::characteristicRead(characteristic, value);

    Q_Q(MultimeterService);
    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::reading) {
        emit q->readingRead(parseReading(value));
        return;
    }

    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::settings) {
        qCWarning(lc).noquote() << tr("Settings characteristic is write-only, but somehow read")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic read for Multimeter service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void MultimeterServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                     const QByteArray &newValue)
{
    AbstractPokitServicePrivate::characteristicWritten(characteristic, newValue);

    Q_Q(MultimeterService);
    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::settings) {
        emit q->settingsWritten();
        return;
    }

    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::reading) {
        qCWarning(lc).noquote() << tr("Reading characteristic is read/notify, but somehow written")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic written for Multimeter service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicChanged to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void MultimeterServicePrivate::characteristicChanged(const QLowEnergyCharacteristic &characteristic,
                                                     const QByteArray &newValue)
{
    AbstractPokitServicePrivate::characteristicChanged(characteristic, newValue);

    Q_Q(MultimeterService);
    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::settings) {
        qCWarning(lc).noquote() << tr("Settings characteristic is write-only, but somehow updated")
            << serviceUuid << characteristic.name() << characteristic.uuid();
        return;
    }

    if (characteristic.uuid() == MultimeterService::CharacteristicUuids::reading) {
        emit q->readingRead(parseReading(newValue));
        return;
    }

    qCWarning(lc).noquote() << tr("Unknown characteristic notified for Multimeter service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/// \endcond
