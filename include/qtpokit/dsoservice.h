// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the DsoService class.
 */

#ifndef QTPOKIT_DSOSERVICE_H
#define QTPOKIT_DSOSERVICE_H

#include "abstractpokitservice.h"
#include "pokitproducts.h"

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

    struct QTPOKIT_EXPORT CharacteristicUuids {
        static const QBluetoothUuid settings;
        static const QBluetoothUuid metadata;
        static const QBluetoothUuid reading;
    };

    enum class Command : quint8 {
        FreeRunning        = 0, ///< Run free, without waiting for edge triggers.
        RisingEdgeTrigger  = 1, ///< Trigger on a rising edge.
        FallingEdgeTrigger = 2, ///< Trigger on a falling edge.
        ResendData         = 3  ///< Resend the last acquired data.
    };

    enum class Mode : quint8 {
        Idle        = 0, ///< Make device idle.
        DcVoltage   = 1, ///< Measure DC voltage.
        AcVoltage   = 2, ///< Measure AC voltage.
        DcCurrent   = 3, ///< Measure DC current.
        AcCurrent   = 4, ///< Measure AC current.
    };
    static QString toString(const Mode &mode);

//    union QTPOKIT_EXPORT Range {
//        PokitCurrentRange currentRange; ///< Range when in AC/DC current mode.
//        PokitVoltageRange voltageRange; ///< Range when in AC/DC voltage mode.
//        Range(const PokitCurrentRange range);
//        Range(const PokitVoltageRange range);
//    };
//    static QString toString(const Range &range, const Mode &mode);
    static QString toString(const PokitProduct product, const quint8 range, const Mode mode);
    QString toString(const quint8 range, const Mode mode);

    struct Settings {
        Command command;         ///< Custom operation request.
        float triggerLevel;      ///< Trigger threshold level in Volts or Amps, depending on #mode.
        Mode mode;               ///< Desired operation mode.
        quint8 range;            ///< Desired range, eg settings.range = +PokitPro::CurrentRange::AutoRange;
        quint32 samplingWindow;  ///< Desired sampling window in microseconds.
        quint16 numberOfSamples; ///< Desired number of samples to acquire.
    };

    enum class DsoStatus : quint8 {
        Done       = 0,  ///< Sampling has completed.
        Sampling   = 1,  ///< Actively sampling.
        Error      = 255 ///< An error has occurred.
    };

    struct Metadata {
        DsoStatus status;        ///< Current DSO status.
        float scale;             ///< Scale to apply to read samples.
        Mode mode;               ///< Operation mode used during last acquisition.
        quint8 range;            ///< Range used during last acquisition.
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
    bool fetchSamples();

    // Metadata characteristic (BLE read/notify).
    Metadata metadata() const;
    bool enableMetadataNotifications();
    bool disableMetadataNotifications();

    // Reading characteristic (BLE notify only).
    bool enableReadingNotifications();
    bool disableReadingNotifications();

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
