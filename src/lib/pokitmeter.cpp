// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the PokitMeter namespace.
 */

#include "qtpokit/pokitmeter.h"

#include <QCoreApplication>

namespace PokitMeter {

/// Returns \a range as a user-friendly string.
QString toString(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_10mA:     return QCoreApplication::translate("PokitMeter", "Up to 10mA",  "CurrentRange");
    case CurrentRange::_30mA:     return QCoreApplication::translate("PokitMeter", "Up to 30mA",  "CurrentRange");
    case CurrentRange::_150mA:    return QCoreApplication::translate("PokitMeter", "Up to 150mA", "CurrentRange");
    case CurrentRange::_300mA:    return QCoreApplication::translate("PokitMeter", "Up to 300mA", "CurrentRange");
    case CurrentRange::_2A:       return QCoreApplication::translate("PokitMeter", "Up to 2A",    "CurrentRange");
    case CurrentRange::AutoRange: return QCoreApplication::translate("PokitMeter", "Auto-range",  "CurrentRange");
    default:                      return QString();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) microamps, or the string "Auto".
 *  If \a range is not a known valid value, then an null QVariant is returned.
 */
QVariant maxValue(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_10mA:     return    10'000;
    case CurrentRange::_30mA:     return    30'000;
    case CurrentRange::_150mA:    return   150'000;
    case CurrentRange::_300mA:    return   300'000;
    case CurrentRange::_2A:       return 2'000'000;
    case CurrentRange::AutoRange: return QCoreApplication::translate("PokitMeter", "Auto", "CurrentRange");
    default:                      return QVariant();
    }
}

/// Returns \a range as a user-friendly string.
QString toString(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_160:      return QCoreApplication::translate("PokitMeter", "Up to 160Ω",  "ResistanceRange");
    case ResistanceRange::_330:      return QCoreApplication::translate("PokitMeter", "Up to 330Ω",  "ResistanceRange");
    case ResistanceRange::_890:      return QCoreApplication::translate("PokitMeter", "Up to 890Ω",  "ResistanceRange");
    case ResistanceRange::_1K5:      return QCoreApplication::translate("PokitMeter", "Up to 1.5KΩ", "ResistanceRange");
    case ResistanceRange::_10K:      return QCoreApplication::translate("PokitMeter", "Up to 10KΩ",  "ResistanceRange");
    case ResistanceRange::_100K:     return QCoreApplication::translate("PokitMeter", "Up to 100KΩ", "ResistanceRange");
    case ResistanceRange::_470K:     return QCoreApplication::translate("PokitMeter", "Up to 470KΩ", "ResistanceRange");
    case ResistanceRange::_1M:       return QCoreApplication::translate("PokitMeter", "Up to 1MΩ",   "ResistanceRange");
    case ResistanceRange::AutoRange: return QCoreApplication::translate("PokitMeter", "Auto-range",  "ResistanceRange");
    default:                         return QString();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) ohms, or the string "Auto".
 *  If \a range is not a known valid value, then an null QVariant is returned.
 */
QVariant maxValue(const ResistanceRange &range)
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
    case ResistanceRange::AutoRange: return QCoreApplication::translate("PokitMeter", "Auto", "ResistanceRange");
    default:                         return QVariant();
    }
}

/// Returns \a range as a user-friendly string.
QString toString(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_300mV:    return QCoreApplication::translate("PokitMeter", "Up to 300mV", "VoltageRange");
    case VoltageRange::_2V:       return QCoreApplication::translate("PokitMeter", "Up to 2V",    "VoltageRange");
    case VoltageRange::_6V:       return QCoreApplication::translate("PokitMeter", "Up to 6V",    "VoltageRange");
    case VoltageRange::_12V:      return QCoreApplication::translate("PokitMeter", "Up to 12V",   "VoltageRange");
    case VoltageRange::_30V:      return QCoreApplication::translate("PokitMeter", "Up to 30V",   "VoltageRange");
    case VoltageRange::_60V:      return QCoreApplication::translate("PokitMeter", "Up to 60V",   "VoltageRange");
    case VoltageRange::AutoRange: return QCoreApplication::translate("PokitMeter", "Auto-range",  "VoltageRange");
    default:                      return QString();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) millivolts, or the string "Auto".
 *  If \a range is not a known valid value, then an null QVariant is returned.
 */
QVariant maxValue(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_300mV:    return    300;
    case VoltageRange::_2V:       return  2'000;
    case VoltageRange::_6V:       return  6'000;
    case VoltageRange::_12V:      return 12'000;
    case VoltageRange::_30V:      return 30'000;
    case VoltageRange::_60V:      return 60'000;
    case VoltageRange::AutoRange: return QCoreApplication::translate("PokitMeter", "Auto", "VoltageRange");
    default:                      return QVariant();
    }
}

}
