// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

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
