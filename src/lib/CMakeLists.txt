cmake_minimum_required(VERSION 3.0)

add_library(PoqitLib SHARED
  pokitdevicedisoveryagent.cpp
  pokitdevicedisoveryagent.h
  poqit_global.h
)

target_link_libraries(
  PoqitLib
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

target_compile_definitions(PoqitLib PRIVATE POQIT_LIBRARY)
