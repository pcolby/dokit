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
class DeviceInformationService;
class DsoService;
class GenericAccessService;
class MultimeterService;
class StatusService;

class PokitDevice;

class QTPOKIT_EXPORT PokitDevicePrivate : public QObject
{
    Q_OBJECT

public:
    QLowEnergyController * controller;

    DataLoggerService * dataLogger;
    DeviceInformationService * deviceInformation;
    DsoService * dso;
    GenericAccessService * genericAccess;
    MultimeterService * multimeter;
    StatusService * status;

    QMutex dataLoggerMutex, deviceInformationMutex, dsoMutex, genericAccessMutex, multimeterMutex,
        statusMutex;

    explicit PokitDevicePrivate(PokitDevice * const q);

    void setController(QLowEnergyController * newController);

public slots:
    void connected();
    void connectionUpdated(const QLowEnergyConnectionParameters &newParameters);
    void disconnected();
    void discoveryFinished();
    void error(QLowEnergyController::Error newError);
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
