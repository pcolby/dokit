cmake_minimum_required(VERSION 3.0)

add_library(QtPokit SHARED)

target_sources(
  QtPokit
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/abstractpokitservice.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/pokitdevicedisoveryagent.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/qtpokit_global.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/utils.h
  PRIVATE ${CMAKE_SOURCE_DIR}/include/qtpokit/uuids.h
  PRIVATE abstractpokitservice.cpp
  PRIVATE abstractpokitservice_p.h
  PRIVATE logging.cpp
  PRIVATE logging_p.h
  PRIVATE pokitdevicedisoveryagent.cpp
  PRIVATE pokitdevicedisoveryagent_p.h
  PRIVATE utils.cpp
)

target_include_directories(QtPokit PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(
  QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

target_compile_definitions(QtPokit PRIVATE QTPOKIT_LIBRARY)
