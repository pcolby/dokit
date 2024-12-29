// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defined the PokitPro helper functions.
 */

#include "qtpokit/pokitpro.h"

#include <QCoreApplication>
#include <QLoggingCategory>

namespace PokitPro {

static Q_LOGGING_CATEGORY(lc, "dokit.pokit.products.pro", QtInfoMsg); ///< Logging category for this file.

namespace {
    class Private
    {
        Q_DECLARE_TR_FUNCTIONS(PokitPro)
    };
}

/*!
 * \cond internal
 * \enum CapacitanceRange
 * \pokitApi These Pokit Pro enumeration values are as-yet undocumented by Pokit Innovations.
 * [\@pcolby](https://github.com/pcolby) reverse-engineered them as part of the
 * [dokit](https://github.com/pcolby/dokit) project.
 * \endcond
 */

/// Returns \a range as a user-friendly string.
QString toString(const CapacitanceRange &range)
{
    switch (range) {
    case CapacitanceRange::_100nF:    return Private::tr("Up to 100nF");
    case CapacitanceRange::_10uF:     return Private::tr("Up to 10μF");
    case CapacitanceRange::_1mF:      return Private::tr("Up to 1mF");
    case CapacitanceRange::AutoRange: return Private::tr("Auto-range");
    default:                          return QString();
    }
}

/*!
 * Returns the maximum value for \a range in nanofarads, or 0 if \a range is not a known value for Pokit Pro devices.
 */
quint32 maxValue(const CapacitanceRange &range)
{
    switch (range) {
    case CapacitanceRange::_100nF:    return       100;
    case CapacitanceRange::_10uF:     return    10'000;
    case CapacitanceRange::_1mF:      return 1'000'000;
    case CapacitanceRange::AutoRange: return 1'000'000;
    default:
        qCWarning(lc).noquote() << Private::tr("Unknown CapacitanceRange value: %1").arg((int)range);
        return 0;
    }
}

/*!
 * \cond internal
 * \enum CurrentRange
 * \pokitApi These Pokit Pro enumeration values are as-yet undocumented by Pokit Innovations.
 * [\@pcolby](https://github.com/pcolby) reverse-engineered them as part of the
 * [dokit](https://github.com/pcolby/dokit) project.
 * \endcond
 */

/// Returns \a range as a user-friendly string.
QString toString(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_500uA:    return Private::tr("Up to 500μA");
    case CurrentRange::_2mA:      return Private::tr("Up to 2mA");
    case CurrentRange::_10mA:     return Private::tr("Up to 10mA");
    case CurrentRange::_125mA:    return Private::tr("Up to 125mA");
    case CurrentRange::_300mA:    return Private::tr("Up to 300mA");
    case CurrentRange::_3A:       return Private::tr("Up to 3A");
    case CurrentRange::_10A:      return Private::tr("Up to 10A");
    case CurrentRange::AutoRange: return Private::tr("Auto-range");
    default:                      return QString();
    }
}

/*!
 * Returns the maximum value for \a range in microamps, or 0 if \a range is not a known value for Pokit Pro devices.
 */
quint32 maxValue(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_500uA:    return        500;
    case CurrentRange::_2mA:      return      2'000;
    case CurrentRange::_10mA:     return     10'000;
    case CurrentRange::_125mA:    return    125'000;
    case CurrentRange::_300mA:    return    300'000;
    case CurrentRange::_3A:       return  3'000'000;
    case CurrentRange::_10A:      return 10'000'000;
    case CurrentRange::AutoRange: return 10'000'000;
    default:
        qCWarning(lc).noquote() << Private::tr("Unknown CurrentRange value: %1").arg((int)range);
        return 0;
    }
}

/*!
 * \cond internal
 * \enum ResistanceRange
 * \pokitApi These Pokit Pro enumeration values are as-yet undocumented by Pokit Innovations.
 * [\@pcolby](https://github.com/pcolby) reverse-engineered them as part of the
 * [dokit](https://github.com/pcolby/dokit) project.
 * \endcond
 */

/// Returns \a range as a user-friendly string.
QString toString(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_30:       return Private::tr("Up to 30Ω");
    case ResistanceRange::_75:       return Private::tr("Up to 75Ω");
    case ResistanceRange::_400:      return Private::tr("Up to 400Ω");
    case ResistanceRange::_5K:       return Private::tr("Up to 5KΩ");
    case ResistanceRange::_10K:      return Private::tr("Up to 10KΩ");
    case ResistanceRange::_15K:      return Private::tr("Up to 15KΩ");
    case ResistanceRange::_40K:      return Private::tr("Up to 40KΩ");
    case ResistanceRange::_500K:     return Private::tr("Up to 500KΩ");
    case ResistanceRange::_700K:     return Private::tr("Up to 700KΩ");
    case ResistanceRange::_1M:       return Private::tr("Up to 1MΩ");
    case ResistanceRange::_3M:       return Private::tr("Up to 3MΩ");
    case ResistanceRange::AutoRange: return Private::tr("Auto-range");
    default:                         return QString();
    }
}

/*!
 * Returns the maximum value for \a range in ohms, or 0 if \a range is not a known value for Pokit Pro devices.
 */
quint32 maxValue(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_30:       return        30;
    case ResistanceRange::_75:       return        75;
    case ResistanceRange::_400:      return       400;
    case ResistanceRange::_5K:       return     5'000;
    case ResistanceRange::_10K:      return    10'000;
    case ResistanceRange::_15K:      return    15'000;
    case ResistanceRange::_40K:      return    40'000;
    case ResistanceRange::_500K:     return   500'000;
    case ResistanceRange::_700K:     return   700'000;
    case ResistanceRange::_1M:       return 1'000'000;
    case ResistanceRange::_3M:       return 3'000'000;
    case ResistanceRange::AutoRange: return 3'000'000;
    default:
        qCWarning(lc).noquote() << Private::tr("Unknown ResistanceRange value: %1").arg((int)range);
        return 0;
    }
}

/*!
 * \cond internal
 * \enum VoltageRange
 * \pokitApi These Pokit Pro enumeration values are as-yet undocumented by Pokit Innovations.
 * [\@pcolby](https://github.com/pcolby) reverse-engineered them as part of the
 * [dokit](https://github.com/pcolby/dokit) project.
 * \endcond
 */

/// Returns \a range as a user-friendly string.
QString toString(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_250mV:    return Private::tr("Up to 250mV");
    case VoltageRange::_2V:       return Private::tr("Up to 2V");
    case VoltageRange::_10V:      return Private::tr("Up to 10V");
    case VoltageRange::_30V:      return Private::tr("Up to 30V");
    case VoltageRange::_60V:      return Private::tr("Up to 60V");
    case VoltageRange::_125V:     return Private::tr("Up to 125V");
    case VoltageRange::_400V:     return Private::tr("Up to 400V");
    case VoltageRange::_600V:     return Private::tr("Up to 600V");
    case VoltageRange::AutoRange: return Private::tr("Auto-range");
    default:                      return QString();
    }
}

/*!
 * Returns the maximum value for \a range in millivolts, or 0 if \a range is not a known value for Pokit Pro devices.
 */
quint32 maxValue(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_250mV:    return     250;
    case VoltageRange::_2V:       return   2'000;
    case VoltageRange::_10V:      return  10'000;
    case VoltageRange::_30V:      return  30'000;
    case VoltageRange::_60V:      return  60'000;
    case VoltageRange::_125V:     return 125'000;
    case VoltageRange::_400V:     return 400'000;
    case VoltageRange::_600V:     return 600'000;
    case VoltageRange::AutoRange: return 600'000;
    default:
        qCWarning(lc).noquote() << Private::tr("Unknown VoltageRange value: %1").arg((int)range);
        return 0;
    }
}

}
