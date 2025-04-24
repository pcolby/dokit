// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the QTPOKIT_USE_STRINGLITERALS macro, and related functions.
 *
 * This is only required to support Qt versions earlier than 6.4, since Qt added string literal operators in that
 * versions. This header can, and will, be removed entirely when support for pre-6.4 Qt is no longer necessary.
 */

#ifndef QTPOKIT_STRINGLITERALS_P_H
#define QTPOKIT_STRINGLITERALS_P_H

#if (QT_VERSION < QT_VERSION_CHECK(6, 4, 0))

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
    return QLatin1String(str,size);
}

inline QString operator"" _s(const char16_t *str, size_t size) noexcept
{
    //return QString(QStringPrivate(nullptr, const_cast<char16_t *>(str), qsizetype(size)));
    return QString::fromUtf16(str, size);
}

} } } // _dokit::Literals::StringLiterals
QTPOKIT_END_NAMESPACE

#define QTPOKIT_USE_STRINGLITERALS using namespace _dokit::Literals::StringLiterals;
#else
#define QTPOKIT_USE_STRINGLITERALS using namespace Qt::Literals::StringLiterals;
#endif

#endif // QTPOKIT_STIRNGLITERALS_P_H
