# SPDX-FileCopyrightText: 2022-2023 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.0)
message(STATUS "CMake version is ${CMAKE_VERSION}")
if (DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "CMake build type is ${CMAKE_BUILD_TYPE}")
endif()

# C++14 needed for std::underlying_type_t support.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Dokit VERSION 0.1.3 LANGUAGES CXX)

set(PROJECT_PRE_RELEASE pre)
#set(PROJECT_PRE_RELEASE rc1)
#unset(PROJECT_PRE_RELEASE)
if (PROJECT_PRE_RELEASE)
  add_definitions(-DPROJECT_PRE_RELEASE="${PROJECT_PRE_RELEASE}")
  set(PROJECT_VERSION_SUFFIX "-${PROJECT_PRE_RELEASE}")
endif()
if (DEFINED ENV{PROJECT_BUILD_ID})
  add_definitions(-DPROJECT_BUILD_ID="$ENV{PROJECT_BUILD_ID}")
  string(APPEND PROJECT_VERSION_SUFFIX "+$ENV{PROJECT_BUILD_ID}")
endif()
message(STATUS "${PROJECT_NAME} ${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}")
file(WRITE "${PROJECT_BINARY_DIR}/version.txt" "${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}\n")

add_definitions(
  -DPROJECT_NAME="${PROJECT_NAME}"
  -DPROJECT_VERSION="${PROJECT_VERSION}")

# Disable automatic ASCII conversions.
add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

# Enable most compiler warnings, and treat as errors.
if (MSVC)
  add_compile_options(/W3 /WX)
else()
  add_compile_options(-Wall -Wextra -Werror)
endif()

# Optional test coverage reporting (off by default, and gcc only for now).
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  option(ENABLE_COVERAGE "Enable test coverage reporting" OFF)
else()
  set(ENABLE_COVERAGE OFF)
endif()
if (ENABLE_COVERAGE)
  message(STATUS "Enabling test coverage reporting")
  add_compile_options(-coverage)
  add_link_options(--coverage)
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
