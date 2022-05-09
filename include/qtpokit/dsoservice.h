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
 * Declares the DsoService class.
 */

#ifndef QTPOKIT_DSOSERVICE_H
#define QTPOKIT_DSOSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class DsoServicePrivate;

class QTPOKIT_EXPORT DsoService : public AbstractPokitService
{
    Q_OBJECT

public:
    static const QBluetoothUuid serviceUuid;

    struct CharacteristicUuids {
        static const QBluetoothUuid settings;
        static const QBluetoothUuid metadata;
        static const QBluetoothUuid reading;
    };

    enum class Command : quint8 {
        FreeRunning        = 0,
        RisingEdgeTrigger  = 1,
        FallingEdgeTrigger = 2,
        ResendData         = 3
    };

    enum class Mode : quint8 {
        Idle        = 0, ///< Make device idle.
        DcVoltage   = 1, ///< Measure DC voltage.
        AcVoltage   = 2, ///< Measure AC voltage.
        DcCurrent   = 3, ///< Measure DC current.
        AcCurrent   = 4, ///< Measure AC current.
    };
    static QString toString(const Mode &mode);

    enum class VoltageRange : quint8 {
        _0_to_300mV  = 0,  ///< 0 to 300mV.
        _300mV_to_2V = 1,  ///< 300mV to 2V.
        _2V_to_6V    = 2,  ///< 2V to 6V.
        _6V_to_12V   = 3,  ///< 6V to 12V.
        _12V_to_30V  = 4,  ///< 12V to 30V.
        _30V_to_60V  = 5,  ///< 30V to 60V.
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
    };
    static QString toString(const CurrentRange &range);
    static QVariant minValue(const CurrentRange &range);
    static QVariant maxValue(const CurrentRange &range);

    union Range {
        VoltageRange voltageRange; ///< Range when in AC/DC voltage mode.
        CurrentRange currentRange; ///< Range when in AC/DC current mode.
    };

    struct Settings {
        Command command;         ///< Custom operation request.
        float triggerLevel;      ///< Trigger threshold level in Volts or Amps, depending on #mode.
        Mode mode;               ///< Desired operation mode.
        Range range;             ///< Desired range.
        quint32 samplingWindow;  ///< Desired sampling window in microseconds.
        quint16 numberOfSamples; ///< Desired number of samples to acquire.
    };

    enum class DsoStatus : quint8 {
        Done       = 0,
        Sampling   = 1,
        Error      = 255
    };

    struct Metadata {
        DsoStatus status;        ///< Current DSO status.
        float scale;             ///< Scale to apply to read samples.
        Mode mode;               ///< Operation mode used during last acquisition.
        Range range;             ///< Range used during last acquisition.
        quint32 samplingWindow;  ///< Sampling window (microseconds) used during last acquisition.
        quint16 numberOfSamples; ///< Number of samples acquired (1 to 8192).
        quint32 samplingRate;    ///< Sampling rate  used during last acquisition (1 to 1MHz).
    };

    typedef QVector<qint16> Samples;

    DsoService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~DsoService() override;

    bool readCharacteristics() override;
    bool readMetadataCharacteristic();

    // Settings characteristic (BLE write only).
    bool setSettings(const Settings &settings);
    bool startDso(const Settings &settings);
    bool stopDso();
    bool fetchSamples();

    // Metadata characteristic (BLE read/notify).
    Metadata metadata() const;
    bool beginMetadata();
    bool stopMetadata();

    // Reading characteristic (BLE notify only).
    bool beginSampling();
    bool stopSampling();

signals:
    void settingsWritten();
    void metadataRead(const DsoService::Metadata &meta);
    void samplesRead(const DsoService::Samples &samples);

protected:
    /// \cond internal
    DsoService(DsoServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(DsoService)
    Q_DISABLE_COPY(DsoService)
    friend class TestDsoService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_DSOSERVICE_H
