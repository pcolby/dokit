/*
    Copyright 2022 Paul Colby

    This file is part of Poqit.

    Poqit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Poqit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Poqit.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
 * \file
 * Global Poqit library macros.
 */

#ifndef POQIT_GLOBAL_H
#define POQIT_GLOBAL_H

#include <QtGlobal>

/// \cond internal

/*!
 * Poqit library export/import macro.
 */
#if defined(POQIT_SHARED) || !defined(POQIT_STATIC)
#  ifdef POQIT_STATIC
#    error "Both POQIT_SHARED and POQIT_STATIC defined."
#  endif
#  if defined(POQIT_LIBRARY)
#    define POQIT_EXPORT Q_DECL_EXPORT
#  else
#    define POQIT_EXPORT Q_DECL_IMPORT
#  endif
#else
#    define POQIT_EXPORT
#endif

/*!
 * \def POQIT_BEGIN_NAMESPACE
 *
 * Macro for starting the Poqit library's top-most namespace (if one is defined).
 *
 * \sa POQIT_END_NAMESPACE
 * \sa POQIT_NAMESPACE
 */

/*!
 * \def POQIT_END_NAMESPACE
 *
 * Macro for ending the Poqit library's top-most namespace (if one is defined).
 *
 * \sa POQIT_BEGIN_NAMESPACE
 * \sa POQIT_NAMESPACE
 */

#ifdef POQIT_NAMESPACE
  #define POQIT_BEGIN_NAMESPACE namespace POQIT_NAMESPACE {
  #define POQIT_END_NAMESPACE }
#else
  #define POQIT_BEGIN_NAMESPACE
  #define POQIT_END_NAMESPACE
#endif

/// \endcond

#endif // POQIT_GLOBAL_H
