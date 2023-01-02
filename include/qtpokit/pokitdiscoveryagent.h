// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the PokitDiscoveryAgent class.
 */

#ifndef QTPOKIT_POKITDISCOVERYAGENT_H
#define QTPOKIT_POKITDISCOVERYAGENT_H

#include "qtpokit_global.h"

#include <QBluetoothDeviceDiscoveryAgent>

QTPOKIT_BEGIN_NAMESPACE

class PokitDiscoveryAgentPrivate;

class QTPOKIT_EXPORT PokitDiscoveryAgent : public QBluetoothDeviceDiscoveryAgent
{
    Q_OBJECT

public:
    explicit PokitDiscoveryAgent(const QBluetoothAddress &deviceAdapter, QObject *parent=nullptr);
    PokitDiscoveryAgent(QObject * parent=nullptr);
    virtual ~PokitDiscoveryAgent();

    static bool isPokitDevice(const QBluetoothDeviceInfo &info);
    static bool isPokitMeter(const QBluetoothDeviceInfo &info);
    static bool isPokitPro(const QBluetoothDeviceInfo &info);

public slots:
    void start(QBluetoothDeviceDiscoveryAgent::DiscoveryMethods methods);
    void start();

signals:
    void pokitDeviceDiscovered(const QBluetoothDeviceInfo &info);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)) // Required signal, and Fields, added in Qt 5.12.
    void pokitDeviceUpdated(const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields);
    #endif

protected:
    /// \cond internal
    PokitDiscoveryAgentPrivate * d_ptr; ///< Internal d-pointer.
    PokitDiscoveryAgent(PokitDiscoveryAgentPrivate * const d,
                              const QBluetoothAddress &deviceAdapter, QObject * const parent);
    PokitDiscoveryAgent(PokitDiscoveryAgentPrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(PokitDiscoveryAgent)
    Q_DISABLE_COPY(PokitDiscoveryAgent)
    friend class TestPokitDiscoveryAgent;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_POKITDISCOVERYAGENT_H
