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
 * Declares the GenericAccessService class.
 */

#ifndef QTPOKIT_GENERICACCESSSERVICE_H
#define QTPOKIT_GENERICACCESSSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class GenericAccessServicePrivate;

class QTPOKIT_EXPORT GenericAccessService : public AbstractPokitService
{
    Q_OBJECT

public:
    static const QBluetoothUuid serviceUuid;

    struct CharacteristicUuids {
        static const QBluetoothUuid name;
        static const QBluetoothUuid appearance;
    };

    GenericAccessService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~GenericAccessService() override;

    bool readCharacteristics() override;
    bool readAppearanceCharacteristic();
    bool readNameCharacteristic();

    // Appearance characteristic (BLE read only).
    quint16 appearance() const;

    // Device Name characteristic (BLE read/write).
    QString deviceName() const;
    bool setDeviceName(const QString &name);

signals:
    void appearanceRead(const quint16 appearance);
    void deviceNameRead(const QString &deviceName);
    void deivceNameWritten();

protected:
    /// \cond internal
    GenericAccessService(GenericAccessServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(GenericAccessService)
    Q_DISABLE_COPY(GenericAccessService)
    friend class TestGenericAccessService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_GENERICACCESSSERVICE_H
