// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the PokitDiscoveryAgentPrivate class.
 */

#ifndef QTPOKIT_POKITDISCOVERYAGENT_P_H
#define QTPOKIT_POKITDISCOVERYAGENT_P_H

#include <qtpokit/qtpokit_global.h>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLoggingCategory>

QTPOKIT_BEGIN_NAMESPACE

class PokitDiscoveryAgent;

class QTPOKIT_EXPORT PokitDiscoveryAgentPrivate : public QObject
{
    Q_OBJECT

public:
    static Q_LOGGING_CATEGORY(lc, "pokit.ble.discovery", QtInfoMsg); ///< Logging category.

    explicit PokitDiscoveryAgentPrivate(PokitDiscoveryAgent * const q);

public Q_SLOTS:
    void canceled() const;
    void deviceDiscovered(const QBluetoothDeviceInfo &info);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    void deviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    #endif
    void error(const QBluetoothDeviceDiscoveryAgent::Error error) const;
    void finished() const;

protected:
    PokitDiscoveryAgent * q_ptr; ///< Internal q-pointer.

private:
    Q_DECLARE_PUBLIC(PokitDiscoveryAgent)
    Q_DISABLE_COPY(PokitDiscoveryAgentPrivate)
    QTPOKIT_BEFRIEND_TEST(PokitDiscoveryAgent)
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITDISCOVERYAGENT_P_H
