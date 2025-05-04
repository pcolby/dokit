// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the DOKIT_USE_STRINGLITERALS macro, and related functions.
 *
 * This is only required to support Qt versions earlier than 6.4, since Qt added string literal operators in that
 * versions. This header can, and will, be removed entirely when Dokit no longer supports Qt versions earlier than 6.4.
 */

#ifndef DOKIT_STRINGLITERALS_P_H
#define DOKIT_STRINGLITERALS_P_H

/// \cond internal

/*!
 * \def DOKIT_USE_STRINGLITERALS
 *
 * Internal macro for using either official Qt string literals (added in Qt 6.4), or our own equivalent ones for older Qt
 * versions. This macro can, and will, be removed entirely when Dokit no longer supports Qt versions earlier than 6.4.
 */

#include <QtGlobal>
#if (QT_VERSION < QT_VERSION_CHECK(6, 4, 0))

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#define __DOKIT_DEST_SIZE_TYPE int
#else
#define __DOKIT_DEST_SIZE_TYPE qsizetype
#endif

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
    return QLatin1String(str, __DOKIT_DEST_SIZE_TYPE(size));
}

inline QString operator"" _s(const char16_t *str, size_t size) noexcept
{
    //return QString(QStringPrivate(nullptr, const_cast<char16_t *>(str), qsizetype(size)));
    return QString::fromUtf16(str, __DOKIT_DEST_SIZE_TYPE(size));
}

} } } // _dokit::Literals::StringLiterals

#undef __DOKIT_DEST_SIZE_TYPE

#define DOKIT_USE_STRINGLITERALS using namespace _dokit::Literals::StringLiterals;
#else
#define DOKIT_USE_STRINGLITERALS using namespace Qt::Literals::StringLiterals;
#endif

/// \endcond

#endif // DOKIT_STRINGLITERALS_P_H
