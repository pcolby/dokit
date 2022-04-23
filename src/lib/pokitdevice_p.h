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
 * Declares the PokitDevicePrivate class.
 */

#ifndef QTPOKIT_POKITDEVICE_P_H
#define QTPOKIT_POKITDEVICE_P_H

#include <qtpokit/qtpokit_global.h>

#include <QLowEnergyController>
#include <QLowEnergyConnectionParameters>
#include <QMutex>
#include <QObject>

QTPOKIT_BEGIN_NAMESPACE

class DataLoggerService;
class InfoService;
class DsoService;
class GenericAccessService;
class MultimeterService;
class StatusService;

class PokitDevice;

class QTPOKIT_EXPORT PokitDevicePrivate : public QObject
{
    Q_OBJECT

public:
    QLowEnergyController * controller; ///< BLE controller for accessing the Pokit device.

    DataLoggerService * dataLogger;       ///< Data Logger service for this Pokit device.
    InfoService * deviceInfo;             ///< Device Info service for this Pokit device.
    DsoService * dso;                     ///< DSO service for this Pokit device.
    GenericAccessService * genericAccess; ///< Generic Access service for this Pokit device.
    MultimeterService * multimeter;       ///< Multimeter service for this Pokit device.
    StatusService * status;               ///< Status service for this Pokit device.

    QMutex dataLoggerMutex;    ///< Mutex for protecting access to #dataLogger.
    QMutex deviceInfoMutex;    ///< Mutex for protecting access to #deviceInformation.
    QMutex dsoMutex;           ///< Mutex for protecting access to #dso.
    QMutex genericAccessMutex; ///< Mutex for protecting access to #genericAccess.
    QMutex multimeterMutex;    ///< Mutex for protecting access to #multimeter.
    QMutex statusMutex;        ///< Mutex for protecting access to #status.

    explicit PokitDevicePrivate(PokitDevice * const q);

    void setController(QLowEnergyController * newController);

public slots:
    void connected();
    void connectionUpdated(const QLowEnergyConnectionParameters &newParameters);
    void disconnected();
    void discoveryFinished();
    void errorOccurred(QLowEnergyController::Error newError);
    void serviceDiscovered(const QBluetoothUuid &newService);
    void stateChanged(QLowEnergyController::ControllerState state);

protected:
    PokitDevice * q_ptr; ///< Internal q-pointer.

private:
    Q_DECLARE_PUBLIC(PokitDevice)
    Q_DISABLE_COPY(PokitDevicePrivate)
    friend class TestPokitDevice;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITDEVICE_P_H
