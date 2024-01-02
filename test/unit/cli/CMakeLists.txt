# SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

function(add_dokit_cli_unit_test name)
if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
  message(STATUS "Skipping test${name} (needs CMake 3.12+)")
else()
  add_dokit_unit_test(${name} ${ARGN} outputstreamcapture.h testdata.h)
  set_tests_properties(${name} PROPERTIES LABELS "cli;unit")
  target_include_directories(test${name} PRIVATE ${CMAKE_SOURCE_DIR}/src/cli)
  target_link_libraries(test${name} PRIVATE cli-lib)
endif()
endfunction()

add_dokit_cli_unit_test(
  AbstractCommand
  testabstractcommand.cpp
  testabstractcommand.h)

add_dokit_cli_unit_test(
  CalibrateCommand
  testcalibratecommand.cpp
  testcalibratecommand.h)

add_dokit_cli_unit_test(
  DeviceCommand
  testdevicecommand.cpp
  testdevicecommand.h)

add_dokit_cli_unit_test(
  DsoCommand
  testdsocommand.cpp
  testdsocommand.h)

add_dokit_cli_unit_test(
  FlashLedCommand
  testflashledcommand.cpp
  testflashledcommand.h)

add_dokit_cli_unit_test(
  InfoCommand
  testinfocommand.cpp
  testinfocommand.h)

add_dokit_cli_unit_test(
  LoggerFetchCommand
  testloggerfetchcommand.cpp
  testloggerfetchcommand.h)

add_dokit_cli_unit_test(
  LoggerStartCommand
  testloggerstartcommand.cpp
  testloggerstartcommand.h)

add_dokit_cli_unit_test(
  LoggerStopCommand
  testloggerstopcommand.cpp
  testloggerstopcommand.h)

add_dokit_cli_unit_test(
  MeterCommand
  testmetercommand.cpp
  testmetercommand.h)

add_dokit_cli_unit_test(
  ScanCommand
  testscancommand.cpp
  testscancommand.h)

add_dokit_cli_unit_test(
  SetNameCommand
  testsetnamecommand.cpp
  testsetnamecommand.h)

add_dokit_cli_unit_test(
  SetTorchCommand
  testsettorchcommand.cpp
  testsettorchcommand.h)

add_dokit_cli_unit_test(
  StatusCommand
  teststatuscommand.cpp
  teststatuscommand.h)
