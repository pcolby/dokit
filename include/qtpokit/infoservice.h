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
 * Declares the InfoService class.
 */

#ifndef QTPOKIT_INFOSERVICE_H
#define QTPOKIT_INFOSERVICE_H

#include "abstractpokitservice.h"

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QVersionNumber>

QTPOKIT_BEGIN_NAMESPACE

class InfoServicePrivate;

class QTPOKIT_EXPORT InfoService : public AbstractPokitService
{
    Q_OBJECT

public:
    static const QBluetoothUuid serviceUuid;

    struct CharacteristicUuids {
        static const QBluetoothUuid manufacturerName;
        static const QBluetoothUuid modelNumber;
        static const QBluetoothUuid hardwareRevision;
        static const QBluetoothUuid firmwareRevision;
        static const QBluetoothUuid softwareRevision;
    };

    InfoService(QLowEnergyController * const pokitDevice, QObject * parent = nullptr);
    ~InfoService() override;

    bool readCharacteristics() override;
    bool readFirmwareRevisionCharacteristic();
    bool readHardwareRevisionCharacteristic();
    bool readManufacturerCharacteristics();
    bool readModelNumberCharacteristic();
    bool readSoftwareRevisionCharacteristic();

    // All Device Info characteristics are read-only, single values.
    QString manufacturer() const;
    QString modelNumber() const;
    QString firmwareRevision() const;
    QString hardwareRevision() const;
    QString softwareRevision() const;

signals:
    void manufacturerRead(const QString &name);
    void modelNumberRead(const QString &model);
    void firmwareRevisionRead(const QString &revision);
    void hardwareRevisionRead(const QString &revision);
    void softwareRevisionRead(const QString &revision);

protected:
    /// \cond internal
    InfoService(InfoServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(InfoService)
    Q_DISABLE_COPY(InfoService)
    friend class TestInfoService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_INFOSERVICE_H
