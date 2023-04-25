# SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.0)

set(DokitCliSources
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
  add_executable(dokit main.cpp ${DokitCliSources})
else()
  # We put all but main.cpp into a shared 'object' library so
  # our app unit tests can link against the same binary objects.
  add_library(DokitCli OBJECT ${DokitCliSources})

  target_link_libraries(
    DokitCli
    PRIVATE QtPokit
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

  add_executable(dokit main.cpp)

  target_link_libraries(dokit PRIVATE DokitCli)
endif()

# Only include localisation for Qt6+ for now. We could support Qt5
# too, but we'd need to manually implement some of Qt6's CMake
# functions, which would be easier if we had a bespoke qrc file.
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_translations(
    dokit TS_FILES
      i18n/en_AU.ts
      i18n/en_GB.ts
      i18n/en_US.ts
    RESOURCE_PREFIX "i18n/cli"
    SOURCES main.cpp ${DokitCliSources})
endif()

target_link_libraries(
  dokit
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
