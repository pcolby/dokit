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
 * Defines the MultimeterService and MultimeterServicePrivate classes.
 */

#include <qtpokit/multimeterservice.h>
#include "multimeterservice_p.h"

#include <QDataStream>
#include <QIODevice>
#include <QtEndian>

/*!
 * \class MultimeterService
 *
 * The MultimeterService class accesses the `Pokit Status` service of Pokit devices.
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
    default:                return QString();
    }
}

/// \enum MultimeterService::VoltageRange
/// \brief Values supported by the `Range` attribute of the `Settings` and `Reading` characteristics,
/// when `Mode` is AC or DC voltage.

/// Returns \a range as a user-friendly string.
QString MultimeterService::toString(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_0_to_300mV:  return tr("0 to 300mV");
    case VoltageRange::_300mV_to_2V: return tr("300mV to 2V");
    case VoltageRange::_2V_to_6V:    return tr("2V to 6V");
    case VoltageRange::_6V_to_12V:   return tr("6V to 12V");
    case VoltageRange::_12V_to_30V:  return tr("12V to 30V");
    case VoltageRange::_30V_to_60V:  return tr("30V to 60V");
    case VoltageRange::AutoRange:    return tr("Auto-range");
    default:                         return QString();
    }
}

/*!
 *  Returns the minimum value for \a range in (integer) millivolts, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 *
 *  Note, this is an *absolute* minimum. That is, the true range for DC measurements is from
 *  `-maxValue(range)` to `+maxValue(range)`. In this sense, `minValue(range)` indicates the
 *  magnitude (ignore signs) that can be measured accurately for the given \a range. As AC voltage
 *  can never be negative, this is relevant for DC voltage only.
 */
QVariant MultimeterService::minValue(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_0_to_300mV:  return     0;
    case VoltageRange::_300mV_to_2V: return   300;
    case VoltageRange::_2V_to_6V:    return  2000;
    case VoltageRange::_6V_to_12V:   return  6000;
    case VoltageRange::_12V_to_30V:  return 12000;
    case VoltageRange::_30V_to_60V:  return 30000;
    case VoltageRange::AutoRange:    return tr("Auto");
    default:                         return QVariant();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) millivolts, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 */
QVariant MultimeterService::maxValue(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_0_to_300mV:  return   300;
    case VoltageRange::_300mV_to_2V: return  2000;
    case VoltageRange::_2V_to_6V:    return  6000;
    case VoltageRange::_6V_to_12V:   return 12000;
    case VoltageRange::_12V_to_30V:  return 30000;
    case VoltageRange::_30V_to_60V:  return 60000;
    case VoltageRange::AutoRange:    return tr("Auto");
    default:                         return QVariant();
    }
}

/// \enum MultimeterService::CurrentRange
/// \brief Values supported by the `Range` attribute of the `Settings` and `Reading` characteristics,
/// when `Mode` is AC or DC current.

/// Returns \a range as a user-friendly string.
QString MultimeterService::toString(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_0_to_10mA:      return tr("0 to 10mA");
    case CurrentRange::_10mA_to_30mA:   return tr("10mA to 30mA");
    case CurrentRange::_30mA_to_150mA:  return tr("30mA to 150mA");
    case CurrentRange::_150mA_to_300mA: return tr("150mA to 300mA");
    case CurrentRange::_300mA_to_3A:    return tr("300mA to 3A");
    case CurrentRange::AutoRange:       return tr("Auto-range");
    default:                            return QString();
    }
}

/*!
 *  Returns the minimum value for \a range in (integer) milliamps, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 *
 *  Note, this is an *absolute* minimum. That is, the true range for DC measurements is from
 *  `-maxValue(range)` to `+maxValue(range)`. In this sense, `minValue(range)` indicates the
 *  magnitude (ignore signs) that can be measured accurately for the given \a range. As AC current
 *  can never be negative, this is relevant for DC current only.
 */
QVariant MultimeterService::minValue(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_0_to_10mA:      return   0;
    case CurrentRange::_10mA_to_30mA:   return  10;
    case CurrentRange::_30mA_to_150mA:  return  30;
    case CurrentRange::_150mA_to_300mA: return 150;
    case CurrentRange::_300mA_to_3A:    return 300;
    case CurrentRange::AutoRange:       return tr("Auto");
    default:                            return QVariant();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) milliamps, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 */
QVariant MultimeterService::maxValue(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_0_to_10mA:      return   10;
    case CurrentRange::_10mA_to_30mA:   return   30;
    case CurrentRange::_30mA_to_150mA:  return  150;
    case CurrentRange::_150mA_to_300mA: return  300;
    case CurrentRange::_300mA_to_3A:    return 3000;
    case CurrentRange::AutoRange:       return tr("Auto");
    default:                            return QVariant();
    }
}

/// \enum MultimeterService::ResistanceRange
/// \brief Values supported by the `Range` attribute of the `Settings` and `Reading` characteristics,
/// when `Mode` is resitance.

/// Returns \a range as a user-friendly string.
QString MultimeterService::toString(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_0_to_160:     return tr("0 to 160 ohms");
    case ResistanceRange::_160_to_330:   return tr("160 to 330 ohms");
    case ResistanceRange::_330_to_890:   return tr("330 to 890 ohms");
    case ResistanceRange::_890_to_1K5:   return tr("890 to 1.5K ohms");
    case ResistanceRange::_1K5_to_10K:   return tr("1.5K to 10K ohms");
    case ResistanceRange::_10K_to_100K:  return tr("10K to 100K ohms");
    case ResistanceRange::_100K_to_470K: return tr("100K to 470K ohms");
    case ResistanceRange::_470K_to_1M:   return tr("470K to 1M ohms");
    case ResistanceRange::AutoRange:     return tr("Auto-range");
    default:                             return QString();
    }
}

/*!
 *  Returns the minimum value for \a range in (integer) ohms, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 */
QVariant MultimeterService::minValue(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_0_to_160:     return      0;
    case ResistanceRange::_160_to_330:   return    160;
    case ResistanceRange::_330_to_890:   return    330;
    case ResistanceRange::_890_to_1K5:   return    890;
    case ResistanceRange::_1K5_to_10K:   return   1500;
    case ResistanceRange::_10K_to_100K:  return  10000;
    case ResistanceRange::_100K_to_470K: return 100000;
    case ResistanceRange::_470K_to_1M:   return 470000;
    case ResistanceRange::AutoRange:     return tr("Auto");
    default:                             return QVariant();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) ohms, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 */
QVariant MultimeterService::maxValue(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_0_to_160:     return     160;
    case ResistanceRange::_160_to_330:   return     330;
    case ResistanceRange::_330_to_890:   return     890;
    case ResistanceRange::_890_to_1K5:   return    1500;
    case ResistanceRange::_1K5_to_10K:   return   10000;
    case ResistanceRange::_10K_to_100K:  return  100000;
    case ResistanceRange::_100K_to_470K: return  470000;
    case ResistanceRange::_470K_to_1M:   return 1000000;
    case ResistanceRange::AutoRange:     return tr("Auto");
    default:                             return QVariant();
    }
}

/// \union MultimeterService::Range
/// \brief Values supported by the `Range` attribute of the `Settings` characteristic.

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
    delete d_ptr;
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

    static_assert(sizeof(settings.mode)           == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.range)          == 1, "Expected to be 1 byte.");
    static_assert(sizeof(settings.updateInterval) == 4, "Expected to be 4 bytes.");

    QByteArray value;
    QDataStream stream(&value, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision); // 32-bit floats, not 64-bit.
    stream << (quint8)settings.mode << (quint8)settings.range.voltageRange << settings.updateInterval;
    Q_ASSERT(value.size() == 6);
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
        : Reading{ MeterStatus::Error, std::numeric_limits<float>::quiet_NaN(),
                   Mode::Idle, { VoltageRange::AutoRange } };
}

/*!
 * Requests the Pokit device to begin reporting multimeter reads.
 *
 * This is an alternative to manually requesting individual reads via readReadingCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 *
 * Successfully read values (if any) will be emitted via the readingRead() signal.
 */
bool MultimeterService::beginClientReadings()
{
    Q_D(MultimeterService);
    return d->enableCharacteristicNotificatons(CharacteristicUuids::reading);
}

/*!
 * Requests the Pokit device to stop reporting multimeter reads.
 *
 * Instantaneous reads can still be fetched by readReadingCharacteristic().
 *
 * Returns `true` is the request was successfully submited to the device queue, `false` otherwise.
 */
bool MultimeterService::stopClientReadings()
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
 * Parses the `Reading` \a value into a MultimeterService::Reading struct.
 */
MultimeterService::Reading MultimeterServicePrivate::parseReading(const QByteArray &value)
{
    MultimeterService::Reading reading{
        MultimeterService::MeterStatus::Error,
        std::numeric_limits<float>::quiet_NaN(),
        MultimeterService::Mode::Idle,
        { MultimeterService::VoltageRange::AutoRange }
    };

    /// \todo Abstract out this style of min/max size checking.
    if (value.size() < 7) {
        qCWarning(lc).noquote() << tr("Invalid reading size %1 for value: 0x%2")
            .arg(value.size()).arg(QLatin1String(value.toHex()));
        return reading;
    }
    if (value.size() > 7) {
        qCWarning(lc).noquote() << tr("Reading has %1 extra bytes: 0x%2")
            .arg(value.size()-7).arg(QLatin1String(value.mid(7).toHex()));
    }

    reading.status = MultimeterService::MeterStatus(value.at(0));
    reading.value = qFromLittleEndian<float>(value.mid(1,4));
    reading.mode = static_cast<MultimeterService::Mode>(value.at(5));
    reading.range.voltageRange = static_cast<MultimeterService::VoltageRange>(value.at(6));
    return reading;
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicRead to parse \a value, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void MultimeterServicePrivate::characteristicRead(const QLowEnergyCharacteristic &characteristic,
                                              const QByteArray &value)
{
    qCDebug(lc).noquote() << tr("Read  characteristic \"%1\" (%2) of size %3: 0x%4")
        .arg(characteristic.name(), characteristic.uuid().toString()).arg(value.size())
        .arg(QLatin1String(value.toHex()));

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

    qCWarning(lc).noquote() << tr("Unknown characteristic read for Status Service")
        << serviceUuid << characteristic.name() << characteristic.uuid();
}

/*!
 * Implements AbstractPokitServicePrivate::characteristicWritten to parse \a newValue, then emit a
 * specialised signal, for each supported \a characteristic.
 */
void MultimeterServicePrivate::characteristicWritten(const QLowEnergyCharacteristic &characteristic,
                                                     const QByteArray &newValue)
{
    qCDebug(lc).noquote() << tr("Characteristic \"%1\" (%2) written, with new value:")
        .arg(characteristic.name(), characteristic.uuid().toString()) << newValue.toHex();

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
    qCDebug(lc).noquote() << tr("Characteristic \"%1\" (%2) changed, with new value:")
        .arg(characteristic.name(), characteristic.uuid().toString()) << newValue.toHex();

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
