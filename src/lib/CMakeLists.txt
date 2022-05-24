# SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.0)

add_library(QtPokit SHARED)

target_sources(
  QtPokit
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/abstractpokitservice.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/calibrationservice.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/dataloggerservice.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/deviceinfoservice.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/dsoservice.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/genericaccessservice.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/multimeterservice.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitdevice.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitdiscoveryagent.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/qtpokit_global.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/statusservice.h
  PRIVATE abstractpokitservice.cpp
  PRIVATE abstractpokitservice_p.h
  PRIVATE calibrationservice.cpp
  PRIVATE calibrationservice_p.h
  PRIVATE dataloggerservice.cpp
  PRIVATE dataloggerservice_p.h
  PRIVATE deviceinfoservice.cpp
  PRIVATE deviceinfoservice_p.h
  PRIVATE dsoservice.cpp
  PRIVATE dsoservice_p.h
  PRIVATE genericaccessservice.cpp
  PRIVATE genericaccessservice_p.h
  PRIVATE multimeterservice.cpp
  PRIVATE multimeterservice_p.h
  PRIVATE pokitdevice.cpp
  PRIVATE pokitdevice_p.h
  PRIVATE pokitdiscoveryagent.cpp
  PRIVATE pokitdiscoveryagent_p.h
  PRIVATE statusservice.cpp
  PRIVATE statusservice_p.h
)

target_include_directories(QtPokit PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(
  QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

target_compile_definitions(QtPokit PRIVATE QTPOKIT_LIBRARY)
