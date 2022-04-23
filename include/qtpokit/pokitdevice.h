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
 * Declares the PokitDevice class.
 */

#ifndef QTPOKIT_POKITDEVICE_H
#define QTPOKIT_POKITDEVICE_H

#include "qtpokit_global.h"

#include <QObject>

class QLowEnergyController;

QTPOKIT_BEGIN_NAMESPACE

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
    explicit PokitDevice(const QString &addressOrUuid, QObject *parent=nullptr);
    explicit PokitDevice(QLowEnergyController *controller, QObject *parent=nullptr);
    virtual ~PokitDevice();

    QLowEnergyController * controller();
    const QLowEnergyController * controller() const;

    DataLoggerService * dataLogger();
    DeviceInfoService * deviceInformation();
    DsoService * dso();
    GenericAccessService * genericAccess();
    MultimeterService * multimeter();
    StatusService * status();

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
