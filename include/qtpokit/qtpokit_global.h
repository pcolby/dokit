// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
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

/*!
 * \def QTPOKIT_USE_NAMESPACE
 *
 * Macro for using the QtPokit library's top-most namespace (if one is defined).
 *
 * \sa QTPOKIT_USE_NAMESPACE_MEMBER
 */

/*!
 * \def QTPOKIT_PREPEND_NAMESPACE
 *
 * Macro for prepending the QtPokit library's top-most namespace (if one is defined).
 */

/*!
 * \def QTPOKIT_FORWARD_DECLARE
 *
 * Macro for forward declaring objects in the QtPokit library's top-most namespace (if one is defined).
 *
 * \sa QTPOKIT_FORWARD_DECLARE_CLASS
 * \sa QTPOKIT_FORWARD_DECLARE_STRUCT
 */

#ifdef QTPOKIT_NAMESPACE
  #define QTPOKIT_BEGIN_NAMESPACE namespace QTPOKIT_NAMESPACE {
  #define QTPOKIT_END_NAMESPACE }
  #define QTPOKIT_USE_NAMESPACE using namespace ::QTPOKIT_NAMESPACE;
  #define QTPOKIT_PREPEND_NAMESPACE(name) ::QTPOKIT_NAMESPACE::name
  #define QTPOKIT_FORWARD_DECLARE(type, name) QTPOKIT_BEGIN_NAMESPACE type name; QTPOKIT_END_NAMESPACE QTPOKIT_USE_NAMESPACE_MEMBER(name)
  namespace QTPOKIT_NAMESPACE {}
#else
  #define QTPOKIT_BEGIN_NAMESPACE
  #define QTPOKIT_END_NAMESPACE
  #define QTPOKIT_USE_NAMESPACE
  #define QTPOKIT_PREPEND_NAMESPACE(name) ::name
  #define QTPOKIT_FORWARD_DECLARE(type, name) type name;
#endif

/*!
 * Macro for forward declaring classes in the QtPokit library's top-most namespace (if one is defined).
 *
 * \sa QTPOKIT_FORWARD_DECLARE
 * \sa QTPOKIT_FORWARD_DECLARE_STRUCT
 */
#define QTPOKIT_FORWARD_DECLARE_CLASS(name) QTPOKIT_FORWARD_DECLARE(class,name)

/*!
 * Macro for forward declaring classes in the QtPokit library's top-most namespace (if one is defined).
 *
 * \sa QTPOKIT_FORWARD_DECLARE
 * \sa QTPOKIT_FORWARD_DECLARE_CLASS
 */
#define QTPOKIT_FORWARD_DECLARE_STRUCT(name) QTPOKIT_FORWARD_DECLARE(struct,name)

/*!
 * Macro for using a member of the QtPokit library's top-most namespace (if one is defined).
 */
#define QTPOKIT_USE_NAMESPACE_MEMBER(name) using QTPOKIT_PREPEND_NAMESPACE(name);

/*!
 * \def QTPOKIT_BEFRIEND_TEST
 *
 * Macro for befriending a related unit test class, but only when QT_TESTLIB_LIB is defined.
 */

#ifdef QT_TESTLIB_LIB
  #define QTPOKIT_BEFRIEND_TEST(Class) friend class Test##Class;
#else
  #define QTPOKIT_BEFRIEND_TEST(Class)
#endif

/// \endcond

#endif // QTPOKIT_GLOBAL_H
