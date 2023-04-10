# SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.0)

set(PokitAppSources
  abstractcommand.cpp
  abstractcommand.h
  calibratecommand.cpp
  calibratecommand.h
  devicecommand.cpp
  devicecommand.h
  dsocommand.cpp
  dsocommand.h
  flashledcommand.cpp
  flashledcommand.h
  infocommand.cpp
  infocommand.h
  loggerfetchcommand.cpp
  loggerfetchcommand.h
  loggerstartcommand.cpp
  loggerstartcommand.h
  loggerstopcommand.cpp
  loggerstopcommand.h
  metercommand.cpp
  metercommand.h
  scancommand.cpp
  scancommand.h
  setnamecommand.cpp
  setnamecommand.h
  statuscommand.cpp
  statuscommand.h
)

if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
  add_executable(pokit main.cpp ${PokitAppSources})
else()
  # We put all but main.cpp into a shared 'object' library so
  # our app unit tests can link against the same binary objects.
  add_library(PokitApp OBJECT ${PokitAppSources})

  target_link_libraries(
    PokitApp
    PRIVATE QtPokit
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

  add_executable(pokit main.cpp)

  target_link_libraries(pokit PRIVATE PokitApp)
endif()

qt_add_translations(
  pokit TS_FILES
    i18n/en_US.ts
  SOURCES main.cpp ${PokitAppSources})

target_link_libraries(
  pokit
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
