cmake_minimum_required(VERSION 3.0)

add_library(QtPokit SHARED
  logging.cpp
  logging.h
  pokitdevicedisoveryagent.cpp
  pokitdevicedisoveryagent.h
  qtpokit_global.h
  utils.h
  uuids.h
)

target_link_libraries(
  QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

target_compile_definitions(QtPokit PRIVATE QTPOKIT_LIBRARY)
