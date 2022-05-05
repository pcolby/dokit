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
        Idle = 0,                  ///< Make device idle.
        MultimeterDcVoltage = 1,   ///< Measure DC voltage.
        MultimeterAcVoltage = 2,   ///< Measure AC voltage.
        MultimeterDcCurrent = 3,   ///< Measure DC current.
        MultimeterAcCurrent = 4,   ///< Measure AC current.
        MultimeterResistance = 5,  ///< Measure resistance.
        MultimeterDiode = 6,       ///< Measure diode.
        MultimeterContinuity = 7,  ///< Measure continuity.
        MultimeterTemperature = 8, ///< Measure temperature.
    };

    enum class VoltageRange : quint8 {
        _0_to_300mV  = 0,
        _300mV_to_2V = 1,
        _2V_to_6V    = 2,
        _6V_to_12V   = 3,
        _12V_to_30V  = 4,
        _30V_to_60V  = 5,
        AutoRange     = 255
    };

    enum class CurrentRange : quint8 {
        _0_to_10mA      = 0,
        _10mA_to_30mA   = 1,
        _30mA_to_150mA  = 2,
        _150mA_to_300mA = 3,
        _300mA_to_3A    = 4,
        AutoRange       = 255
    };

    enum class ResistanceRange : quint8 {
        _0_to_160     = 0,
        _160_to_330   = 1,
        _330_to_890   = 2,
        _890_to_1K5   = 3,
        _1K5_to_10K   = 4,
        _10K_to_100K  = 5,
        _100K_to_470K = 6,
        _470K_to_1M   = 7,
        AutoRange     = 255
    };

    union MultimeterRange {
        VoltageRange voltageRange;
        CurrentRange currentRange;
        ResistanceRange resitanceRange;
    };

    struct Settings {
        MultimeterMode mode;
        MultimeterRange range;
        quint32 updateInterval;
    };

    enum class ReadingStatusFlag : quint8 {
        AutoRange  = 0x01,
        Continuity = 0x01,
        Error      = 0xFF,
    };
    Q_DECLARE_FLAGS(ReadingStatusFlags, ReadingStatusFlag)

    struct Reading {
        ReadingStatusFlags status;
        float value;
        MultimeterMode mode;
        MultimeterRange range;
    };

    MultimeterService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~MultimeterService() override;

    bool readCharacteristics() override;
    bool readReadingCharacteristic();

    // Settings characteristic (BLE write only).
    bool setSettings(const Settings &settings);

    // Reading characteristic (BLE read/notify).
    Reading reading() const;

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
