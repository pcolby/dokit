// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
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

    struct QTPOKIT_EXPORT CharacteristicUuids {
        static const QBluetoothUuid settings;
        static const QBluetoothUuid metadata;
        static const QBluetoothUuid reading;
    };

    enum class Command : quint8 {
        Start   = 0, ///< Start the Data Logger.
        Stop    = 1, ///< Stop the Data Logger.
        Refresh = 2, ///< Refresh the Data Logger.
    };

    enum class Mode : quint8 {
        Idle        = 0, ///< Make device idle.
        DcVoltage   = 1, ///< Measure DC voltage.
        AcVoltage   = 2, ///< Measure AC voltage.
        DcCurrent   = 3, ///< Measure DC current.
        AcCurrent   = 4, ///< Measure AC current.
        Temperature = 5, ///< Measure temperature.
    };
    static QString toString(const Mode &mode);

    static QString toString(const PokitProduct product, const quint8 range, const Mode mode);
    QString toString(const quint8 range, const Mode mode) const;
    static QVariant maxValue(const PokitProduct product, const quint8 range, const Mode mode);
    QVariant maxValue(const quint8 range, const Mode mode) const;

    struct Settings {
        Command command;        ///< Custom operation request.
        quint16 arguments;      ///< Reserved to used along with #command in future.
        Mode mode;              ///< Desired operation mode.
        quint8 range;           ///< Desired range.
        quint32 updateInterval; ///< Desired update interval in milliseconds.
        quint32 timestamp;      ///< Custom timestamp for start time in retrieved metadata.
    };

    enum class LoggerStatus : quint8 {
        Done       = 0,   ///< Sampling has completed.
        Sampling   = 1,   ///< Actively sampling.
        BufferFull = 2,   ///< Buffer is full.
        Error      = 255  ///< An error has occurred.
    };

    struct Metadata {
        LoggerStatus status;     ///< Current data logger status.
        float scale;             ///< Scale to apply to read samples.
        Mode mode;               ///< Current operation mode.
        quint8 range;            ///< Current range.
        quint32 updateInterval;  ///< Current logging interval in milliseconds.
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
    bool disableMetadataNotifications();

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
