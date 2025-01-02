// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the PokitDevicePrivate class.
 */

#ifndef QTPOKIT_POKITDEVICE_P_H
#define QTPOKIT_POKITDEVICE_P_H

#include <qtpokit/qtpokit_global.h>

#include <QLoggingCategory>
#include <QLowEnergyController>
#include <QLowEnergyConnectionParameters>
#include <QMutex>
#include <QObject>

QTPOKIT_BEGIN_NAMESPACE

class CalibrationService;
class DataLoggerService;
class DeviceInfoService;
class DsoService;
class MultimeterService;
class StatusService;

class PokitDevice;

class QTPOKIT_EXPORT PokitDevicePrivate : public QObject
{
    Q_OBJECT

public:
    static Q_LOGGING_CATEGORY(lc, "pokit.ble.controller", QtInfoMsg); ///< Logging category.

    QLowEnergyController * controller { nullptr };    ///< BLE controller for accessing the Pokit device.

    CalibrationService * calibration { nullptr };     ///< Calibration service for this Pokit device.
    DataLoggerService * dataLogger { nullptr };       ///< Data Logger service for this Pokit device.
    DeviceInfoService * deviceInfo { nullptr };       ///< Device Info service for this Pokit device.
    DsoService * dso { nullptr };                     ///< DSO service for this Pokit device.
    MultimeterService * multimeter { nullptr };       ///< Multimeter service for this Pokit device.
    StatusService * status { nullptr };               ///< Status service for this Pokit device.

    QMutex calibrationMutex;   ///< Mutex for protecting access to #calibration.
    QMutex dataLoggerMutex;    ///< Mutex for protecting access to #dataLogger.
    QMutex deviceInfoMutex;    ///< Mutex for protecting access to #deviceInfo.
    QMutex dsoMutex;           ///< Mutex for protecting access to #dso.
    QMutex multimeterMutex;    ///< Mutex for protecting access to #multimeter.
    QMutex statusMutex;        ///< Mutex for protecting access to #status.

    explicit PokitDevicePrivate(PokitDevice * const q);

    void setController(QLowEnergyController * newController);

public Q_SLOTS:
    void connected() const;
    void connectionUpdated(const QLowEnergyConnectionParameters &newParameters) const;
    void disconnected() const;
    void discoveryFinished() const;
    void errorOccurred(QLowEnergyController::Error newError) const;
    void serviceDiscovered(const QBluetoothUuid &newService) const;
    void stateChanged(QLowEnergyController::ControllerState state) const;

protected:
    PokitDevice * q_ptr; ///< Internal q-pointer.

private:
    Q_DECLARE_PUBLIC(PokitDevice)
    Q_DISABLE_COPY(PokitDevicePrivate)
    QTPOKIT_BEFRIEND_TEST(PokitDevice)
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITDEVICE_P_H
