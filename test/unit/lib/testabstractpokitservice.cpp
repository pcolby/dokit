// SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testabstractpokitservice.h"

#include <qtpokit/abstractpokitservice.h>
#include "abstractpokitservice_p.h"

#include <QLowEnergyController>
#include <QRegularExpression>
#include <QSignalSpy>

Q_DECLARE_METATYPE(PokitProduct)

class MockPokitService : public AbstractPokitService
{
public:
    explicit MockPokitService(QLowEnergyController * const controller, QObject * parent = nullptr)
        : AbstractPokitService(new AbstractPokitServicePrivate(QBluetoothUuid::createUuid(),
            controller, this), parent)
    {

    }

    bool readCharacteristics() override { return false; } // Ignored by our tests.
};

void TestAbstractPokitService::autoDiscover()
{
    MockPokitService service(nullptr);
    QVERIFY(service.autoDiscover()); // On, by default.
    service.setAutoDiscover(false);
    QVERIFY(!service.autoDiscover());
    service.setAutoDiscover(true);
    QVERIFY(service.autoDiscover());
}

void TestAbstractPokitService::pokitProduct_data()
{
    QTest::addColumn<PokitProduct>("product");
    QTest::addRow("Pokit Meter") << PokitProduct::PokitMeter;
    QTest::addRow("Pokit Pro") << PokitProduct::PokitPro;
}

void TestAbstractPokitService::pokitProduct()
{
    QFETCH(PokitProduct, product);
    MockPokitService service(nullptr);
    QVERIFY(!service.pokitProduct());
    service.setPokitProduct(product);
    QVERIFY(service.pokitProduct());
    QVERIFY(service.pokitProduct() == product);
}

// Underscore suffix is just to prevent local `service` variables shadowing this function.
void TestAbstractPokitService::service_()
{
    MockPokitService service(nullptr);
    QCOMPARE(service.service(), nullptr);
    const MockPokitService constService(nullptr);
    QCOMPARE(constService.service(), nullptr);
}

void TestAbstractPokitService::createServiceObject()
{
    // Verify that creation will fail without a Bluetooth device controller
    MockPokitService service(nullptr);
    QCOMPARE(service.service(), nullptr);
    QVERIFY(!service.d_ptr->createServiceObject());
    QCOMPARE(service.service(), nullptr);

    // Verify that creation will still fail without a connected Bluetooth device.
    service.d_ptr->controller = QLowEnergyController::createCentral(QBluetoothDeviceInfo(), &service);
    QVERIFY(!service.d_ptr->createServiceObject());
    QCOMPARE(service.service(), nullptr);

    // Verify that existing, assigned BLE services will be left intact (even if they are completely
    // invalid pointers we just re-intrerpreted... dangerous, but only short-lived for testing).
    // Note, we QCOMPARE with anonymous pointers below, because if our hacked pointer is presented
    // as a QObject-derived type then QCOMPARE will attempt to invoke QObject::objectName() on it.
    char x[sizeof(QLowEnergyService)];
    service.d_ptr->service = reinterpret_cast<QLowEnergyService *>(&x);
    QCOMPARE((std::uintptr_t)service.service(), (std::uintptr_t)(&x));
    QVERIFY(service.d_ptr->createServiceObject());
    QCOMPARE((std::uintptr_t)service.service(), (std::uintptr_t)(&x));
    service.d_ptr->service = nullptr; // Just in case we ever add more code here, and forget to.
}

void TestAbstractPokitService::getCharacteristic()
{
    {   // Verify an invalid characteristic is returned safely, when no controller is set.
        MockPokitService service(nullptr);
        const QLowEnergyCharacteristic characteristic = service.d_ptr->getCharacteristic(QUuid::createUuid());
        QVERIFY(!characteristic.isValid());
    }

    {   // Verify an invalid characteristic is returned safely, when a device controller is set, but
        // no services have been discovered (because we have no Bluetooth device to talk to).
        MockPokitService service(QLowEnergyController::createCentral(QBluetoothDeviceInfo()));
        QVERIFY(!service.d_ptr->createServiceObject());
        const QLowEnergyCharacteristic characteristic = service.d_ptr->getCharacteristic(QUuid::createUuid());
        QVERIFY(!characteristic.isValid());
    }
}

void TestAbstractPokitService::readCharacteristic()
{
    // Verify that characteristic reads fail safely, when no Bluetooth device is connected.
    MockPokitService service(nullptr);
    QVERIFY(!service.d_ptr->readCharacteristic(QUuid::createUuid()));
}

void TestAbstractPokitService::enableCharacteristicNotificatons()
{
    // Verify that CCCD-enable writes fail safely, when no Bluetooth device is connected.
    MockPokitService service(nullptr);
    QVERIFY(!service.d_ptr->enableCharacteristicNotificatons(QUuid::createUuid()));
}

void TestAbstractPokitService::disableCharacteristicNotificatons()
{
    // Verify that CCCD-disable writes fail safely, when no Bluetooth device is connected.
    MockPokitService service(nullptr);
    QVERIFY(!service.d_ptr->disableCharacteristicNotificatons(QUuid::createUuid()));
}

void TestAbstractPokitService::checkSize_data()
{
    QTest::addColumn<QString>("label");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("min");
    QTest::addColumn<int>("max");
    QTest::addColumn<bool>("failOnMax");
    QTest::addColumn<bool>("expected");

    QTest::addRow("ok") << QStringLiteral("<label>") << QByteArray(5, '\x01')
        << 2 << 10 << false << true;

    QTest::addRow("too-short") << QStringLiteral("<label>") << QByteArray("\x01")
        << 2 << 10 << false << false;

    QTest::addRow("long") << QStringLiteral("<label>") << QByteArray(20, '\x01')
        << 2 << 10 << false << true;

    QTest::addRow("too-long") << QStringLiteral("<label>") << QByteArray(20,'\x01')
        << 2 << 10 << true << false;
}

void TestAbstractPokitService::checkSize()
{
    QFETCH(QString, label);
    QFETCH(QByteArray, data);
    QFETCH(int, min);
    QFETCH(int, max);
    QFETCH(bool, failOnMax);
    QFETCH(bool, expected);
    if (data.size() < min) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            R"(^<label> requires \d+ byte/s, but only \d+ present: 0x[a-zA-Z0-9,]*$)")));
    }
    if (data.size() > max) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral(
            R"(^<label> has \d+ extraneous byte/s: 0x[a-zA-Z0-9,]*$)")));
    }
    QCOMPARE(AbstractPokitServicePrivate::checkSize(label, data, min, max, failOnMax), expected);
}


void TestAbstractPokitService::toHexString_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("max");
    QTest::addColumn<QString>("expected");

    QTest::addRow("empty") << QByteArray() << 10
        << QStringLiteral("0x");

    QTest::addRow("ok") << QByteArray("\x01\x02\x03\x04\x05") << 10
        << QStringLiteral("0x01,02,03,04,05");

    QTest::addRow("just-ok") << QByteArray("\x01\x02\x03\x04\x05") << 5
        << QStringLiteral("0x01,02,03,04,05");

    QTest::addRow("just-over") << QByteArray("\x01\x02\x03\x04\x05") << 4
        << QStringLiteral("0x01,...,05");

    QTest::addRow("well-over") << QByteArray(1000, '\x01') << 10
        << QStringLiteral("0x01,01,01,01,...,01,01,01,01");
}

void TestAbstractPokitService::toHexString()
{
    QFETCH(QByteArray, data);
    QFETCH(int, max);
    QFETCH(QString, expected);
    QCOMPARE(AbstractPokitServicePrivate::toHexString(data, max), expected);
}

void TestAbstractPokitService::connected()
{
    // Verify safe error handling.
    {
        MockPokitService service(nullptr);
        service.d_ptr->connected();
    }
    {
        MockPokitService service(QLowEnergyController::createCentral(QBluetoothDeviceInfo()));
        service.d_ptr->connected();
    }
}

void TestAbstractPokitService::discoveryFinished()
{
    // Verify safe error handling.
    {
        MockPokitService service(nullptr);
        QTest::ignoreMessage(QtWarningMsg, "Discovery finished with no controller set QObject(0x0)");
        service.d_ptr->discoveryFinished();
    }
    {
        MockPokitService service(QLowEnergyController::createCentral(QBluetoothDeviceInfo()));
        service.d_ptr->discoveryFinished();
    }
}

void TestAbstractPokitService::errorOccurred()
{
    // Verify that errors bubble up.
    MockPokitService service(nullptr);
    QSignalSpy spy(&service, &AbstractPokitService::serviceErrorOccurred);
    service.d_ptr->errorOccurred(QLowEnergyService::ServiceError::UnknownError);
    QCOMPARE(spy.count(), 1);
}

void TestAbstractPokitService::serviceDiscovered()
{
    MockPokitService service(nullptr);
    service.d_ptr->serviceDiscovered(QBluetoothUuid::createUuid()); // Slightly different code paths
    service.d_ptr->serviceDiscovered(service.d_ptr->serviceUuid);   // but both must be safe.
}

void TestAbstractPokitService::stateChanged()
{
    // Verify that signals with [Remote]ServiceDiscovered state bubble up.
    MockPokitService service(nullptr);
    QSignalSpy spy(&service, &AbstractPokitService::serviceDetailsDiscovered);

    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    service.d_ptr->stateChanged(QLowEnergyService::ServiceState::DiscoveryRequired);
    service.d_ptr->stateChanged(QLowEnergyService::ServiceState::DiscoveringServices);
    #else
    service.d_ptr->stateChanged(QLowEnergyService::ServiceState::RemoteService);
    service.d_ptr->stateChanged(QLowEnergyService::ServiceState::RemoteServiceDiscovering);
    #endif
    QCOMPARE(spy.count(), 0); // No signals yet.

    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    service.d_ptr->stateChanged(QLowEnergyService::ServiceState::ServiceDiscovered);
    #else
    service.d_ptr->stateChanged(QLowEnergyService::ServiceState::RemoteServiceDiscovered);
    #endif
    QCOMPARE(spy.count(), 1); // Signaled.
}

void TestAbstractPokitService::characteristicRead()
{
    // Verify safe error handling.
    MockPokitService service(nullptr);
    const QLowEnergyCharacteristic characteristic =
        service.d_ptr->getCharacteristic(QUuid::createUuid());
    service.d_ptr->characteristicRead(characteristic, QByteArray());
}

void TestAbstractPokitService::characteristicWritten()
{
    // Verify safe error handling.
    MockPokitService service(nullptr);
    const QLowEnergyCharacteristic characteristic =
        service.d_ptr->getCharacteristic(QUuid::createUuid());
    service.d_ptr->characteristicWritten(characteristic, QByteArray());
}

void TestAbstractPokitService::characteristicChanged()
{
    // Verify safe error handling.
    MockPokitService service(nullptr);
    const QLowEnergyCharacteristic characteristic =
        service.d_ptr->getCharacteristic(QUuid::createUuid());
    service.d_ptr->characteristicChanged(characteristic, QByteArray());
}

void TestAbstractPokitService::tr()
{
    // Exercise the inline tr() function (added by the Q_OBJECT macro) to avoid false negatives in
    // test coverage.  There is no need to actually test tr() here, since its part of the Qt API.
    MockPokitService service(nullptr);
    QVERIFY(!service.tr("ignored").isEmpty());
}

QTEST_MAIN(TestAbstractPokitService)
