// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the #PokitProduct helper functions.
 */

#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>
#include <qtpokit/pokitproducts.h>
#include <qtpokit/statusservice.h>

#include "pokitproducts_p.h"

#include <QCoreApplication>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(lc, "dokit.pokit.products", QtInfoMsg); ///< Logging category for this file.

QTPOKIT_BEGIN_NAMESPACE

/*!
 * Returns \c product as user-friendly string.
 */
QString toString(const PokitProduct product)
{
    switch (product) {
    case PokitProduct::PokitMeter: return QStringLiteral("Pokit Meter");
    case PokitProduct::PokitPro:   return QStringLiteral("Pokit Pro");
    }
    qCWarning(lc).noquote() << QCoreApplication::translate("PokitProducts",
        "Unknown PokitProduct value: %1", "toString").arg((int)product);
    return QString();
}

/*!
 * Returns \c true if \a info describes a Pokit device.
 *
 * Currently, this is based on whether or not \a info's service UUIDs includes a known Pokit
 * Status service, but this test criteria might be swapped for something else sometime.
 */
bool isPokitProduct(const QBluetoothDeviceInfo &info)
{
    return isPokitProduct(info.serviceUuids());
}

/*!
 * Returns the #PokitProduct corresponding the Bluetotoh device \a info.
 *
 * If \a info is not a Pokit device, then result is undefined.
 *
 * \see isPokitProduct
 */
PokitProduct pokitProduct(const QBluetoothDeviceInfo &info)
{
    return pokitProduct(info.serviceUuids());
}

/// \cond internal

/*!
 * Returns \c true if \a serviceUuids contains a known Pokit Status service UUID.
 *
 * Currently, this is the only known way to detect a Pokit device.
 */
bool isPokitProduct(const QList<QBluetoothUuid> &serviceUuids)
{
    return (serviceUuids.contains(StatusService::ServiceUuids::pokitMeter) ||
            serviceUuids.contains(StatusService::ServiceUuids::pokitPro));
}


/*!
 * Returns \c true if \a controller describes a Pokit device.
 *
 * Currently, this is based on whether or not \a controller's service UUIDs includes a known Pokit
 * Status service, but this test criteria might be swapped for something else sometime.
 *
 * \see isPokitProduct
 */
bool isPokitProduct(const QLowEnergyController &controller)
{
    return isPokitProduct(controller.services());
}

/*!
 * Returns the #PokitProduct corresponding to the Bluetooth \a serviceUuids.
 *
 * Currently, this is based on whether or not \a servceUuids includes a known Pokit
 * Status service, but this test criteria might be swapped for something else sometime.
 *
 * \see isPokitProduct
 */
PokitProduct pokitProduct(const QList<QBluetoothUuid> &serviceUuids)
{
    if (serviceUuids.contains(StatusService::ServiceUuids::pokitMeter)) {
        return PokitProduct::PokitMeter;
    } else if (serviceUuids.contains(StatusService::ServiceUuids::pokitPro)) {
        return PokitProduct::PokitPro;
    } else {
        qCWarning(lc).noquote()
            << QCoreApplication::translate("PokitProducts", "Device is not a Pokit product", "pokitProduct");
        qCDebug(lc).noquote() << "Service UUIDs:" << serviceUuids;
        return PokitProduct::PokitMeter; // Need to fallback to something; Pokit Meter is just the lowest product.
    }
}

/*!
 * Returns the #PokitProduct corresponding to the Bluetooth \a controller.
 *
 * Currently, this is based on whether or not \a controller's service UUIDs includes a known Pokit
 * Status service, but this test criteria might be swapped for something else sometime.
 *
 * \see isPokitProduct
 */
PokitProduct pokitProduct(const QLowEnergyController &controller)
{
    return pokitProduct(controller.services());
}

QTPOKIT_END_NAMESPACE

/// Encapsulates convenience functions for working with capacitance ranges.
namespace CapacitanceRange {

/*!
 *  Returns \a product's capacitance \a range as a human-friendly string.
 *
 *  \note Since Pokit Meters do not support capacitance measurement, \a product should not be PokitProduct::PokitMeter.
 *
 *  \see PokitPro::toString(const PokitPro::CapacitanceRange &range)
 */
QString toString(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        qCWarning(lc).noquote()
            << QCoreApplication::translate("PokitProducts", "Pokit Meter has no capacitance support", "toString");
        return QString();
    case PokitProduct::PokitPro:
        return PokitPro::toString(static_cast<PokitPro::CapacitanceRange>(range));
    }
    qCWarning(lc).noquote() << QCoreApplication::translate("CapacitanceRange",
        "Unknown PokitProduct value: %1", "toString").arg((int)product);
    return QString();
}

/*!
 *  Returns the maximum value for \a product's \a range in (integer) nanofarads, or the string "Auto".
 *  If \a range is not a known valid value, then an null QVariant is returned.
 *
 *  \note Since Pokit Meters do not support capacitance measurement, \a product should not be PokitProduct::PokitMeter.
 *
 *  \see PokitPro::maxValue(const PokitPro::CapacitanceRange &range)
 */
QVariant maxValue(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        qCWarning(lc).noquote()
            << QCoreApplication::translate("PokitProducts", "Pokit Meter has no capacitance support", "toString");
        return QVariant();
    case PokitProduct::PokitPro:
        return PokitPro::maxValue(static_cast<PokitPro::CapacitanceRange>(range));
    }
    qCWarning(lc).noquote() << QCoreApplication::translate("CapacitanceRange",
        "Unknown PokitProduct value: %1", "maxValue").arg((int)product);
    return QVariant();
}

}

/// Encapsulates convenience functions for working with current ranges.
namespace CurrentRange {

/*!
 *  Returns \a product's current \a range as a human-friendly string.
 *
 *  \see PokitMeter::toString(const PokitMeter::CurrentRange &range)
 *  \see PokitPro::toString(const PokitPro::CurrentRange &range)
 */
QString toString(const PokitProduct product, const quint8 range) {
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::toString(static_cast<PokitMeter::CurrentRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::toString(static_cast<PokitPro::CurrentRange>(range));
    }
    qCWarning(lc).noquote() << QCoreApplication::translate("CurrentRange",
        "Unknown PokitProduct value: %1", "toString").arg((int)product);
    return QString();
}

/*!
 *  Returns the maximum value for \a product's \a range in (integer) microamps, or the string "Auto".
 *  If \a range is not a known valid value, then an null QVariant is returned.
 *
 *  \see PokitMeter::maxValue(const PokitMeter::CurrentRange &range)
 *  \see PokitPro::maxValue(const PokitPro::CurrentRange &range)
 */
QVariant maxValue(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::maxValue(static_cast<PokitMeter::CurrentRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::maxValue(static_cast<PokitPro::CurrentRange>(range));
    }
    qCWarning(lc).noquote() << QCoreApplication::translate("CurrentRange",
        "Unknown PokitProduct value: %1", "maxValue").arg((int)product);
    return QVariant();
}

}

/// Encapsulates convenience functions for working with resistance ranges.
namespace ResistanceRange {

/*!
 *  Returns \a product's current \a range as a human-friendly string.
 *
 *  \see PokitMeter::toString(const PokitMeter::ResistanceRange &range)
 *  \see PokitPro::toString(const PokitPro::ResistanceRange &range)
 */
QString toString(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::toString(static_cast<PokitMeter::ResistanceRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::toString(static_cast<PokitPro::ResistanceRange>(range));
    }
    qCWarning(lc).noquote() << QCoreApplication::translate("ResistanceRange",
        "Unknown PokitProduct value: %1", "toString").arg((int)product);
    return QString();
}

/*!
 *  Returns the maximum value for \a product's \a range in (integer) ohms, or the string "Auto".
 *  If \a range is not a known valid value, then an null QVariant is returned.
 *
 *  \see PokitMeter::maxValue(const PokitMeter::ResistanceRange &range)
 *  \see PokitPro::maxValue(const PokitPro::ResistanceRange &range)
 */
QVariant maxValue(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::maxValue(static_cast<PokitMeter::ResistanceRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::maxValue(static_cast<PokitPro::ResistanceRange>(range));
    }
    qCWarning(lc).noquote() << QCoreApplication::translate("ResistanceRange",
        "Unknown PokitProduct value: %1", "maxValue").arg((int)product);
    return QVariant();
}

}

/// Encapsulates convenience functions for working with voltage ranges.
namespace VoltageRange {

/*!
 *  Returns \a product's current \a range as a human-friendly string.
 *
 *  \see PokitMeter::toString(const PokitMeter::VoltageRange &range)
 *  \see PokitPro::toString(const PokitPro::VoltageRange &range)
 */
QString toString(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::toString(static_cast<PokitMeter::VoltageRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::toString(static_cast<PokitPro::VoltageRange>(range));
    }
    qCWarning(lc).noquote() << QCoreApplication::translate("VoltageRange",
        "Unknown PokitProduct value: %1", "toString").arg((int)product);
    return QString();
}

/*!
 *  Returns the maximum value for \a product's \a range in (integer) millivolts, or the string "Auto".
 *  If \a range is not a known valid value, then an null QVariant is returned.
 *
 *  \see PokitMeter::maxValue(const PokitMeter::VoltageRange &range)
 *  \see PokitPro::maxValue(const PokitPro::VoltageRange &range)
 */
QVariant maxValue(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::maxValue(static_cast<PokitMeter::VoltageRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::maxValue(static_cast<PokitPro::VoltageRange>(range));
    }
    qCWarning(lc).noquote() << QCoreApplication::translate("VoltageRange",
        "Unknown PokitProduct value: %1", "maxValue").arg((int)product);
    return QVariant();
}

}

/// \endcond
