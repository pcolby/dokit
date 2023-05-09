// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "qtpokit/pokitpro.h"

#include <QCoreApplication>

namespace PokitPro {

/// Returns \a range as a user-friendly string.
QString toString(const CapacitanceRange &range)
{
    switch (range) {
    case CapacitanceRange::_100nF:    return QCoreApplication::translate("PokitPro", "Up to 100nF", "CapacitanceRange");
    case CapacitanceRange::_10uF:     return QCoreApplication::translate("PokitPro", "Up to 10μF",  "CapacitanceRange");
    case CapacitanceRange::_1mF:      return QCoreApplication::translate("PokitPro", "Up to 1mF",   "CapacitanceRange");
    case CapacitanceRange::AutoRange: return QCoreApplication::translate("PokitPro", "Auto-range",  "CapacitanceRange");
    default:                          return QString();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) nanofarads, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 */
QVariant maxValue(const CapacitanceRange &range)
{
    switch (range) {
    case CapacitanceRange::_100nF:    return     100;
    case CapacitanceRange::_10uF:     return   10000;
    case CapacitanceRange::_1mF:      return 1000000;
    case CapacitanceRange::AutoRange: return QCoreApplication::translate("PokitPro", "Auto", "CapacitanceRange");
    default:                          return QVariant();
    }
}

/// Returns \a range as a user-friendly string.
QString toString(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_500uA:    return QCoreApplication::translate("PokitPro", "Up to 500μA", "CurrentRange");
    case CurrentRange::_2mA:      return QCoreApplication::translate("PokitPro", "Up to 2mA",   "CurrentRange");
    case CurrentRange::_10mA:     return QCoreApplication::translate("PokitPro", "Up to 10mA",  "CurrentRange");
    case CurrentRange::_125mA:    return QCoreApplication::translate("PokitPro", "Up to 125mA", "CurrentRange");
    case CurrentRange::_300mA:    return QCoreApplication::translate("PokitPro", "Up to 300mA", "CurrentRange");
    case CurrentRange::_3A:       return QCoreApplication::translate("PokitPro", "Up to 3A",    "CurrentRange");
    case CurrentRange::_10A:      return QCoreApplication::translate("PokitPro", "Up to 10A",   "CurrentRange");
    case CurrentRange::AutoRange: return QCoreApplication::translate("PokitPro", "Auto-range",  "CurrentRange");
    default:                      return QString();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) microamps, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 */
QVariant maxValue(const CurrentRange &range)
{
    switch (range) {
    case CurrentRange::_500uA:    return      500;
    case CurrentRange::_2mA:      return     2000;
    case CurrentRange::_10mA:     return    10000;
    case CurrentRange::_125mA:    return   125000;
    case CurrentRange::_300mA:    return   300000;
    case CurrentRange::_3A:       return  3000000;
    case CurrentRange::_10A:      return 10000000;
    case CurrentRange::AutoRange: return QCoreApplication::translate("PokitPro", "Auto", "CurrentRange");
    default:                      return QVariant();
    }
}

/// Returns \a range as a user-friendly string.
QString toString(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_30:       return QCoreApplication::translate("PokitPro", "Up to 30Ω",   "ResistanceRange");
    case ResistanceRange::_75:       return QCoreApplication::translate("PokitPro", "Up to 75Ω",   "ResistanceRange");
    case ResistanceRange::_400:      return QCoreApplication::translate("PokitPro", "Up to 400Ω",  "ResistanceRange");
    case ResistanceRange::_5K:       return QCoreApplication::translate("PokitPro", "Up to 5KΩ",   "ResistanceRange");
    case ResistanceRange::_10K:      return QCoreApplication::translate("PokitPro", "Up to 10KΩ",  "ResistanceRange");
    case ResistanceRange::_15K:      return QCoreApplication::translate("PokitPro", "Up to 15KΩ",  "ResistanceRange");
    case ResistanceRange::_40K:      return QCoreApplication::translate("PokitPro", "Up to 40KΩ",  "ResistanceRange");
    case ResistanceRange::_500K:     return QCoreApplication::translate("PokitPro", "Up to 500KΩ", "ResistanceRange");
    case ResistanceRange::_700K:     return QCoreApplication::translate("PokitPro", "Up to 700KΩ", "ResistanceRange");
    case ResistanceRange::_1M:       return QCoreApplication::translate("PokitPro", "Up to 1MΩ",   "ResistanceRange");
    case ResistanceRange::_3M:       return QCoreApplication::translate("PokitPro", "Up to 3MΩ",   "ResistanceRange");
    case ResistanceRange::AutoRange: return QCoreApplication::translate("PokitPro", "Auto-range",  "ResistanceRange");
    default:                         return QString();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) ohms, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 */
QVariant maxValue(const ResistanceRange &range)
{
    switch (range) {
    case ResistanceRange::_30:       return      30;
    case ResistanceRange::_75:       return      75;
    case ResistanceRange::_400:      return     400;
    case ResistanceRange::_5K:       return    5000;
    case ResistanceRange::_10K:      return   10000;
    case ResistanceRange::_15K:      return   15000;
    case ResistanceRange::_40K:      return   40000;
    case ResistanceRange::_500K:     return  500000;
    case ResistanceRange::_700K:     return  700000;
    case ResistanceRange::_1M:       return 1000000;
    case ResistanceRange::_3M:       return 3000000;
    case ResistanceRange::AutoRange: return QCoreApplication::translate("PokitPro", "Auto", "ResistanceRange");
    default:                         return QVariant();
    }
}

/// Returns \a range as a user-friendly string.
QString toString(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_250mV:    return QCoreApplication::translate("PokitPro", "Up to 250mV", "VoltageRange");
    case VoltageRange::_2V:       return QCoreApplication::translate("PokitPro", "Up to 2V",    "VoltageRange");
    case VoltageRange::_10V:      return QCoreApplication::translate("PokitPro", "Up to 10V",   "VoltageRange");
    case VoltageRange::_30V:      return QCoreApplication::translate("PokitPro", "Up to 30V",   "VoltageRange");
    case VoltageRange::_60V:      return QCoreApplication::translate("PokitPro", "Up to 60V",   "VoltageRange");
    case VoltageRange::_125V:     return QCoreApplication::translate("PokitPro", "Up to 125V",  "VoltageRange");
    case VoltageRange::_400V:     return QCoreApplication::translate("PokitPro", "Up to 400V",  "VoltageRange");
    case VoltageRange::_600V:     return QCoreApplication::translate("PokitPro", "Up to 600V",  "VoltageRange");
    case VoltageRange::AutoRange: return QCoreApplication::translate("PokitPro", "Auto-range",  "VoltageRange");
    default:                      return QString();
    }
}

/*!
 *  Returns the maximum value for \a range in (integer) millivolts, or the string "Auto".
 *  If \a range is not known valid value, then an null QVariant is returned.
 */
QVariant maxValue(const VoltageRange &range)
{
    switch (range) {
    case VoltageRange::_250mV:    return    250;
    case VoltageRange::_2V:       return   2000;
    case VoltageRange::_10V:      return  10000;
    case VoltageRange::_30V:      return  30000;
    case VoltageRange::_60V:      return  60000;
    case VoltageRange::_125V:     return 125000;
    case VoltageRange::_400V:     return 400000;
    case VoltageRange::_600V:     return 600000;
    case VoltageRange::AutoRange: return QCoreApplication::translate("PokitPro", "Auto", "VoltageRange");
    default:                      return QVariant();
    }
}

}
