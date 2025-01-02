// SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the AbstractPokitService and AbstractPokitServicePrivate classes.
 */

#include <qtpokit/abstractpokitservice.h>
#include "abstractpokitservice_p.h"
#include "pokitproducts_p.h"

#include <qtpokit/pokitdevice.h>

#include <QLowEnergyController>

/*!
 * \class AbstractPokitService
 *
 * The AbstractPokitService class provides a common base for Pokit services classes.
 */

/*!
 * \cond internal
 * Constructs a new Pokit service with \a parent, and private implementation \a d.
 */
AbstractPokitService::AbstractPokitService(
    AbstractPokitServicePrivate * const d, QObject * const parent)
    : QObject(parent), d_ptr(d)
{

}
/// \endcond

/*!
 * Destroys this AbstractPokitService object.
 */
AbstractPokitService::~AbstractPokitService()
{
    delete d_ptr;
}

/*!
 * \fn virtual bool AbstractPokitService::readCharacteristics() = 0
 *
 * Read all characteristics.
 *
 * This convenience function will queue refresh requests of all characteristics supported by this
 * service.
 *
 * Relevant `*Service::*Read` signals will be emitted by derived class objects as each
 * characteristic is successfully read.
 */

/*!
 * Returns `true` if autodiscovery of services and service details is enabled, `false` otherwise.
 *
 * \see setAutoDiscover for more information on what autodiscovery provides.
 */
bool AbstractPokitService::autoDiscover() const
{
    Q_D(const AbstractPokitService);
    return d->autoDiscover;
}

/*!
 * If \a discover is \c true, autodiscovery will be attempted.
 *
 * Specifically, this may resulting in automatic invocation of:
 * * QLowEnergyController::discoverServices if/when the internal controller is connected; and
 * * QLowEnergyService::discoverDetails if/when an internal service object is created.
 *
 * \see autoDiscover
 */
void AbstractPokitService::setAutoDiscover(const bool discover)
{
    Q_D(AbstractPokitService);
    d->autoDiscover = discover;
}

/*!
 * Returns the Pokit product this service is attached to.
 *
 * \see setPokitProduct
 */
std::optional<PokitProduct> AbstractPokitService::pokitProduct() const
{
    Q_D(const AbstractPokitService);
    return d->pokitProduct;
}

/*!
 * Sets the current Pokit \a product.
 *
 * This must be called to set the product before this object's BLE controller's services are discovered. If
 * autoDiscover() is enabled, then this should be done before the controller's \c connectToDevice() is called.
 *
 * For example:
 * ```
 * Q_ASSERT(isPokitProduct(deviceInfo));
 * auto controller = QLowEnergyController::createCentral(deviceInfo);
 * auto service = new DsoService(controller);
 * service->setPokitProduct(pokitProduct(deviceInfo));
 * controller->connectToDevice();
 * ```
 *
 * \see autoDiscover
 * \see pokitProduct
 */
void AbstractPokitService::setPokitProduct(const PokitProduct product)
{
    Q_D(AbstractPokitService);
    d->pokitProduct = product;
}

/*!
 * Returns a non-const pointer to the internal service object, if any.
 */
QLowEnergyService * AbstractPokitService::service()
{
    Q_D(AbstractPokitService);
    return d->service;
}

/*!
 * Returns a const pointer to the internal service object, if any.
 */
const QLowEnergyService * AbstractPokitService::service() const
{
    Q_D(const AbstractPokitService);
    return d->service;
}

/*!
 * \fn void AbstractPokitService::serviceDetailsDiscovered()
 *
 * This signal is emitted when the Pokit service details have been discovered.
 *
 * Once this signal has been emitted, cached characteristics values should be immediately available
 * via derived classes' accessor functions, and refreshes can be queued via readCharacteristics()
 * and any related read functions provided by derived classes.
 */

/*!
 * \fn void AbstractPokitService::serviceErrorOccurred(QLowEnergyService::ServiceError newError)
 *
 *  This signal is emitted whenever an error occurs on the underlying QLowEnergyService.
 */

/*!
 * \cond internal
 * \class AbstractPokitServicePrivate
 *
 * The AbstractPokitServicePrivate class provides private implementation for AbstractPokitService.
 */

/*!
 * \internal
 * Constructs a new AbstractPokitServicePrivate object with public implementation \a q.
 *
 * Note, typically the \a serviceUuid should be set validly, however, in the rare case that a
 * service's UUID can vary (ie the Status Service), \a serviceUuid may be set to a `null`
 * QBluetoothUuid here, and updated when the correct service UUID is known.
 *
 * \see StatusService::ServiceUuids
 * \see StatusServicePrivate::serviceDiscovered
 */
AbstractPokitServicePrivate::AbstractPokitServicePrivate(const QBluetoothUuid &serviceUuid,
    QLowEnergyController * controller, AbstractPokitService * const q)
    : controller(controller), serviceUuid(serviceUuid), q_ptr(q)
{
    if (controller) {
        connect(controller, &QLowEnergyController::connected,
                this, &AbstractPokitServicePrivate::connected);

        connect(controller, &QLowEnergyController::discoveryFinished,
                this, &AbstractPokitServicePrivate::discoveryFinished);

        connect(controller, &QLowEnergyController::serviceDiscovered,
                this, &AbstractPokitServicePrivate::serviceDiscovered);

        createServiceObject();
    }
}

/*!
 * Creates an internal service object from the internal controller.
 *
 * Any existing service object will *not* be replaced.
 *
 * Returns \c true if a service was created successfully, either now, or sometime previously.
 */
bool AbstractPokitServicePrivate::createServiceObject()
{
    if (!controller) {
        return false;
    }

    if (service) {
        qCDebug(lc).noquote() << tr("Already have service object:") << service;
        return true;
    }

    if (serviceUuid.isNull()) {
        qCDebug(lc).noquote() << tr("Service UUID not assigned yet.");
        return false;
    }

    service = controller->createServiceObject(serviceUuid, this);
    if (!service) {
        return false;
    }
    qCDebug(lc).noquote() << tr("Service object created for %1 device:").arg(toString(*this->pokitProduct)) << service;

    connect(service, &QLowEnergyService::stateChanged,
            this, &AbstractPokitServicePrivate::stateChanged);
    connect(service, &QLowEnergyService::characteristicRead,
            this, &AbstractPokitServicePrivate::characteristicRead);
    connect(service, &QLowEnergyService::characteristicWritten,
            this, &AbstractPokitServicePrivate::characteristicWritten);
    connect(service, &QLowEnergyService::characteristicChanged,
            this, &AbstractPokitServicePrivate::characteristicChanged);

    connect(service, &QLowEnergyService::descriptorRead, this,
        [](const QLowEnergyDescriptor &descriptor, const QByteArray &value){
            qCDebug(lc).noquote() << tr(R"(Descriptor "%1" (%2) read.)")
                .arg(descriptor.name(), descriptor.uuid().toString());
            Q_UNUSED(value)
        });

    connect(service, &QLowEnergyService::descriptorWritten, this,
        [](const QLowEnergyDescriptor &descriptor, const QByteArray &newValue){
            qCDebug(lc).noquote() << tr(R"(Descriptor "%1" (%2) written.)")
                .arg(descriptor.name(), descriptor.uuid().toString());
            Q_UNUSED(newValue)
        });

    connect(service,
    #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
        QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error),
    #else
        &QLowEnergyService::errorOccurred,
    #endif
        this, &AbstractPokitServicePrivate::errorOccurred);

    if (autoDiscover) {
        service->discoverDetails();
    }
    return true;
}

/*!
 * Get \a uuid characteristc from the underlying service. This helper function is equivalent to
 *
 * ```
 * return service->characteristic(uuid);
 * ```
 *
 * except that it performs some sanity checks, such as checking the service object pointer has been
 * assigned first, and also logs failures in a consistent manner.
 *
 * \param uuid
 * \return
 */
QLowEnergyCharacteristic AbstractPokitServicePrivate::getCharacteristic(const QBluetoothUuid &uuid) const
{
    if (!service) {
        qCDebug(lc).noquote() << tr(R"(Characteristic %1 "%2" requested before service assigned.)")
            .arg(uuid.toString(), PokitDevice::charcteristicToString(uuid));
        return QLowEnergyCharacteristic();
    }

    if (const QLowEnergyCharacteristic characteristic = service->characteristic(uuid); characteristic.isValid()) {
        return characteristic;
    }

    if (service->state() != QLowEnergyService::
        #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        ServiceDiscovered
        #else
        RemoteServiceDiscovered
        #endif
    ) {
        qCWarning(lc).noquote() << tr(R"(Characteristic %1 "%2" requested before service %3 "%4" discovered.)")
            .arg(uuid.toString(), PokitDevice::charcteristicToString(uuid),
            service->serviceUuid().toString(), PokitDevice::serviceToString(service->serviceUuid()));
        qCInfo(lc).noquote() << tr("Current service state:") << service->state();
        return QLowEnergyCharacteristic();
    }

    qCWarning(lc).noquote() << tr(R"(Characteristic %1 "%2" not found in service %3 "%4".)")
        .arg(uuid.toString(), PokitDevice::charcteristicToString(uuid),
        service->serviceUuid().toString(), PokitDevice::serviceToString(service->serviceUuid()));
    return QLowEnergyCharacteristic();
}

/*!
 * Read the \a uuid characteristic.
 *
 * If succesful, the `QLowEnergyService::characteristicRead` signal will be emitted by the internal
 * service object.  For convenience, derived classes should implement the characteristicRead()
 * virtual function to handle the read value.
 *
 * Returns \c true if the characteristic read request was successfully queued, \c false otherwise.
 *
 * \see AbstractPokitService::readCharacteristics()
 * \see AbstractPokitServicePrivate::characteristicRead()
 */
bool AbstractPokitServicePrivate::readCharacteristic(const QBluetoothUuid &uuid)
{
    const QLowEnergyCharacteristic characteristic = getCharacteristic(uuid);
    if (!characteristic.isValid()) {
        return false;
    }
    qCDebug(lc).noquote() << tr(R"(Reading characteristic %1 "%2".)")
        .arg(uuid.toString(), PokitDevice::charcteristicToString(uuid));
    service->readCharacteristic(characteristic);
    return true;
}

/*!
 * Enables client (Pokit device) side notification for characteristic \a uuid.
 *
 * Returns \c true if the notication enable request was successfully queued, \c false otherwise.
 *
 * \see AbstractPokitServicePrivate::characteristicChanged
 * \see AbstractPokitServicePrivate::disableCharacteristicNotificatons
 */
bool AbstractPokitServicePrivate::enableCharacteristicNotificatons(const QBluetoothUuid &uuid)
{
    qCDebug(lc).noquote() << tr(R"(Enabling CCCD for characteristic %1 "%2".)")
        .arg(uuid.toString(), PokitDevice::charcteristicToString(uuid));
    QLowEnergyCharacteristic characteristic = getCharacteristic(uuid);
    if (!characteristic.isValid()) {
        return false;
    }

    QLowEnergyDescriptor descriptor = characteristic.descriptor(
        QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration); // 0x2902
    if (!descriptor.isValid()) {
        qCWarning(lc).noquote() << tr(R"(Characteristic %1 "%2" has no client configuration descriptor.)")
            .arg(uuid.toString(), PokitDevice::charcteristicToString(uuid));
        return false;
    }

    service->writeDescriptor(descriptor,
        #if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
        QLowEnergyCharacteristic::CCCDEnableNotification
        #else
        QByteArray::fromHex("0100") // See Qt6's QLowEnergyCharacteristic::CCCDEnableNotification.
        #endif
    );
    return true;
}

/*!
 * Disables client (Pokit device) side notification for characteristic \a uuid.
 *
 * Returns \c true if the notication disable request was successfully queued, \c false otherwise.
 *
 * \see AbstractPokitServicePrivate::characteristicChanged
 * \see AbstractPokitServicePrivate::enableCharacteristicNotificatons
 */
bool AbstractPokitServicePrivate::disableCharacteristicNotificatons(const QBluetoothUuid &uuid)
{
    qCDebug(lc).noquote() << tr(R"(Disabling CCCD for characteristic %1 "%2".)")
        .arg(uuid.toString(), PokitDevice::charcteristicToString(uuid));
    QLowEnergyCharacteristic characteristic = getCharacteristic(uuid);
    if (!characteristic.isValid()) {
        return false;
    }

    QLowEnergyDescriptor descriptor = characteristic.descriptor(
        QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration); // 0x2902
    if (!descriptor.isValid()) {
        qCWarning(lc).noquote() << tr(R"(Characteristic %1 "%2" has no client configuration descriptor.)")
            .arg(uuid.toString(), PokitDevice::charcteristicToString(uuid));
        return false;
    }

    service->writeDescriptor(descriptor,
        #if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
        QLowEnergyCharacteristic::CCCDDisable
        #else
        QByteArray::fromHex("0000") // See Qt6's QLowEnergyCharacteristic::CCCDDisable.
        #endif
    );
    return true;
}

/*!
 * Returns `false` if \a data is smaller than \a minSize, otherwise returns \a failOnMax if \a data
 * is bigger than \a maxSize, otherwise returns `true`.
 *
 * A warning is logged if either \a minSize or \a maxSize is violated, regardless of the returned
 * value; ie this funcion can be used to simply warn if \a data is too big, or it can be used to
 * failed (return `false`) in that case.
 */
bool AbstractPokitServicePrivate::checkSize(const QString &label, const QByteArray &data,
                                            const int minSize, const int maxSize,
                                            const bool failOnMax)
{
    if (data.size() < minSize) {
        qCWarning(lc).noquote() << tr("%1 requires %n byte/s, but only %2 present: %3", nullptr, minSize)
            .arg(label).arg(data.size()).arg(toHexString(data));
        return false;
    }
    if ((maxSize >= 0) && (data.size() > maxSize)) {
        qCWarning(lc).noquote() << tr("%1 has %n extraneous byte/s: %2", nullptr, data.size()-maxSize)
            .arg(label, toHexString(data.mid(maxSize)));
        return (!failOnMax);
    }
    return true;
}

/*!
 * Returns up to \a maxSize bytes of \a data as a human readable hexadecimal string. If \a data
 * exceeds \a maxSize, then \a data is elided in the middle. For example:
 *
 * ```
 * toHex(QBytArray("\x1\x2\x3\x4\x5\x6", 4); // "0x01,02,...,05,06"
 * ```
 */
QString AbstractPokitServicePrivate::toHexString(const QByteArray &data, const int maxSize)
{
    return (data.size() <= maxSize)
        ? QString::fromLatin1("0x%1").arg(QLatin1String(data.toHex(',')))
        : QString::fromLatin1("0x%1,...,%2").arg(
            QLatin1String(data.left(maxSize/2-1).toHex(',')),
            QLatin1String(data.right(maxSize/2-1).toHex(',')));
}

/*!
 * Handles `QLowEnergyController::connected` events.
 *
 * If `autoDiscover` is enabled, this will begin service discovery on the newly connected contoller.
 *
 * \see AbstractPokitService::autoDiscover()
 */
void AbstractPokitServicePrivate::connected()
{
    if (!controller) {
        qCWarning(lc).noquote() << tr("Connected with no controller set") << sender();
        return;
    }

    qCDebug(lc).noquote() << tr(R"(Connected to "%1" (%2) at %3.)").arg(
        controller->remoteName(), controller->remoteDeviceUuid().toString(),
        controller->remoteAddress().toString());
    if (autoDiscover) {
        controller->discoverServices();
    }
}

/*!
 * Handles `QLowEnergyController::discoveryFinished` events.
 *
 * As this event indicates that the conroller has finished discovering services, this function will
 * invoke createServiceObject() to create the internal service object (if not already created).
 */
void AbstractPokitServicePrivate::discoveryFinished()
{
    if (!controller) {
        qCWarning(lc).noquote() << tr("Discovery finished with no controller set") << sender();
        return;
    }

    qCDebug(lc).noquote() << tr(R"(Discovery finished for "%1" (%2) at %3.)").arg(
        controller->remoteName(), controller->remoteDeviceUuid().toString(),
        controller->remoteAddress().toString());

    if (!createServiceObject()) {
        qCWarning(lc).noquote() << tr("Discovery finished, but service not found.");
        Q_Q(AbstractPokitService);
        Q_EMIT q->serviceErrorOccurred(QLowEnergyService::ServiceError::UnknownError);
    }
}

/*!
 * Handles `QLowEnergyController::errorOccurred` events.
 *
 * This function simply re-emits \a newError as AbstractPokitService::serviceErrorOccurred.
 */
void AbstractPokitServicePrivate::errorOccurred(const QLowEnergyService::ServiceError newError)
{
    Q_Q(AbstractPokitService);
    qCDebug(lc).noquote() << tr("Service error") << newError;
    Q_EMIT q->serviceErrorOccurred(newError);
}

/*!
 * Handles `QLowEnergyController::serviceDiscovered` events.
 *
 * If the discovered service is the one this (or rather the derived) class wraps, then
 * createServiceObject() will be invoked immediately (otherwise it will be invoked after full
 * service discovery has completed, ie in discoveryFinished()).
 */
void AbstractPokitServicePrivate::serviceDiscovered(const QBluetoothUuid &newService)
{
    if ((!service) && (newService == serviceUuid)) {
        qCDebug(lc).noquote() << tr("Service discovered") << newService;
        createServiceObject();
    }
}

/*!
 * Handles `QLowEnergyController::stateChanged` events.
 *
 * If \a newState indicates that service details have now been discovered, then
 * AbstractPokitService::serviceDetailsDiscovered will be emitted.
 *
 * \see AbstractPokitService::autoDiscover()
 */
void AbstractPokitServicePrivate::stateChanged(QLowEnergyService::ServiceState newState)
{
    qCDebug(lc).noquote() << tr("State changed to") << newState;

    if (lc().isDebugEnabled()) {
        for (const auto &characteristic: service->characteristics()) {
            QStringList properties;
            /// \cond no-doxygen
            #define QTPOKIT_INTERNAL_TEST_AND_APPEND(property) \
            if (characteristic.properties().testFlag(QLowEnergyCharacteristic::property)) { \
                properties.append(QStringLiteral(#property).toLower());\
            }
            /// \endcond
            QTPOKIT_INTERNAL_TEST_AND_APPEND(Broadcasting)
            QTPOKIT_INTERNAL_TEST_AND_APPEND(Read)
            QTPOKIT_INTERNAL_TEST_AND_APPEND(WriteNoResponse)
            QTPOKIT_INTERNAL_TEST_AND_APPEND(Write)
            QTPOKIT_INTERNAL_TEST_AND_APPEND(Notify)
            QTPOKIT_INTERNAL_TEST_AND_APPEND(Indicate)
            QTPOKIT_INTERNAL_TEST_AND_APPEND(WriteSigned)
            QTPOKIT_INTERNAL_TEST_AND_APPEND(ExtendedProperty)
            #undef QTPOKIT_INTERNAL_TEST_AND_APPEND
            qCDebug(lc).noquote() << tr(R"(Characteristic %1 "%2" supports %3.)").arg(characteristic.uuid().toString(),
                PokitDevice::charcteristicToString(characteristic.uuid()), properties.join(QStringLiteral(", ")));
        }
    }

    if (newState == QLowEnergyService::
            #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            ServiceDiscovered
            #else
            RemoteServiceDiscovered
            #endif
        ) {
        Q_Q(AbstractPokitService);
        qCDebug(lc).noquote() << tr("Service details discovered.");
        Q_EMIT q->serviceDetailsDiscovered();
    }
}

/*!
 * Handles `QLowEnergyService::characteristicRead` events. This base implementation simply debug
 * logs the event.
 *
 * Derived classes should implement this function to handle the successful reads of
 * \a characteristic, typically by parsing \a value, then emitting a speciailised signal.
 */
void AbstractPokitServicePrivate::characteristicRead(
    const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(lc).noquote() << tr(R"(Characteristic %1 "%2" read %n byte/s: %3)", nullptr, value.size()).arg(
        characteristic.uuid().toString(), PokitDevice::charcteristicToString(characteristic.uuid()), toHexString(value));
}

/*!
 * Handles `QLowEnergyService::characteristicWritten` events. This base implementation simply debug
 * logs the event.
 *
 * Derived classes should implement this function to handle the successful writes of
 * \a characteristic, typically by parsing \a newValue, then emitting a speciailised signal.
 */
void AbstractPokitServicePrivate::characteristicWritten(
    const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    qCDebug(lc).noquote() << tr(R"(Characteristic %1 "%2" written with %Ln byte/s: %3)", nullptr, newValue.size())
        .arg(characteristic.uuid().toString(), PokitDevice::charcteristicToString(characteristic.uuid()), toHexString(newValue));
}

/*!
 * Handles `QLowEnergyService::characteristicChanged` events. This base implementation simply debug
 * logs the event.
 *
 * If derived classes support characteristics with client-side notification (ie Notify, as opposed
 * to Read or Write operations), they should implement this function to handle the successful reads of
 * \a characteristic, typically by parsing \a value, then emitting a speciailised signal.
 */
void AbstractPokitServicePrivate::characteristicChanged(
    const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    qCDebug(lc).noquote() << tr(R"(Characteristic %1 "%2" changed to %Ln byte/s: %3)", nullptr, newValue.size())
        .arg(characteristic.uuid().toString(), PokitDevice::charcteristicToString(characteristic.uuid()), toHexString(newValue));
}

/// \endcond
