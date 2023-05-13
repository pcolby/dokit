// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the PokitPro namespace.
 */

#ifndef QTPOKIT_POKITPRO_H
#define QTPOKIT_POKITPRO_H

#include "qtpokit_global.h"

#include <QVariant>

QTPOKIT_BEGIN_NAMESPACE

/// Encapsulates details specific to Pokit Pro devices.
namespace PokitPro {

    // Multimeter only.
    enum class CapacitanceRange : quint8 {
        _100nF    = 0,  ///< Up 100nF.
        _10uF     = 1,  ///< Up 10μF.
        _1mF      = 2,  ///< Up 1mF.
        AutoRange = 255 ///< Auto-range.
    };
    QTPOKIT_EXPORT QString toString(const CapacitanceRange &range);
    QTPOKIT_EXPORT QVariant maxValue(const CapacitanceRange &range);


    // Multimeter, DSO and Logger.
    enum class CurrentRange : quint8 {
        _500uA    = 0,  ///< Up to 5µA.
        _2mA      = 1,  ///< Up to 2mA.
        _10mA     = 2,  ///< Up to 10mA.
        _125mA    = 3,  ///< Up to 125mA.
        _300mA    = 4,  ///< Up to 300mA.
        _3A       = 5,  ///< Up to 3A.
        _10A      = 6,  ///< Up to 10A.
        AutoRange = 255 ///< Auto-range.
    };
    QTPOKIT_EXPORT QString toString(const CurrentRange &range);
    QTPOKIT_EXPORT QVariant maxValue(const CurrentRange &range);

    // Multimeter only.
    enum class ResistanceRange : quint8 {
        _30       = 0,  ///< Up to 30Ω.
        _75       = 1,  ///< Up to 75Ω.
        _400      = 2,  ///< Up to 400Ω.
        _5K       = 3,  ///< Up to 5KΩ.
        _10K      = 4,  ///< Up to 10KΩ.
        _15K      = 5,  ///< Up to 15KΩ.
        _40K      = 6,  ///< Up to 40KΩ.
        _500K     = 7,  ///< Up to 500KΩ.
        _700K     = 8,  ///< Up to 700KΩ.
        _1M       = 9,  ///< Up to 1MΩ.
        _3M       = 10, ///< Up to 3MΩ.
        AutoRange = 255 ///< Auto-range.
    };
    QTPOKIT_EXPORT QString toString(const ResistanceRange &range);
    QTPOKIT_EXPORT QVariant maxValue(const ResistanceRange &range);

    // Multimeter, DSO and Logger.
    enum class VoltageRange : quint8 {
        _250mV    = 0,  ///< Up to 250mV.
        _2V       = 1,  ///< Up to 2V.
        _10V      = 2,  ///< Up to 10V.
        _30V      = 3,  ///< Up to 30V.
        _60V      = 4,  ///< Up to 60V.
        _125V     = 5,  ///< Up to 125V.
        _400V     = 6,  ///< Up to 400V.
        _600V     = 7,  ///< Up to 600V.
        AutoRange = 255 ///< Auto-range.
    };
    QTPOKIT_EXPORT QString toString(const VoltageRange &range);
    QTPOKIT_EXPORT QVariant maxValue(const VoltageRange &range);

}

/// Returns \a range as a `quint8` as a convenience for assigning to services' `settings.range` members.
constexpr quint8 operator+(PokitPro::CapacitanceRange range) noexcept { return static_cast<quint8>(range); }

/// Returns \a range as a `quint8` as a convenience for assigning to services' `settings.range` members.
constexpr quint8 operator+(PokitPro::CurrentRange range) noexcept { return static_cast<quint8>(range); }

/// Returns \a range as a `quint8` as a convenience for assigning to services' `settings.range` members.
constexpr quint8 operator+(PokitPro::ResistanceRange range) noexcept { return static_cast<quint8>(range); }

/// Returns \a range as a `quint8` as a convenience for assigning to services' `settings.range` members.
constexpr quint8 operator+(PokitPro::VoltageRange range) noexcept { return static_cast<quint8>(range); }

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITPRO_H
