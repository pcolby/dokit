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
 * QtPokit logging categories.
 */

#ifndef QTPOKIT_LOGGING_P_H
#define QTPOKIT_LOGGING_P_H

#include "qtpokit_global.h"

#include <QLoggingCategory>

QTPOKIT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(pokitDiscovery);
Q_DECLARE_LOGGING_CATEGORY(pokitUtils);

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_LOGGING_P_H
