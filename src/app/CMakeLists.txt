# SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.12)

# We put all but main.cpp into a shared 'object' library so
# our app unit tests can link against the same binary objects.
add_library(PokitApp OBJECT
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

target_link_libraries(
  PokitApp
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

add_executable(pokit main.cpp)

target_link_libraries(
  pokit
  PRIVATE PokitApp
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
