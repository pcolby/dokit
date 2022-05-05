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
 * Declares the MultimeterService class.
 */

#ifndef QTPOKIT_MULTIMETERSERVICE_H
#define QTPOKIT_MULTIMETERSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class MultimeterServicePrivate;

class QTPOKIT_EXPORT MultimeterService : public AbstractPokitService
{
    Q_OBJECT

public:
    static const QBluetoothUuid serviceUuid;

    struct CharacteristicUuids {
        static const QBluetoothUuid settings;
        static const QBluetoothUuid reading;
    };

    enum class MultimeterMode : quint8 {
        Idle = 0,        ///< Make device idle.
        DcVoltage = 1,   ///< Measure DC voltage.
        AcVoltage = 2,   ///< Measure AC voltage.
        DcCurrent = 3,   ///< Measure DC current.
        AcCurrent = 4,   ///< Measure AC current.
        Resistance = 5,  ///< Measure resistance.
        Diode = 6,       ///< Measure diode.
        Continuity = 7,  ///< Measure continuity.
        Temperature = 8, ///< Measure temperature.
    };

    enum class VoltageRange : quint8 {
        _0_to_300mV  = 0,  ///< 0 to 300mV.
        _300mV_to_2V = 1,  ///< 300mV to 2V.
        _2V_to_6V    = 2,  ///< 2V to 6V.
        _6V_to_12V   = 3,  ///< 6V to 12V.
        _12V_to_30V  = 4,  ///< 12V to 30V.
        _30V_to_60V  = 5,  ///< 30V to 60V.
        AutoRange    = 255 ///< Auto-range.
    };

    enum class CurrentRange : quint8 {
        _0_to_10mA      = 0,  ///< 0 to 10mA.
        _10mA_to_30mA   = 1,  ///< 10mA to 30mA.
        _30mA_to_150mA  = 2,  ///< 30mA to 150mA.
        _150mA_to_300mA = 3,  ///< 150mA to 300mA.
        _300mA_to_3A    = 4,  ///< 300mA to 3A.
        AutoRange       = 255 ///< Auto-range.
    };

    enum class ResistanceRange : quint8 {
        _0_to_160     = 0,  ///< 0 to 160 ohms.
        _160_to_330   = 1,  ///< 160 to 330 ohms.
        _330_to_890   = 2,  ///< 330 to 890 ohms.
        _890_to_1K5   = 3,  ///< 890 to 1.5K ohms.
        _1K5_to_10K   = 4,  ///< 1.5K to 10K ohms.
        _10K_to_100K  = 5,  ///< 10K to 100K ohms.
        _100K_to_470K = 6,  ///< 100K to 470K ohms.
        _470K_to_1M   = 7,  ///< 470K to 1M ohms.
        AutoRange     = 255 ///< Auto-range.
    };

    union MultimeterRange {
        VoltageRange voltageRange;      ///< Range when in AC/DC voltage mode.
        CurrentRange currentRange;      ///< Range when in AC/DC current mode.
        ResistanceRange resitanceRange; ///< Range when in resistance mode.
    };

    struct Settings {
        MultimeterMode mode;    ///< Desired operation mode.
        MultimeterRange range;  ///< Desired range.
        quint32 updateInterval; ///< Desired update interval in milliseconds.
    };

    enum class ReadingStatusFlag : quint8 {
        AutoRange  = 0x01,
        Continuity = 0x01,
        Error      = 0xFF,
    };
    Q_DECLARE_FLAGS(ReadingStatusFlags, ReadingStatusFlag)

    struct Reading {
        ReadingStatusFlags status; ///< Current multimeter status.
        float value;               ///< Last acquired value.
        MultimeterMode mode;       ///< Current operation mode.
        MultimeterRange range;     ///< Current range.
    };

    MultimeterService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~MultimeterService() override;

    bool readCharacteristics() override;
    bool readReadingCharacteristic();

    // Settings characteristic (BLE write only).
    bool setSettings(const Settings &settings);

    // Reading characteristic (BLE read/notify).
    Reading reading() const;
    bool beginClientReadings();
    bool stopClientReadings();

    /// \todo How to handle BLE 'notify'.

signals:
    void settingsWritten();
    void readingRead(const MultimeterService::Reading &reading);

protected:
    /// \cond internal
    MultimeterService(MultimeterServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(MultimeterService)
    Q_DISABLE_COPY(MultimeterService)
    friend class TestMultimeterService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_MULTIMETERSERVICE_H
