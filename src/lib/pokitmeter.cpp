// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the PokitMeter namespace.
 */

#include "qtpokit/pokitmeter.h"

#include <QCoreApplication>
#include <QLoggingCategory>

namespace PokitMeter {

static Q_LOGGING_CATEGORY(lc, "dokit.pokit.products.meter", QtInfoMsg); ///< Logging category for this file.

namespace {
    class Private
    {
        Q_DECLARE_TR_FUNCTIONS(PokitMeter)
    };
}

/// Returns \a range as a user-friendly string.
QString toString(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_10mA:     return Private::tr("Up to 10mA");
    case CurrentRange::_30mA:     return Private::tr("Up to 30mA");
    case CurrentRange::_150mA:    return Private::tr("Up to 150mA");
    case CurrentRange::_300mA:    return Private::tr("Up to 300mA");
    case CurrentRange::_2A:       return Private::tr("Up to 2A");
    case CurrentRange::AutoRange: return Private::tr("Auto-range");
    default:                      return QString();
    }
}

/*!
 * Returns the maximum value for \a range in microamps, or 0 if \a range is not a known value for Pokit Meter devices.
 */
quint32 maxValue(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_10mA:     return    10'000;
    case CurrentRange::_30mA:     return    30'000;
    case CurrentRange::_150mA:    return   150'000;
    case CurrentRange::_300mA:    return   300'000;
    case CurrentRange::_2A:       return 2'000'000;
    case CurrentRange::AutoRange: return 2'000'000;
    default:
        qCWarning(lc).noquote() << Private::tr("Unknown CurrentRange value: %1").arg((int)range);
        return 0;
    }
}

/// Returns \a range as a user-friendly string.
QString toString(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_160:      return Private::tr("Up to 160Ω");
    case ResistanceRange::_330:      return Private::tr("Up to 330Ω");
    case ResistanceRange::_890:      return Private::tr("Up to 890Ω");
    case ResistanceRange::_1K5:      return Private::tr("Up to 1.5KΩ");
    case ResistanceRange::_10K:      return Private::tr("Up to 10KΩ");
    case ResistanceRange::_100K:     return Private::tr("Up to 100KΩ");
    case ResistanceRange::_470K:     return Private::tr("Up to 470KΩ");
    case ResistanceRange::_1M:       return Private::tr("Up to 1MΩ");
    case ResistanceRange::AutoRange: return Private::tr("Auto-range");
    default:                         return QString();
    }
}

/*!
 * Returns the maximum value for \a range in ohms, or 0 if \a range is not a known value for Pokit Meter devices.
 */
quint32 maxValue(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_160:      return       160;
    case ResistanceRange::_330:      return       330;
    case ResistanceRange::_890:      return       890;
    case ResistanceRange::_1K5:      return     1'500;
    case ResistanceRange::_10K:      return    10'000;
    case ResistanceRange::_100K:     return   100'000;
    case ResistanceRange::_470K:     return   470'000;
    case ResistanceRange::_1M:       return 1'000'000;
    case ResistanceRange::AutoRange: return 1'000'000;
    default:
        qCWarning(lc).noquote() << Private::tr("Unknown ResistanceRange value: %1").arg((int)range);
        return 0;
    }
}

/// Returns \a range as a user-friendly string.
QString toString(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_300mV:    return Private::tr("Up to 300mV");
    case VoltageRange::_2V:       return Private::tr("Up to 2V");
    case VoltageRange::_6V:       return Private::tr("Up to 6V");
    case VoltageRange::_12V:      return Private::tr("Up to 12V");
    case VoltageRange::_30V:      return Private::tr("Up to 30V");
    case VoltageRange::_60V:      return Private::tr("Up to 60V");
    case VoltageRange::AutoRange: return Private::tr("Auto-range");
    default:                      return QString();
    }
}

/*!
 * Returns the maximum value for \a range in millivolts, or 0 if \a range is not a known value for Pokit Meter devices.
 */
quint32 maxValue(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_300mV:    return    300;
    case VoltageRange::_2V:       return  2'000;
    case VoltageRange::_6V:       return  6'000;
    case VoltageRange::_12V:      return 12'000;
    case VoltageRange::_30V:      return 30'000;
    case VoltageRange::_60V:      return 60'000;
    case VoltageRange::AutoRange: return 60'000;
    default:
        qCWarning(lc).noquote() << Private::tr("Unknown VoltageRange value: %1").arg((int)range);
        return 0;
    }
}

}
