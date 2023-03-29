# SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

add_subdirectory(app)
add_subdirectory(lib)

find_program(LINUXDEPLOY NAMES linuxdeploy linuxdeploy-x86_64.AppImage)
if (LINUXDEPLOY)
  message(STATUS "Found linuxdeploy: ${LINUXDEPLOY}")
  configure_file(desktop.in pokit.desktop)
  add_custom_target(appimage
    COMMAND ${CMAKE_COMMAND} -E env
      OUTPUT=${CMAKE_BINARY_DIR}/pokit-${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}.AppImage
      QMAKE=$<TARGET_FILE:Qt${QT_VERSION_MAJOR}::qmake>
      --
      ${LINUXDEPLOY}
      --appdir "${CMAKE_BINARY_DIR}/AppDir"
      --desktop-file "${CMAKE_CURRENT_BINARY_DIR}/pokit.desktop"
      --executable "$<TARGET_FILE:pokit>"
      --icon-file "${CMAKE_CURRENT_SOURCE_DIR}/pokit.png"
      --output appimage
      --plugin qt
    DEPENDS pokit QtPokit
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
