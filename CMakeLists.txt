cmake_minimum_required(VERSION 3.0)
message("-- CMake version is ${CMAKE_VERSION}")
if (DEFINED CMAKE_BUILD_TYPE)
  message("-- CMake build type is ${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(
  QtPokit
  VERSION 0.1.0
  DESCRIPTION "Desktop applications for Pokit measuring devices"
 #HOMEPAGE_URL "https://github.com/pcolby/qtpokit"
  LANGUAGES CXX)

set(POKIT_PRE_RELEASE pre)
#set(POKIT_PRE_RELEASE rc1)
#unset(POKIT_PRE_RELEASE)
if (POKIT_PRE_RELEASE)
  add_definitions(-DPOKIT_PRE_RELEASE="${POKIT_PRE_RELEASE}")
  set(POKIT_VERSION_SUFFIX "-${POKIT_PRE_RELEASE}")
endif()
message("-- ${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}${POKIT_VERSION_SUFFIX}")
file(WRITE "${PROJECT_BINARY_DIR}/version.txt" "${CMAKE_PROJECT_VERSION}${POKIT_VERSION_SUFFIX}\n")

add_definitions(
  -DCMAKE_PROJECT_NAME="${CMAKE_PROJECT_NAME}"
  -DCMAKE_PROJECT_VERSION="${CMAKE_PROJECT_VERSION}"
  -DCMAKE_PROJECT_DESCRIPTION="${CMAKE_PROJECT_DESCRIPTION}")

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
  message("-- Enabling test coverage reporting")
  add_compile_options(-coverage)
  add_link_options(--coverage)
endif()

# Default to Qt6 where available, otherwise Qt5.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(QT REQUIRED COMPONENTS Core Network NAMES Qt6 Qt5)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Bluetooth)
message("-- Found Qt ${Qt${QT_VERSION_MAJOR}_VERSION}")
message("-- Found Qt Bluetooth ${Qt${QT_VERSION_MAJOR}Bluetooth_VERSION}")

include(CTest)
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(test)
