// SPDX-FileCopyrightText: 2022-2026 Paul Colby <git@colby.id.au>
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
 * \def DOKIT_STRING_INDEX_TYPE
 *
 * Internal macro for matching the index type used by QString methods. Specifically, from Qt 6.0.0 onwards, QString
 * (and related classes, and methods) use qsizetype for index positions, while Qt5 used int instead.
 *
 * \todo Remove this macro when Dokit no longer supports Qt5.
 */

/*!
 * \def DOKIT_USE_STRINGLITERALS
 *
 * Internal macro for using either official Qt string literals (added in Qt 6.4), or our own equivalent ones for older
 * Qt versions.
 *
 * \todo Remove this macro when Dokit no longer supports Qt versions earlier than 6.4.
 */

#include <QtGlobal>

// Although qsizetype was added in Qt 5.10.0, it wasn't used for QString indexes until 6.0.
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#define DOKIT_STRING_INDEX_TYPE int
#else
#define DOKIT_STRING_INDEX_TYPE qsizetype
#endif

#if (QT_VERSION < QT_VERSION_CHECK(6, 4, 0)) // String literals added in Qt 6.4.0.

namespace _dokit {
inline namespace Literals {
inline namespace StringLiterals {

constexpr inline QLatin1Char operator""_L1(char ch) noexcept
{
    return QLatin1Char(ch);
}

constexpr inline QLatin1String/*View*/ operator""_L1(const char *str, size_t size) noexcept
{
    //return {str, qsizetype(size)};
    return QLatin1String(str, DOKIT_STRING_INDEX_TYPE(size));
}

inline QString operator""_s(const char16_t *str, size_t size) noexcept
{
    //return QString(QStringPrivate(nullptr, const_cast<char16_t *>(str), qsizetype(size)));
    return QString::fromUtf16(str, DOKIT_STRING_INDEX_TYPE(size));
}

} } } // _dokit::Literals::StringLiterals

#define DOKIT_USE_STRINGLITERALS using namespace _dokit::Literals::StringLiterals;
#else
#define DOKIT_USE_STRINGLITERALS using namespace Qt::Literals::StringLiterals;
#endif

/// \endcond

#endif // DOKIT_STRINGLITERALS_P_H
