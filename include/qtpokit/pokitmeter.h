// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTPOKIT_POKITMETER_H
#define QTPOKIT_POKITMETER_H

#include "qtpokit_global.h"

#include <QVariant>

QTPOKIT_BEGIN_NAMESPACE

namespace PokitMeter {
    // Multimeter, DSO and Logger.
    enum class CurrentRange : quint8 {
        _10mA     = 0,  ///< Up to 10mA.
        _30mA     = 1,  ///< Up to 30mA.
        _150mA    = 2,  ///< Up to 150mA.
        _300mA    = 3,  ///< Up to 300mA.
        _2A       = 4,  ///< Up to 2A.
        AutoRange = 255 ///< Auto-range.
    };
    QTPOKIT_EXPORT QString toString(const CurrentRange &range);
    QTPOKIT_EXPORT QVariant maxValue(const CurrentRange &range);

    // Multimeter only.
    enum class ResistanceRange : quint8 {
        _160      = 0,  ///< Up to 160Ω.
        _330      = 1,  ///< Up to 330Ω.
        _890      = 2,  ///< Up to 890Ω.
        _1K5      = 3,  ///< Up to 1.5KΩ.
        _10K      = 4,  ///< Up to 10KΩ.
        _100K     = 5,  ///< Up to 100KΩ.
        _470K     = 6,  ///< Up to 470KΩ.
        _1M       = 7,  ///< Up to 1MΩ.
        AutoRange = 255 ///< Auto-range.
    };
    QTPOKIT_EXPORT QString toString(const ResistanceRange &range);
    QTPOKIT_EXPORT QVariant maxValue(const ResistanceRange &range);

    // Multimeter, DSO and Logger.
    enum class VoltageRange : quint8 {
        _300mV    = 0,  ///< Up to 300mV.
        _2V       = 1,  ///< Up to 2V.
        _6V       = 2,  ///< Up to 6V.
        _12V      = 3,  ///< Up to 12V.
        _30V      = 4,  ///< Up to 30V.
        _60V      = 5,  ///< Up to 60V DC (42V AC).
        AutoRange = 255 ///< Auto-range.
    };
    QTPOKIT_EXPORT QString toString(const VoltageRange &range);
    QTPOKIT_EXPORT QVariant maxValue(const VoltageRange &range);

    template<typename T> T minRange(const quint32 maxValue);
}

template<> QTPOKIT_EXPORT PokitMeter::CurrentRange    PokitMeter::minRange<PokitMeter::CurrentRange>   (const quint32 maxValue);
template<> QTPOKIT_EXPORT PokitMeter::ResistanceRange PokitMeter::minRange<PokitMeter::ResistanceRange>(const quint32 maxValue);
template<> QTPOKIT_EXPORT PokitMeter::VoltageRange    PokitMeter::minRange<PokitMeter::VoltageRange>   (const quint32 maxValue);

constexpr quint8 operator+(PokitMeter::CurrentRange    range) noexcept { return static_cast<quint8>(range); }
constexpr quint8 operator+(PokitMeter::ResistanceRange range) noexcept { return static_cast<quint8>(range); }
constexpr quint8 operator+(PokitMeter::VoltageRange    range) noexcept { return static_cast<quint8>(range); }

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITMETER_H
