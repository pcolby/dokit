// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the QTPOKIT_USE_STRINGLITERALS macro, and related functions.
 *
 * This is only required to support Qt versions earlier than 6.4, since Qt added string literal operators in that
 * versions. This header can, and will, be removed entirely when Dokit no long suppoers Qt versions earlier than 6.4.
 */

#ifndef QTPOKIT_STRINGLITERALS_P_H
#define QTPOKIT_STRINGLITERALS_P_H

/// \cond internal

/*!
 * \def QTPOKIT_USE_STRINGLITERALS
 *
 * Internal macro for using either official Qt string literals (added in Qt 6.4), or our own equivalent ones for older Qt
 * versions. This macro can, and will, be removed entirely when Dokit no long suppoers Qt versions earlier than 6.4.
 */

#include <QtGlobal>
#if (QT_VERSION < QT_VERSION_CHECK(6, 4, 0))

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#define __QTPOKIT_DEST_SIZE_TYPE int
#else
#define __QTPOKIT_DEST_SIZE_TYPE qsizetype
#endif

#include <qtpokit/qtpokit_global.h>
QTPOKIT_BEGIN_NAMESPACE
namespace _dokit {
inline namespace Literals {
inline namespace StringLiterals {

constexpr inline QLatin1Char operator"" _L1(char ch) noexcept
{
    return QLatin1Char(ch);
}

constexpr inline QLatin1String/*View*/ operator"" _L1(const char *str, size_t size) noexcept
{
    //return {str, qsizetype(size)};
    return QLatin1String(str, __QTPOKIT_DEST_SIZE_TYPE(size));
}

inline QString operator"" _s(const char16_t *str, size_t size) noexcept
{
    //return QString(QStringPrivate(nullptr, const_cast<char16_t *>(str), qsizetype(size)));
    return QString::fromUtf16(str, __QTPOKIT_DEST_SIZE_TYPE(size));
}

} } } // _dokit::Literals::StringLiterals
QTPOKIT_END_NAMESPACE
#undef __QTPOKIT_DEST_SIZE_TYPE

#define QTPOKIT_USE_STRINGLITERALS using namespace QTPOKIT_PREPEND_NAMESPACE(_dokit::Literals::StringLiterals);
#else
#define QTPOKIT_USE_STRINGLITERALS using namespace Qt::Literals::StringLiterals;
#endif

/// \endcond

#endif // QTPOKIT_STIRNGLITERALS_P_H
