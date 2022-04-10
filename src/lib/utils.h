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
 * Declares some simple general purpose, utility functions used by QtPokit.
 */

#ifndef QTPOKIT_UTILS_H
#define QTPOKIT_UTILS_H

#include "qtpokit_global.h"

#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

QTPOKIT_BEGIN_NAMESPACE

QTPOKIT_EXPORT QJsonObject toJson(const QBluetoothDeviceInfo &info);
QTPOKIT_EXPORT QJsonValue  toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass);
QTPOKIT_EXPORT QJsonValue  toJson(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass);
QTPOKIT_EXPORT QJsonArray  toJson(const QBluetoothDeviceInfo::ServiceClasses &classes);
QTPOKIT_EXPORT QJsonArray  toJson(const QList<QBluetoothUuid> &uuids);
QTPOKIT_EXPORT QJsonObject toJson(const QMultiHash<quint16, QByteArray> &data);

QTPOKIT_EXPORT QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass);
QTPOKIT_EXPORT QString toString(const QBluetoothDeviceInfo::MajorDeviceClass &majorClass, const quint8 minorClass);

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_UTILS_H
