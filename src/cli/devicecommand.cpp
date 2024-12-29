// SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicecommand.h"

#include <qtpokit/abstractpokitservice.h>
#include <qtpokit/pokitdevice.h>
#include <qtpokit/pokitdiscoveryagent.h>
#include "qtpokit/pokitmeter.h"
#include "qtpokit/pokitpro.h"

/*!
 * \class DeviceCommand
 *
 * The AbstractCommand class extends AbstractCommand to add a PokitDevice instance.
 */

/*!
 * Construct a new DeviceCommand object with \a parent.
 */
DeviceCommand::DeviceCommand(QObject * const parent) : AbstractCommand(parent)
{

}

/*!
 * Begins scanning for the Pokit device.
 */
bool DeviceCommand::start()
{
    qCInfo(lc).noquote() << ((deviceToScanFor.isNull())
        ? tr("Looking for first available Pokit device...")
        : tr(R"(Looking for device "%1"...)").arg(deviceToScanFor));
    discoveryAgent->start();
    return true;
}

/*!
 * Disconnects the underlying Pokit device, and sets \a exitCode to be return to the OS once the
 * disconnection has taken place.
 */
void DeviceCommand::disconnect(int exitCode)
{
    qCDebug(lc).noquote() << tr("Disconnecting Pokit device...");
    Q_ASSERT(device);
    Q_ASSERT(device->controller());
    exitCodeOnDisconnect = exitCode;
    device->controller()->disconnectFromDevice();
}

/*!
 * \fn virtual AbstractPokitService * DeviceCommand::getService() = 0
 *
 * Returns a Pokit service object for the derived command class. This will be called by
 * deviceDiscovered() when the requested Pokit device has been found, after which
 * deviceDiscovered() will connect the returned service's common signals, and kick off the
 * device's connection process.
 */

#define DOKIT_CLI_IF_LESS_THAN_RETURN(value, ns, label) \
if (value <= ns::maxValue(label)) { \
    return label; \
}

/**
 * \fn template<typename T> static T DeviceCommand::minRange(const quint32 maxValue)
 *
 * Returns the lowest \a T range that can measure at least up to \a maxValue, or AutoRange if no such range is
 * available.
 *
 * \tparam T Range enumerator to evaluate ranges for. Must be one of:
 *   * PokitMeter::CurrentRange
 *   * PokitMeter::ResistanceRange
 *   * PokitMeter::VoltageRange
 *   * PokitPro::CapacitanceRange
 *   * PokitPro::CurrentRange
 *   * PokitPro::ResistanceRange
 *   * PokitPro::VoltageRange
 *
 * \cond Doxygen has "only very limited support for member specialization at the moment", so hide these from Doxygen.
 * Specifically, if we don't hide them, then Doxygen (at least the current version: v1.9.6) sees the following
 * specialisations as new, public, non-static members.
 */

/*!
 * Returns the lowest PokitMeter::CurrentRange value that can measure at least up to \a maxValue, or AutoRange if
 * the Pokit Meter cannot measure as high as \a maxValue.
 */
template<> PokitMeter::CurrentRange DeviceCommand::minRange<>(const quint32 maxValue)
{
    if (maxValue == 0) return PokitMeter::CurrentRange::AutoRange;
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::CurrentRange::_10mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::CurrentRange::_30mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::CurrentRange::_150mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::CurrentRange::_300mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::CurrentRange::_2A)
    return PokitMeter::CurrentRange::AutoRange;
}

/*!
 * Returns the lowest PokitMeter::ResistanceRange value that can measure at least up to \a maxValue, or AutoRange if
 * the Pokit Meter cannot measure as high as \a maxValue.
 */
template<> PokitMeter::ResistanceRange DeviceCommand::minRange(const quint32 maxValue)
{
    if (maxValue == 0) return PokitMeter::ResistanceRange::AutoRange;
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::ResistanceRange::_160)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::ResistanceRange::_330)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::ResistanceRange::_890)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::ResistanceRange::_1K5)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::ResistanceRange::_10K)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::ResistanceRange::_100K)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::ResistanceRange::_470K)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::ResistanceRange::_1M)
    return PokitMeter::ResistanceRange::AutoRange;
}

/*!
 * Returns the lowest PokitMeter::VoltageRange value that can measure at least up to \a maxValue, or AutoRange if
 * the Pokit Meter cannot measure as high as \a maxValue.
 */
template<> PokitMeter::VoltageRange DeviceCommand::minRange(const quint32 maxValue)
{
    if (maxValue == 0) return PokitMeter::VoltageRange::AutoRange;
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::VoltageRange::_300mV)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::VoltageRange::_2V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::VoltageRange::_6V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::VoltageRange::_12V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::VoltageRange::_30V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitMeter, PokitMeter::VoltageRange::_60V)
    return PokitMeter::VoltageRange::AutoRange;
}

/*!
 * Returns the lowest PokitPro::CapacitanceRange value that can measure at least up to \a maxValue, or AutoRange if
 * the Pokit Pro cannot measure as high as \a maxValue.
 */
template<> PokitPro::CapacitanceRange DeviceCommand::minRange(const quint32 maxValue)
{
    if (maxValue == 0) return PokitPro::CapacitanceRange::AutoRange;
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CapacitanceRange::_100nF)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CapacitanceRange::_10uF)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CapacitanceRange::_1mF)
    return PokitPro::CapacitanceRange::AutoRange;
}

/*!
 * Returns the lowest PokitPro::CurrentRange value that can measure at least up to \a maxValue, or AutoRange if
 * the Pokit Pro cannot measure as high as \a maxValue.
 */
template<> PokitPro::CurrentRange DeviceCommand::minRange(const quint32 maxValue)
{
    if (maxValue == 0) return PokitPro::CurrentRange::AutoRange;
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CurrentRange::_500uA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CurrentRange::_2mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CurrentRange::_10mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CurrentRange::_125mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CurrentRange::_300mA)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CurrentRange::_3A)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::CurrentRange::_10A)
    return PokitPro::CurrentRange::AutoRange;
}

/*!
 * Returns the lowest PokitPro::ResistanceRange value that can measure at least up to \a maxValue, or AutoRange if
 * the Pokit Pro cannot measure as high as \a maxValue.
 */
template<> PokitPro::ResistanceRange DeviceCommand::minRange(const quint32 maxValue)
{
    if (maxValue == 0) return PokitPro::ResistanceRange::AutoRange;
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_30)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_75)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_400)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_5K)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_10K)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_15K)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_40K)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_500K)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_700K)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_1M)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::ResistanceRange::_3M)
    return PokitPro::ResistanceRange::AutoRange;
}

/*!
 * Returns the lowest PokitPro::VoltageRange value that can measure at least up to \a maxValue, or AutoRange if
 * the Pokit Pro cannot measure as high as \a maxValue.
 */
template<> PokitPro::VoltageRange DeviceCommand::minRange(const quint32 maxValue)
{
    if (maxValue == 0) return PokitPro::VoltageRange::AutoRange;
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::VoltageRange::_250mV)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::VoltageRange::_2V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::VoltageRange::_10V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::VoltageRange::_30V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::VoltageRange::_60V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::VoltageRange::_125V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::VoltageRange::_400V)
    DOKIT_CLI_IF_LESS_THAN_RETURN(maxValue, PokitPro, PokitPro::VoltageRange::_600V)
    return PokitPro::VoltageRange::AutoRange;
}

/// \endcond

/*!
 * Returns the \a product's lowest capacitance range that can measure at least up to \a maxValue (nF), or AutoRange if
 * no such range is available.
 *
 * \note Since Pokit Meters do not support capacitance measurement, \a product should not be PokitProduct::PokitMeter.
 *
 * \see minRange<PokitPro::CapacitanceRange>
 */
quint8 DeviceCommand::minCapacitanceRange(const PokitProduct product, const quint32 maxValue)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        Q_ASSERT_X(false, "DeviceCommand::minCapacitanceRange", "Pokit Meter has no capacitance support.");
        return 255;
    case PokitProduct::PokitPro:
        return +minRange<PokitPro::CapacitanceRange>(maxValue);
    }
    Q_ASSERT_X(false, "DeviceCommand::minCapacitanceRange", "Unknown PokitProduct enum value");
    return 255;
}

/*!
 * Returns the \a product's lowest current range that can measure at least up to \a maxValue (µA), or AutoRange if no
 * such range is available.
 *
 * \see DeviceCommand::minRange<PokitMeter::CurrentRange>(const quint32 maxValue)
 * \see minRange<PokitPro::CurrentRange>(const quint32 maxValue)
 */
quint8 DeviceCommand::minCurrentRange(const PokitProduct product, const quint32 maxValue)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return +minRange<PokitMeter::CurrentRange>(maxValue);
    case PokitProduct::PokitPro:
        return +minRange<PokitPro::CurrentRange>(maxValue);
    }
    Q_ASSERT_X(false, "DeviceCommand::minCurrentRange", "Unknown PokitProduct enum value");
    return 255;
}

/*!
 * Returns the \a product's lowest resistance range that can measure at least up to \a maxValue (Ω), or AutoRange if no
 * such range is available.
 *
 * \see DeviceCommand::minRange<PokitMeter::ResistanceRange>(const quint32 maxValue)
 * \see minRange<PokitPro::ResistanceRange>(const quint32 maxValue)
 */
quint8 DeviceCommand::minResistanceRange(const PokitProduct product, const quint32 maxValue)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return +minRange<PokitMeter::ResistanceRange>(maxValue);
    case PokitProduct::PokitPro:
        return +minRange<PokitPro::ResistanceRange>(maxValue);
    }
    Q_ASSERT_X(false, "DeviceCommand::minResistanceRange", "Unknown PokitProduct enum value");
    return 255;
}

/*!
 t
* Returns the \a product's lowest voltage range that can measure at least up to \a maxValue (mV), or AutoRange if no
 * such range is available.
 *
 * \see DeviceCommand::minRange<PokitMeter::VoltageRange>(const quint32 maxValue)
 * \see minRange<PokitPro::VoltageRange>(const quint32 maxValue)
 */
quint8 DeviceCommand::minVoltageRange(const PokitProduct product, const quint32 maxValue)
{
    switch (product) {
    case PokitProduct::PokitMeter:
        return +minRange<PokitMeter::VoltageRange>(maxValue);
    case PokitProduct::PokitPro:
        return +minRange<PokitPro::VoltageRange>(maxValue);
    }
    Q_ASSERT_X(false, "DeviceCommand::minVoltageRange", "Unknown PokitProduct enum value");
    return 255;
}

#undef DOKIT_CLI_IF_LESS_THAN_RETURN


/*!
 * Handles controller error events. This base implementation simply logs \a error and then exits
 * with `EXIT_FAILURE`. Derived classes may override this slot to implement their own error
 * handing if desired.
 */
void DeviceCommand::controllerError(QLowEnergyController::Error error)
{
    qCWarning(lc).noquote() << tr("Bluetooth controller error:") << error;
    QCoreApplication::exit(EXIT_FAILURE);
}

/*!
 * Handles devics disconnection events. This base implementation simply logs and exits the
 * application (via QCoreApplication::exit) with the current exitCodeOnDisconnect value, which is
 * initialise to `EXIT_FAILURE` in the constructor, but should be set to `EXIT_SUCESS` if/when
 * the derived command class has completed its actions and requested the disconnection (as opposed
 * to a spontaneous disconnection on error).
 */
void DeviceCommand::deviceDisconnected()
{
    qCDebug(lc).noquote() << tr("Pokit device disconnected. Exiting with code %1.")
        .arg(exitCodeOnDisconnect);
    QCoreApplication::exit(exitCodeOnDisconnect);
}

/*!
 * Handles service error events. This base implementation simply logs \a error and then exits
 * with `EXIT_FAILURE`. Derived classes may override this slot to implement their own error
 * handing if desired.
 *
 * \note As this base class does not construct services (derived classed do), its up to the derived
 * classed to connect this slot to the relevant service's error signal if desired.
 */
void DeviceCommand::serviceError(const QLowEnergyService::ServiceError error)
{
    qCWarning(lc).noquote() << tr("Bluetooth service error:") << error;
    QCoreApplication::exit(EXIT_FAILURE);
}

/*!
 * Handles service detail discovery events. This base implementation simply logs the event, and
 * nothing more. Derived classes may (usually do) override this slot to provide their own processing
 * when a services details have been discovered.
 */
void DeviceCommand::serviceDetailsDiscovered()
{
    qCDebug(lc).noquote() << tr("Service details discovered.");
}

/*!
 * Checks if \a info is the device (if any) we're looking for, and if so, create a contoller and
 * service, and begins connecting to the device.
 */
void DeviceCommand::deviceDiscovered(const QBluetoothDeviceInfo &info)
{
    Q_ASSERT(isPokitProduct(info));

    if (device) {
        qCDebug(lc).noquote() << tr(R"(Ignoring additional Pokit device "%1" (%2) at (%3).)")
            .arg(info.name(), info.deviceUuid().toString(), info.address().toString());
        return;
    }

    if ((deviceToScanFor.isEmpty()) || (deviceToScanFor == info.name()) ||
        ((!info.address().isNull()) && (info.address() == QBluetoothAddress(deviceToScanFor))) ||
        ((!info.deviceUuid().isNull()) && (info.deviceUuid() == QBluetoothUuid(deviceToScanFor))))
    {
        qCDebug(lc).noquote() << tr(R"(Found Pokit device "%1" (%2) at (%3).)")
            .arg(info.name(), info.deviceUuid().toString(), info.address().toString());
        discoveryAgent->stop();

        device = new PokitDevice(info, this);
        connect(device->controller(), &QLowEnergyController::disconnected,
                this, &DeviceCommand::deviceDisconnected);
        connect(device->controller(),
            #if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
            QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
            #else
            &QLowEnergyController::errorOccurred,
            #endif
            this, &DeviceCommand::controllerError, Qt::QueuedConnection);

        AbstractPokitService * const service = getService();
        service->setPokitProduct(pokitProduct(info));

        Q_ASSERT(service);
        connect(service, &AbstractPokitService::serviceDetailsDiscovered,
                this, &DeviceCommand::serviceDetailsDiscovered);
        connect(service, &AbstractPokitService::serviceErrorOccurred,
                this, &DeviceCommand::serviceError);

        qCDebug(lc).noquote() << tr(R"(Connecting to %1 device "%2" (%3) at (%4).)").arg(
            toString(*service->pokitProduct()), info.name(), info.deviceUuid().toString(), info.address().toString());
        device->controller()->connectToDevice();
        return;
    }

    qCDebug(lc).noquote() << tr(R"(Ignoring non-matching Pokit device "%1" (%2) at (%3).)")
        .arg(info.name(), info.deviceUuid().toString(), info.address().toString());
    return;
}

/*!
 * Checks that the requested device was discovered, and if not, reports and error and exits.
 */
void DeviceCommand::deviceDiscoveryFinished()
{
    if (!device) {
        qCWarning(lc).noquote() << ((deviceToScanFor.isNull())
            ? tr("Failed to find any Pokit device.")
            : tr(R"(Failed to find device "%1".)").arg(deviceToScanFor));
        QCoreApplication::exit(EXIT_FAILURE);
    }
}
