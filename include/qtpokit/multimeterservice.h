// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

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

    struct QTPOKIT_EXPORT CharacteristicUuids {
        static const QBluetoothUuid settings;
        static const QBluetoothUuid reading;
    };

    enum class Mode : quint8 {
        Idle        = 0, ///< Make device idle.
        DcVoltage   = 1, ///< Measure DC voltage.
        AcVoltage   = 2, ///< Measure AC voltage.
        DcCurrent   = 3, ///< Measure DC current.
        AcCurrent   = 4, ///< Measure AC current.
        Resistance  = 5, ///< Measure resistance.
        Diode       = 6, ///< Measure diode.
        Continuity  = 7, ///< Measure continuity.
        Temperature = 8, ///< Measure temperature.
        /// \todo Pokit Pro supports capacitance too.
    };
    static QString toString(const Mode &mode);

    enum class VoltageRange : quint8 {
        _0_to_300mV  = 0,  ///< 0 to 300mV.
        _300mV_to_2V = 1,  ///< 300mV to 2V.
        _2V_to_6V    = 2,  ///< 2V to 6V.
        _6V_to_12V   = 3,  ///< 6V to 12V.
        _12V_to_30V  = 4,  ///< 12V to 30V.
        _30V_to_60V  = 5,  ///< 30V to 60V.
        /// \todo Pokit Pro supports up to 600V.
        AutoRange    = 255 ///< Auto-range.
    };
    static QString toString(const VoltageRange &range);
    static QVariant minValue(const VoltageRange &range);
    static QVariant maxValue(const VoltageRange &range);

    enum class CurrentRange : quint8 {
        _0_to_10mA      = 0,  ///< 0 to 10mA.
        _10mA_to_30mA   = 1,  ///< 10mA to 30mA.
        _30mA_to_150mA  = 2,  ///< 30mA to 150mA.
        _150mA_to_300mA = 3,  ///< 150mA to 300mA.
        _300mA_to_3A    = 4,  ///< 300mA to 3A.
        /// \todo Pokit Pro supports up to 10A.
        AutoRange       = 255 ///< Auto-range.
    };
    static QString toString(const CurrentRange &range);
    static QVariant minValue(const CurrentRange &range);
    static QVariant maxValue(const CurrentRange &range);

    enum class ResistanceRange : quint8 {
        _0_to_160     = 0,  ///< 0 to 160 ohms.
        _160_to_330   = 1,  ///< 160 to 330 ohms.
        _330_to_890   = 2,  ///< 330 to 890 ohms.
        _890_to_1K5   = 3,  ///< 890 to 1.5K ohms.
        _1K5_to_10K   = 4,  ///< 1.5K to 10K ohms.
        _10K_to_100K  = 5,  ///< 10K to 100K ohms.
        _100K_to_470K = 6,  ///< 100K to 470K ohms.
        _470K_to_1M   = 7,  ///< 470K to 1M ohms.
        /// \todo Pokit Pro supports up to 3M ohms.
        AutoRange     = 255 ///< Auto-range.
    };
    static QString toString(const ResistanceRange &range);
    static QVariant minValue(const ResistanceRange &range);
    static QVariant maxValue(const ResistanceRange &range);

    union QTPOKIT_EXPORT Range {
        VoltageRange voltageRange;      ///< Range when in AC/DC voltage mode.
        CurrentRange currentRange;      ///< Range when in AC/DC current mode.
        ResistanceRange resitanceRange; ///< Range when in resistance mode.
        Range();
        Range(const VoltageRange range);
        Range(const CurrentRange range);
        Range(const ResistanceRange range);
    };
    static QString toString(const Range &range, const Mode &mode);

    struct Settings {
        Mode mode;              ///< Desired operation mode.
        Range range;            ///< Desired range.
        quint32 updateInterval; ///< Desired update interval in milliseconds.
    };

    enum class MeterStatus : quint8 {
        AutoRangeOff = 0,  ///< Auto-range is disabled (voltage, current and resitance modes only).
        AutoRangeOn  = 1,  ///< Auto-range is enabled (voltage, current and resitance modes only).
        NoContinuity = 0,  ///< No continuity (continuity mode only).
        Continuity   = 1,  ///< Continuity (continuity mode only).
        Ok           = 0,  ///< Ok (temperature and diode modes only).
        Error        = 255 ///< Error (all modes).
    };

    struct Reading {
        MeterStatus status; ///< Current multimeter status.
        float value;        ///< Last acquired value.
        Mode mode;          ///< Current operation mode.
        Range range;        ///< Current range.
    };

    MultimeterService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~MultimeterService() override;

    bool readCharacteristics() override;
    bool readReadingCharacteristic();

    // Settings characteristic (BLE write only).
    bool setSettings(const Settings &settings);

    // Reading characteristic (BLE read/notify).
    Reading reading() const;
    bool enableReadingNotifications();
    bool disableReadingNotifications();

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

QTPOKIT_EXPORT bool operator==(const MultimeterService::Range &lhs, const MultimeterService::Range &rhs);
QTPOKIT_EXPORT bool operator!=(const MultimeterService::Range &lhs, const MultimeterService::Range &rhs);
QTPOKIT_EXPORT bool operator< (const MultimeterService::Range &lhs, const MultimeterService::Range &rhs);
QTPOKIT_EXPORT bool operator> (const MultimeterService::Range &lhs, const MultimeterService::Range &rhs);
QTPOKIT_EXPORT bool operator<=(const MultimeterService::Range &lhs, const MultimeterService::Range &rhs);
QTPOKIT_EXPORT bool operator>=(const MultimeterService::Range &lhs, const MultimeterService::Range &rhs);

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_MULTIMETERSERVICE_H
