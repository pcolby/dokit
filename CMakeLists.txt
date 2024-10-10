# SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.15...3.29 FATAL_ERROR)
message(STATUS "CMake version is ${CMAKE_VERSION}")
if (DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "CMake build type is ${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
  Dokit
  VERSION 0.5.3
  DESCRIPTION "Cross-platform library, and desktop apps for Pokit measuring devices"
  HOMEPAGE_URL "https://github.com/pcolby/dokit"
  LANGUAGES CXX)

# Allow setting the log level via the environment (if not already set some other way).
if (NOT DEFINED CMAKE_MESSAGE_LOG_LEVEL AND DEFINED ENV{CMAKE_MESSAGE_LOG_LEVEL})
  set(CMAKE_MESSAGE_LOG_LEVEL "$ENV{CMAKE_MESSAGE_LOG_LEVEL}")
endif()

set(PROJECT_PRE_RELEASE pre)
#set(PROJECT_PRE_RELEASE rc1)
#unset(PROJECT_PRE_RELEASE)
if (PROJECT_PRE_RELEASE)
  add_compile_definitions(PROJECT_PRE_RELEASE="${PROJECT_PRE_RELEASE}")
  set(PROJECT_VERSION_SUFFIX "-${PROJECT_PRE_RELEASE}")
endif()
if (DEFINED ENV{PROJECT_BUILD_ID})
  add_compile_definitions(PROJECT_BUILD_ID="$ENV{PROJECT_BUILD_ID}")
  string(APPEND PROJECT_VERSION_SUFFIX "+$ENV{PROJECT_BUILD_ID}")
endif()
message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}")
file(WRITE "${PROJECT_BINARY_DIR}/version.txt" "${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}\n")

add_compile_definitions(
  PROJECT_NAME="${PROJECT_NAME}"
  PROJECT_VERSION="${PROJECT_VERSION}")

# Enable Qt's strict mode.
add_compile_definitions(
  QT_ENABLE_STRICT_MODE_UP_TO=0x060800
  # Qt 6.0.0
  QT_NO_FOREACH
  QT_NO_CAST_FROM_ASCII
  QT_NO_CAST_TO_ASCII
  QT_NO_CAST_FROM_BYTEARRAY
  QT_NO_URL_CAST_FROM_STRING
  QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
  QT_NO_JAVA_STYLE_ITERATORS
  # Qt 6.6.0
  QT_NO_QEXCHANGE
  # Qt 6.7.0
  QT_NO_CONTEXTLESS_CONNECT
  # Qt 6.8.0
  QT_NO_QASCONST
  QT_NO_QSNPRINTF
  QT_USE_NODISCARD_FILE_OPEN
)

# Enable most compiler warnings, by default.
option(EXTRA_COMPILE_WARNINGS "Enable additional compile warnings" ON)
if (EXTRA_COMPILE_WARNINGS)
  message(STATUS "Enabling additional compile warnings")
  if (MSVC)
    add_compile_options(/W3)
  else()
    add_compile_options(-Wall -Wextra)
  endif()
endif()

# Treat warnings as errors, by default.
if (CMAKE_VERSION VERSION_LESS 3.24)
  option(COMPILE_WARNING_AS_ERROR "Treat compile compiler warnings as errors" ON)
  if (CMAKE_COMPILE_WARNING_AS_ERROR)
    message(STATUS "Treating compile warnings as errors")
    if (MSVC)
      add_compile_options(/WX)
    else()
      add_compile_options(-Werror)
    endif()
  endif()
elseif (NOT DEFINED CMAKE_COMPILE_WARNING_AS_ERROR)
  message(STATUS "Treating compile warnings as errors")
  set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif()

# Optional test coverage reporting (off by default, and only supported for some compilers).
set(SUPPORTED_COVERAGE_COMPILER_IDS "AppleClang" "Clang" "GNU")
if (CMAKE_CXX_COMPILER_ID IN_LIST SUPPORTED_COVERAGE_COMPILER_IDS)
  message(VERBOSE "Coverage reporting suported for compiler ${CMAKE_CXX_COMPILER_ID}")
  option(ENABLE_COVERAGE "Enable test coverage reporting" OFF)
else()
  message(VERBOSE "Coverage reporting not suported for compiler: ${CMAKE_CXX_COMPILER_ID}")
  set(ENABLE_COVERAGE OFF)
endif()
if (ENABLE_COVERAGE)
  message(STATUS "Enabling test coverage reporting")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate)
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-coverage)
    add_link_options(--coverage)
  else()
    message(SEND_ERROR "Do not know how to enable coverage reports for ${CMAKE_CXX_COMPILER_ID} compiler")
  endif()
endif()

# Default to Qt6 where available, otherwise Qt5.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(QT REQUIRED COMPONENTS Core Bluetooth LinguistTools NAMES Qt6 Qt5)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Bluetooth LinguistTools)
message(STATUS "Found Qt ${Qt${QT_VERSION_MAJOR}_VERSION}")
message(STATUS "Found Qt Bluetooth ${Qt${QT_VERSION_MAJOR}Bluetooth_VERSION}")
message(STATUS "Found Qt Linguist Tools ${Qt${QT_VERSION_MAJOR}LinguistTools_VERSION}")

include(CTest)
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(test)
