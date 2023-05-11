// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
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
        Capacitance = 9, ///< Measure capacitance.
        ExternalTemperature = 10, ///< Measure temperature via an external temperature probe.
    };
    static QString toString(const Mode &mode);

    static QString toString(const PokitProduct product, const quint8 range, const Mode mode);
    QString toString(const quint8 range, const Mode mode) const;
    static QVariant maxValue(const PokitProduct product, const quint8 range, const Mode mode);
    QVariant maxValue(const quint8 range, const Mode mode) const;

    struct Settings {
        Mode mode;              ///< Desired operation mode.
        quint8 range;           ///< Desired range.
        quint32 updateInterval; ///< Desired update interval in milliseconds.
    };

    enum class MeterStatus : quint8 {
        AutoRangeOff = 0,  ///< Auto-range is disabled (voltage, current and resistance modes only).
        AutoRangeOn  = 1,  ///< Auto-range is enabled (voltage, current and resistance modes only).
        NoContinuity = 0,  ///< No continuity (continuity mode only).
        Continuity   = 1,  ///< Continuity (continuity mode only).
        Ok           = 0,  ///< Ok (temperature and diode modes only).
        Error        = 255 ///< Error (all modes).
    };

    struct Reading {
        MeterStatus status; ///< Current multimeter status.
        float value;        ///< Last acquired value.
        Mode mode;          ///< Current operation mode.
        quint8 range;       ///< Current range.
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

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_MULTIMETERSERVICE_H
