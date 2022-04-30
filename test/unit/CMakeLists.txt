find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)

function(add_pokit_unit_test name)
  add_executable(test${name} ${ARGN})

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
  PokitDiscoveryAgent
  testpokitdiscoveryagent.cpp
  testpokitdiscoveryagent.h)
