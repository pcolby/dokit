// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <qtpokit/pokitmeter.h>
#include <qtpokit/pokitpro.h>
#include <qtpokit/pokitproducts.h>
#include <qtpokit/statusservice.h>

QString toString(const PokitProduct product)
{
    switch (product) {
    case PokitProduct::PokitMeter: return QStringLiteral("Pokit Meter");
    case PokitProduct::PokitPro:   return QStringLiteral("Pokit Pro");
    default:                       return QString();
    }
}

/*!
 * Returns \c true if \a serviceUuids contains a known Pokit Status service UUID.
 */
bool isPokitProduct(const QList<QBluetoothUuid> &serviceUuids)
{
    return (serviceUuids.contains(StatusService::ServiceUuids::pokitMeter) ||
            serviceUuids.contains(StatusService::ServiceUuids::pokitPro));
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
 * Returns \c true if \a controller describes a Pokit device.
 *
 * Currently, this is based on whether or not \a controller's service UUIDs includes a known Pokit
 * Status service, but this test criteria might be swapped for something else sometime.
 */
bool isPokitProduct(const QLowEnergyController &controller)
{
    return isPokitProduct(controller.services());
}

PokitProduct pokitProduct(const QList<QBluetoothUuid> &serviceUuids)
{
    if (serviceUuids.contains(StatusService::ServiceUuids::pokitMeter)) {
        return PokitProduct::PokitMeter;
    } else if (serviceUuids.contains(StatusService::ServiceUuids::pokitPro)) {
        return PokitProduct::PokitPro;
    } else {
        Q_ASSERT_X(false, "pokitProduct", "device is not a Pokit product");
        return PokitProduct::PokitMeter;
    }
}

PokitProduct pokitProduct(const QBluetoothDeviceInfo &info)
{
    return pokitProduct(info.serviceUuids());
}

PokitProduct pokitProduct(const QLowEnergyController &controller)
{
    return pokitProduct(controller.services());
}

namespace CapacitanceRange {

QString toString(const PokitProduct product, const quint8 range)
{
    Q_ASSERT(product == PokitProduct::PokitPro);
    switch (product) {
    case PokitProduct::PokitMeter:
        break;
    case PokitProduct::PokitPro:
        return PokitPro::toString(static_cast<PokitPro::CapacitanceRange>(range));
    }
    return QString();
}

QVariant maxValue(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        break;
    case PokitProduct::PokitPro:
        return PokitPro::maxValue(static_cast<PokitPro::CapacitanceRange>(range));
    }
    return QString();
}

}

namespace CurrentRange {

QString toString(const PokitProduct product, const quint8 range) {
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::toString(static_cast<PokitMeter::CurrentRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::toString(static_cast<PokitPro::CurrentRange>(range));
    }
    return QString();
}

QVariant maxValue(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::maxValue(static_cast<PokitMeter::CurrentRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::maxValue(static_cast<PokitPro::CurrentRange>(range));
    }
    return QString();
}

}

namespace ResistanceRange {

QString toString(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::toString(static_cast<PokitMeter::ResistanceRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::toString(static_cast<PokitPro::ResistanceRange>(range));
    }
    return QString();
}

QVariant maxValue(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::maxValue(static_cast<PokitMeter::ResistanceRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::maxValue(static_cast<PokitPro::ResistanceRange>(range));
    }
    return QString();
}

}

namespace VoltageRange {

QString toString(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::toString(static_cast<PokitMeter::VoltageRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::toString(static_cast<PokitPro::VoltageRange>(range));
    }
    return QString();
}

QVariant maxValue(const PokitProduct product, const quint8 range)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return PokitMeter::maxValue(static_cast<PokitMeter::VoltageRange>(range));
    case PokitProduct::PokitPro:
        return PokitPro::maxValue(static_cast<PokitPro::VoltageRange>(range));
    }
    return QString();
}

}
