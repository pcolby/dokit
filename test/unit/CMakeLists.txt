find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)

function(add_pokit_unit_test name)
  add_executable(test${name} ${ARGN})

  # \todo Export headers so tests include like <qtpokit/foo.h>
  target_include_directories(test${name} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

  target_link_libraries(
    test${name}
    PRIVATE QtPokit
    PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth
    PRIVATE Qt${QT_VERSION_MAJOR}::Test)

  add_test(NAME ${name} COMMAND test${name})
endfunction()

add_pokit_unit_test(
  PokitUtils
  tst_utils.cpp
  tst_utils.h)

add_pokit_unit_test(
  testPokitDeviceDiscoveryAgent
  testpokitdevicedisoveryagent.cpp
  testpokitdevicedisoveryagent.h)
