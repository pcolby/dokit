// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the AbstractPokitService class.
 */

#ifndef QTPOKIT_ABSTRACTPOKITSERVICE_H
#define QTPOKIT_ABSTRACTPOKITSERVICE_H

#include "qtpokit_global.h"
#include "pokitproducts.h"

#include <QLowEnergyService>
#include <QObject>

class QLowEnergyController;

QTPOKIT_BEGIN_NAMESPACE

class AbstractPokitServicePrivate;

class QTPOKIT_EXPORT AbstractPokitService : public QObject
{
    Q_OBJECT

public:
    AbstractPokitService() = delete;
    virtual ~AbstractPokitService();

    virtual bool readCharacteristics() = 0;

    bool autoDiscover() const;
    void setAutoDiscover(const bool discover = true);

    PokitProduct pokitProduct() const;
    void setPokitProduct(const PokitProduct product);

    QLowEnergyService * service();
    const QLowEnergyService * service() const;

signals:
    void serviceDetailsDiscovered();
    void serviceErrorOccurred(QLowEnergyService::ServiceError newError);

protected:
    /// \cond internal
    AbstractPokitServicePrivate * d_ptr; ///< Internal d-pointer.
    AbstractPokitService(AbstractPokitServicePrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(AbstractPokitService)
    Q_DISABLE_COPY(AbstractPokitService)
    friend class TestAbstractPokitService;
};

QTPOKIT_END_NAMESPACE

#endif // QTPOKIT_ABSTRACTPOKITSERVICE_H
