// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the PokitDevice class.
 */

#ifndef QTPOKIT_POKITDEVICE_H
#define QTPOKIT_POKITDEVICE_H

#include "qtpokit_global.h"

#include <QBluetoothDeviceInfo>
#include <QObject>

class QLowEnergyController;

QTPOKIT_BEGIN_NAMESPACE

class CalibrationService;
class DataLoggerService;
class DeviceInfoService;
class DsoService;
class GenericAccessService;
class MultimeterService;
class StatusService;

class PokitDevicePrivate;

class QTPOKIT_EXPORT PokitDevice : public QObject
{
    Q_OBJECT

public:
    explicit PokitDevice(const QBluetoothDeviceInfo &deviceInfo, QObject * parent = nullptr);
    explicit PokitDevice(QLowEnergyController * controller, QObject * parent = nullptr);
    virtual ~PokitDevice();

    QLowEnergyController * controller();
    const QLowEnergyController * controller() const;

    CalibrationService * calibration();
    DataLoggerService * dataLogger();
    DeviceInfoService * deviceInformation();
    DsoService * dso();
    GenericAccessService * genericAccess();
    MultimeterService * multimeter();
    StatusService * status();

    static QString serviceToString(const QBluetoothUuid &uuid);
    static QString charcteristicToString(const QBluetoothUuid &uuid);

public slots:

signals:

protected:
    /// \cond internal
    PokitDevicePrivate * d_ptr; ///< Internal d-pointer.
    PokitDevice(PokitDevicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(PokitDevice)
    Q_DISABLE_COPY(PokitDevice)
    friend class TestPokitDevice;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITDEVICE_H
