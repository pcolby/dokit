# SPDX-FileCopyrightText: 2022-2026 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

add_library(
  QtPokit SHARED
  ${CMAKE_SOURCE_DIR}/include/qtpokit/abstractpokitservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/calibrationservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/dataloggerservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/deviceinfoservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/dsoservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/multimeterservice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitdevice.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitdiscoveryagent.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitmeter.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitpro.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitproducts.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/qtpokit_global.h
  ${CMAKE_SOURCE_DIR}/include/qtpokit/statusservice.h
  ${CMAKE_SOURCE_DIR}/src/stringliterals_p.h
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
  multimeterservice.cpp
  multimeterservice_p.h
  pokitdevice.cpp
  pokitdevice_p.h
  pokitdiscoveryagent.cpp
  pokitdiscoveryagent_p.h
  pokitmeter.cpp
  pokitpro.cpp
  pokitproducts.cpp
  pokitproducts_p.h
  statusservice.cpp
  statusservice_p.h
)

# Only include localisation for Qt6+ for now. We could support Qt5
# too, but we'd need to manually implement some of Qt6's CMake
# functions, which would be easier if we had a bespoke qrc file.
if(${QT_VERSION_MAJOR} EQUAL 6)
  if(${QT_VERSION_MINOR} GREATER_EQUAL 7) # Signature added in Qt 6.7.
    qt_add_translations(
      TARGETS QtPokit
      SOURCE_TARGETS QtPokit
      TS_FILES ${TranslationSourceFiles}
      RESOURCE_PREFIX "i18n/lib")
  elseif(${QT_VERSION_MINOR} GREATER_EQUAL 2) # Signature added in Qt 6.2, but deprecated in Qt 6.7.
    qt_add_translations(QtPokit
      TS_FILES ${TranslationSourceFiles}
      RESOURCE_PREFIX "i18n/lib")
  endif()
endif()

target_include_directories(QtPokit PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(
  QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

target_compile_definitions(QtPokit PRIVATE QTPOKIT_LIBRARY QT_NO_KEYWORDS)
