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
 * Official UUIDs from Pokit's Bluetooth API Definition document.
 */

#ifndef QTPOKIT_UUIDS_H
#define QTPOKIT_UUIDS_H

#include "qtpokit_global.h"

QTPOKIT_BEGIN_NAMESPACE

#define POKIT_SERVICE_MULTIMETER                "e7481d2f-5781-442e-bb9a-fd4e3441dadc"
#define POKIT_CHARACTERISTIC_MM_SETTINGS        "53dc9a7a-bc19-4280-b76b-002d0e23b078"
#define POKIT_CHARACTERISTIC_MM_READING         "047d3559-8bee-423a-b229-4417fa603b90"

#define POKIT_SERVICE_DSO                       "1569801e-1425-4a7a-b617-a4f4ed719de6"
#define POKIT_CHARACTERISTIC_DSO_SETTINGS       "a81af1b6-b8b3-4244-8859-3da368d2be39"
#define POKIT_CHARACTERISTIC_DSO_METADATA       "970f00ba-f46f-4825-96a8-153a5cd0cda9"
#define POKIT_CHARACTERISTIC_DSO_READING        "98e14f8e-536e-4f24-b4f4-1debfed0a99e"

#define POKIT_SERVICE_DATA_LOGGER               "a5ff3566-1fd8-4e10-8362-590a578a4121"
#define POKIT_CHARACTERISTIC_DL_SETTINGS        "5f97c62b-a83b-46c6-b9cd-cac59e130a78"
#define POKIT_CHARACTERISTIC_DL_METADATA        "9acada2e-3936-430b-a8f7-da407d97ca6e"
#define POKIT_CHARACTERISTIC_DL_READING         "3c669dab-fc86-411c-9498-4f9415049cc0"

#define POKIT_SERVICE_STATUS                    "57d3a771-267c-4394-8872-78223e92aec5" // 4?!
#define POKIT_CHARACTERISTIC_STATUS_DEVICE      "6974f5e5-0e54-45c3-97dd-29e4b5fb0849"
#define POKIT_CHARACTERISTIC_STATUS_STATUS      "3dba36e1-6120-4706-8dfd-ed9c16e569b6"
#define POKIT_CHARACTERISTIC_STATUS_DEVICE_NAME "7f0375de-077e-4555-8f78-800494509cc3"
#define POKIT_CHARACTERISTIC_STATUS_FLASH_LED   "ec9bb1f3-05a9-4277-8dd0-60a7896f0d6e"

/// \todo Should we keep these, or comment them out?
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#define POKIT_SERVICE_DEVICE_INFO                QBluetoothUuid::DeviceInformation      // 0x180A
#define POKIT_CHARACTERISTIC_DEVICE_MANUFACTURER QBluetoothUuid::ManufacturerNameString // 0x2A29
#define POKIT_CHARACTERISTIC_DEVICE_MODEL_NUMBER QBluetoothUuid::ModelNumberString      // 0x2A24
#define POKIT_CHARACTERISTIC_DEVICE_FIRMWARE_REV QBluetoothUuid::FirmwareRevisionString // 0x2A26
#define POKIT_CHARACTERISTIC_DEVICE_SOFTWARE_REV QBluetoothUuid::SoftwareRevisionString // 0x2A28
#define POKIT_CHARACTERISTIC_DEVICE_HARDWARE_REV QBluetoothUuid::HardwareRevisionString // 0x2A27

#define POKIT_SERVICE_GENERIC_ACCCESS            QBluetoothUuid::GenericAccess          // 0x1800
#define POKIT_CHARACTERISTIC_GA_DEVICE_NAME      QBluetoothUuid::DeviceName             // 0x2A00
#define POKIT_CHARACTERISTIC_GA_APPEARANCE       QBluetoothUuid::Appearance             // 0x2A01
#else /// \todo No doubt we could simplify these with some clever macros (if we keep them at all).
#define POKIT_SERVICE_DEVICE_INFO                QBluetoothUuid::ServiceClassUuid::DeviceInformation        // 0x180A
#define POKIT_CHARACTERISTIC_DEVICE_MANUFACTURER QBluetoothUuid::CharacteristicType::ManufacturerNameString // 0x2A29
#define POKIT_CHARACTERISTIC_DEVICE_MODEL_NUMBER QBluetoothUuid::CharacteristicType::ModelNumberString      // 0x2A24
#define POKIT_CHARACTERISTIC_DEVICE_FIRMWARE_REV QBluetoothUuid::CharacteristicType::FirmwareRevisionString // 0x2A26
#define POKIT_CHARACTERISTIC_DEVICE_SOFTWARE_REV QBluetoothUuid::CharacteristicType::SoftwareRevisionString // 0x2A28
#define POKIT_CHARACTERISTIC_DEVICE_HARDWARE_REV QBluetoothUuid::CharacteristicType::HardwareRevisionString // 0x2A27

#define POKIT_SERVICE_GENERIC_ACCCESS            QBluetoothUuid::ServiceClassUuid::GenericAccess            // 0x1800
#define POKIT_CHARACTERISTIC_GA_DEVICE_NAME      QBluetoothUuid::CharacteristicType::DeviceName             // 0x2A00
#define POKIT_CHARACTERISTIC_GA_APPEARANCE       QBluetoothUuid::CharacteristicType::Appearance             // 0x2A01
#endif
QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_UUIDS_H
