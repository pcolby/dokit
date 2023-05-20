# SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.0)

# \todo Move this to the top-level CMakeLists.txt when the GUI app is ready.
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts Widgets)
message(STATUS "Found Qt Charts ${Qt${QT_VERSION_MAJOR}Charts_VERSION}")
message(STATUS "Found Qt Widgets ${Qt${QT_VERSION_MAJOR}Widgets_VERSION}")

set(DokitGuiSources
  mainwindow.cpp
  mainwindow.h
  models/pokitdevicesmodel.cpp
  models/pokitdevicesmodel.h
  resources.cpp
  resources.h
)

if((${QT_VERSION_MAJOR} EQUAL 5) AND (${QT_VERSION_MINOR} LESS 15))
  qt5_add_resources(DokitGuiSources qrc/icons.qrc)
else()
  qt_add_resources(DokitGuiSources qrc/icons.qrc)
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
  add_executable(gui main.cpp ${DokitGuiSources})
else()
  # We put all but main.cpp into a shared 'object' library so
  # our app unit tests can link against the same binary objects.
  add_library(gui-lib OBJECT ${DokitGuiSources})

  target_link_libraries(
    gui-lib
    PRIVATE QtPokit
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth
    PRIVATE Qt${QT_VERSION_MAJOR}::Charts
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

  add_executable(gui main.cpp)

  target_link_libraries(gui PRIVATE gui-lib)
endif()

set_target_properties(gui PROPERTIES OUTPUT_NAME Dokit)

# Only include localisation for Qt6+ for now. We could support Qt5
# too, but we'd need to manually implement some of Qt6's CMake
# functions, which would be easier if we had a bespoke qrc file.
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_translations(
    gui TS_FILES
      i18n/en_AU.ts
      i18n/en_GB.ts
      i18n/en_US.ts
    RESOURCE_PREFIX "i18n/gui"
    SOURCES main.cpp ${DokitGuiSources})
endif()

target_link_libraries(
  gui
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth
  PRIVATE Qt${QT_VERSION_MAJOR}::Charts
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
