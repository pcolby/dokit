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
  add_executable(cli main.cpp ${DokitCliSources})
else()
  # We put all but main.cpp into a shared 'object' library so
  # our app unit tests can link against the same binary objects.
  add_library(cli-lib OBJECT ${DokitCliSources})

  target_link_libraries(
    cli-lib
    PRIVATE QtPokit
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

  add_executable(cli main.cpp)

  target_link_libraries(cli PRIVATE cli-lib)
endif()

set_target_properties(cli PROPERTIES OUTPUT_NAME dokit)

# Only include localisation for Qt6+ for now. We could support Qt5
# too, but we'd need to manually implement some of Qt6's CMake
# functions, which would be easier if we had a bespoke qrc file.
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_translations(
    cli TS_FILES
      i18n/en_AU.ts
      i18n/en_GB.ts
      i18n/en_US.ts
    RESOURCE_PREFIX "i18n/cli"
    SOURCES main.cpp ${DokitCliSources})
endif()

target_link_libraries(
  cli
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

find_program(LINUXDEPLOY NAMES linuxdeploy linuxdeploy-x86_64.AppImage)
if (LINUXDEPLOY)
  message(STATUS "Found linuxdeploy: ${LINUXDEPLOY}")
  configure_file(desktop.in dokit.desktop)
  add_custom_target(cli-appimage
    COMMAND ${CMAKE_COMMAND} -E env
      OUTPUT=${CMAKE_BINARY_DIR}/dokit-${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}.AppImage
      QMAKE=$<TARGET_FILE:Qt${QT_VERSION_MAJOR}::qmake>
      --
      ${LINUXDEPLOY}
      --appdir "${CMAKE_BINARY_DIR}/AppDir"
      --desktop-file "${CMAKE_CURRENT_BINARY_DIR}/dokit.desktop"
      --executable "$<TARGET_FILE:cli>"
      --icon-file "${CMAKE_CURRENT_SOURCE_DIR}/dokit.png"
      --output appimage
      --plugin qt
    DEPENDS dokit QtPokit
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
