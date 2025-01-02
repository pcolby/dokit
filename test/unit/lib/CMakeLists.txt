# SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

add_dokit_unit_test(
  AbstractPokitService
  testabstractpokitservice.cpp
  testabstractpokitservice.h)

add_dokit_unit_test(
  CalibrationService
  testcalibrationservice.cpp
  testcalibrationservice.h)

add_dokit_unit_test(
  DataLoggerService
  testdataloggerservice.cpp
  testdataloggerservice.h)

add_dokit_unit_test(
  DeviceInfoService
  testdeviceinfoservice.cpp
  testdeviceinfoservice.h)

add_dokit_unit_test(
  DsoService
  testdsoservice.cpp
  testdsoservice.h)

add_dokit_unit_test(
  MultimeterService
  testmultimeterservice.cpp
  testmultimeterservice.h)

add_dokit_unit_test(
  PokitDevice
  testpokitdevice.cpp
  testpokitdevice.h)

add_dokit_unit_test(
  PokitDiscoveryAgent
  testpokitdiscoveryagent.cpp
  testpokitdiscoveryagent.h)

add_dokit_unit_test(
  PokitMeter
  testpokitmeter.cpp
  testpokitmeter.h)

add_dokit_unit_test(
  PokitPro
  testpokitpro.cpp
  testpokitpro.h)

add_dokit_unit_test(
  PokitProducts
  testpokitproducts.cpp
  testpokitproducts.h)

add_dokit_unit_test(
  StatusService
  teststatusservice.cpp
  teststatusservice.h)
