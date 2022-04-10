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
 * Global QtPokit library macros.
 */

#ifndef QTPOKIT_GLOBAL_H
#define QTPOKIT_GLOBAL_H

#include <QtGlobal>

/// \cond internal

/*!
 * QtPokit library export/import macro.
 */
#if defined(QTPOKIT_SHARED) || !defined(QTPOKIT_STATIC)
#  ifdef QTPOKIT_STATIC
#    error "Both QTPOKIT_SHARED and QTPOKIT_STATIC defined."
#  endif
#  if defined(QTPOKIT_LIBRARY)
#    define QTPOKIT_EXPORT Q_DECL_EXPORT
#  else
#    define QTPOKIT_EXPORT Q_DECL_IMPORT
#  endif
#else
#    define QTPOKIT_EXPORT
#endif

/*!
 * \def QTPOKIT_BEGIN_NAMESPACE
 *
 * Macro for starting the QtPokit library's top-most namespace (if one is defined).
 *
 * \sa QTPOKIT_END_NAMESPACE
 * \sa QTPOKIT_NAMESPACE
 */

/*!
 * \def QTPOKIT_END_NAMESPACE
 *
 * Macro for ending the QtPokit library's top-most namespace (if one is defined).
 *
 * \sa QTPOKIT_BEGIN_NAMESPACE
 * \sa QTPOKIT_NAMESPACE
 */

#ifdef QTPOKIT_NAMESPACE
  #define QTPOKIT_BEGIN_NAMESPACE namespace QTPOKIT_NAMESPACE {
  #define QTPOKIT_END_NAMESPACE }
#else
  #define QTPOKIT_BEGIN_NAMESPACE
  #define QTPOKIT_END_NAMESPACE
#endif

/// \endcond

#endif // QTPOKIT_GLOBAL_H
