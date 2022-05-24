// SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the DataLoggerService class.
 */

#ifndef QTPOKIT_DATALOGGERSERVICE_H
#define QTPOKIT_DATALOGGERSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class DataLoggerServicePrivate;

class QTPOKIT_EXPORT DataLoggerService : public AbstractPokitService
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
        Start   = 0,
        Stop    = 1,
        Refresh = 2,
    };

    enum class Mode : quint8 {
        Idle        = 0, ///< Make device idle.
        DcVoltage   = 1, ///< Measure DC voltage.
        AcVoltage   = 2, ///< Measure AC voltage.
        DcCurrent   = 3, ///< Measure DC current.
        AcCurrent   = 4, ///< Measure AC current.
        /// \todo Pokit Pro supports temperature in logger mode too.
    };
    static QString toString(const Mode &mode);

    enum class VoltageRange : quint8 {
        _0_to_300mV  = 0,  ///< 0 to 300mV.
        _300mV_to_2V = 1,  ///< 300mV to 2V.
        _2V_to_6V    = 2,  ///< 2V to 6V.
        _6V_to_12V   = 3,  ///< 6V to 12V.
        _12V_to_30V  = 4,  ///< 12V to 30V.
        _30V_to_60V  = 5,  ///< 30V to 60V.
        /// \todo Pokit Pro supports up to 600V, which are enum 6 and 7 via Android app.
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
    };
    static QString toString(const CurrentRange &range);
    static QVariant minValue(const CurrentRange &range);
    static QVariant maxValue(const CurrentRange &range);

    union Range {
        VoltageRange voltageRange; ///< Range when in AC/DC voltage mode.
        CurrentRange currentRange; ///< Range when in AC/DC current mode.
    };
    static QString toString(const Range &range, const Mode &mode);

    struct Settings {
        Command command;        ///< Custom operation request.
        quint16 arguments;      ///< Reserved to used along with #command in future.
        Mode mode;              ///< Desired operation mode.
        Range range;            ///< Desired range.
        quint16 updateInterval; ///< Desired update interval in seconds.
        quint32 timestamp;      ///< Custom timestamp for start time in retrieved metadata.
    };

    enum class LoggerStatus : quint8 {
        Done       = 0,
        Sampling   = 1,
        BufferFull = 2,
        Error      = 255
    };

    struct Metadata {
        LoggerStatus status;     ///< Current data logger status.
        float scale;             ///< Scale to apply to read samples.
        Mode mode;               ///< Current operation mode.
        Range range;             ///< Current range.
        quint32 updateInterval;  ///< Current logging interval.
        quint16 numberOfSamples; ///< Number of samples acquired (1 to 6192).
        quint32 timestamp;       ///< Timestamp stored at the beginning of the logging session.
    };

    typedef QVector<qint16> Samples;

    DataLoggerService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~DataLoggerService() override;

    bool readCharacteristics() override;
    bool readMetadataCharacteristic();

    // Settings characteristic (BLE write only).
    bool setSettings(const Settings &settings);
    bool startLogger(const Settings &settings);
    bool stopLogger();
    bool fetchSamples();

    // Metadata characteristic (BLE read/notify).
    Metadata metadata() const;
    bool enableMetadataNotifications();
    bool disableMetadatNotifications();

    // Reading characteristic (BLE notify only).
    bool enableReadingNotifications();
    bool disableReadingNotifications();

signals:
    void settingsWritten();
    void metadataRead(const DataLoggerService::Metadata &meta);
    void samplesRead(const DataLoggerService::Samples &samples);

protected:
    /// \cond internal
    DataLoggerService(DataLoggerServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(DataLoggerService)
    Q_DISABLE_COPY(DataLoggerService)
    friend class TestDataLoggerService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_DATALOGGERSERVICE_H
