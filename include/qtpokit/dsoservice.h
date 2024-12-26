// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
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
    /// UUID of the "DSO" service.
    static inline const QBluetoothUuid serviceUuid { QStringLiteral("1569801e-1425-4a7a-b617-a4f4ed719de6") };

    /// Characteristics available via the `DSO` service.
    struct QTPOKIT_EXPORT CharacteristicUuids {
        /// UUID of the `DSO` service's `Settings` characterstic.
        static inline const QBluetoothUuid settings { QStringLiteral("a81af1b6-b8b3-4244-8859-3da368d2be39") };

        /// UUID of the `DSO` service's `Metadata` characterstic.
        static inline const QBluetoothUuid metadata { QStringLiteral("970f00ba-f46f-4825-96a8-153a5cd0cda9") };

        /// UUID of the `DSO` service's `Reading` characterstic.
        static inline const QBluetoothUuid reading  { QStringLiteral("98e14f8e-536e-4f24-b4f4-1debfed0a99e") };
    };

    /// Values supported by the `Command` attribute of the `Settings` characteristic.
    enum class Command : quint8 {
        FreeRunning        = 0, ///< Run free, without waiting for edge triggers.
        RisingEdgeTrigger  = 1, ///< Trigger on a rising edge.
        FallingEdgeTrigger = 2, ///< Trigger on a falling edge.
        ResendData         = 3  ///< Resend the last acquired data.
    };

    /// Values supported by the `Mode` attribute of the `Settings` and `Metadata` characteristics.
    enum class Mode : quint8 {
        Idle        = 0, ///< Make device idle.
        DcVoltage   = 1, ///< Measure DC voltage.
        AcVoltage   = 2, ///< Measure AC voltage.
        DcCurrent   = 3, ///< Measure DC current.
        AcCurrent   = 4, ///< Measure AC current.
    };
    static QString toString(const Mode &mode);

    static QString toString(const PokitProduct product, const quint8 range, const Mode mode);
    QString toString(const quint8 range, const Mode mode) const;
    static quint32 maxValue(const PokitProduct product, const quint8 range, const Mode mode);
    quint32 maxValue(const quint8 range, const Mode mode) const;

    /// Attributes included in the `Settings` characterstic.
    struct Settings {
        Command command;         ///< Custom operation request.
        float triggerLevel;      ///< Trigger threshold level in Volts or Amps, depending on #mode.
        Mode mode;               ///< Desired operation mode.
        quint8 range;            ///< Desired range, eg settings.range = +PokitPro::CurrentRange::AutoRange;
        quint32 samplingWindow;  ///< Desired sampling window in microseconds.
        quint16 numberOfSamples; ///< Desired number of samples to acquire.
    };

    /// Values supported by the `Status` attribute of the `Metadata` characteristic.
    enum class DsoStatus : quint8 {
        Done       = 0,  ///< Sampling has completed.
        Sampling   = 1,  ///< Actively sampling.
        Error      = 255 ///< An error has occurred.
    };

    /// Attributes included in the `Metadata` characterstic.
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
    ~DsoService() = default;

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

Q_SIGNALS:
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
