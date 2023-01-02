# SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.0)

add_library(
  QtPokit SHARED
  ${CMAKE_SOURCE_DIR}/include/qtpokit/abstractpokitservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/calibrationservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/dataloggerservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/deviceinfoservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/dsoservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/genericaccessservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/multimeterservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitdevice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitdiscoveryagent.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/qtpokit_global.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/statusservice.h
  abstractpokitservice.cpp
  abstractpokitservice_p.h
  calibrationservice.cpp
  calibrationservice_p.h
  dataloggerservice.cpp
  dataloggerservice_p.h
  deviceinfoservice.cpp
  deviceinfoservice_p.h
  dsoservice.cpp
  dsoservice_p.h
  genericaccessservice.cpp
  genericaccessservice_p.h
  multimeterservice.cpp
  multimeterservice_p.h
  pokitdevice.cpp
  pokitdevice_p.h
  pokitdiscoveryagent.cpp
  pokitdiscoveryagent_p.h
  statusservice.cpp
  statusservice_p.h
)

target_include_directories(QtPokit PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(
  QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

target_compile_definitions(QtPokit PRIVATE QTPOKIT_LIBRARY)
