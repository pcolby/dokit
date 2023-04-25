# SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

add_subdirectory(cli)
add_subdirectory(lib)

# Experimental (for now) GUI app, off by default.
option(ENABLE_GUI "Enable test coverage reporting" OFF)
if (ENABLE_GUI)
  message(STATUS "Enabling experimental GUI app")
  add_subdirectory(gui)
endif()

find_program(LINUXDEPLOY NAMES linuxdeploy linuxdeploy-x86_64.AppImage)
if (LINUXDEPLOY)
  message(STATUS "Found linuxdeploy: ${LINUXDEPLOY}")
  configure_file(desktop.in dokit.desktop)
  add_custom_target(appimage
    COMMAND ${CMAKE_COMMAND} -E env
      OUTPUT=${CMAKE_BINARY_DIR}/dokit-${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}.AppImage
      QMAKE=$<TARGET_FILE:Qt${QT_VERSION_MAJOR}::qmake>
      --
      ${LINUXDEPLOY}
      --appdir "${CMAKE_BINARY_DIR}/AppDir"
      --desktop-file "${CMAKE_CURRENT_BINARY_DIR}/dokit.desktop"
      --executable "$<TARGET_FILE:dokit>"
      --icon-file "${CMAKE_CURRENT_SOURCE_DIR}/dokit.png"
      --output appimage
      --plugin qt
    DEPENDS dokit QtPokit
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
