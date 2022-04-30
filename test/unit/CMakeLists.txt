find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)

# Library Unit Tests

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
  PokitDiscoveryAgent
  testpokitdiscoveryagent.cpp
  testpokitdiscoveryagent.h)

# App Unit Tests

function(add_pokit_app_unit_test name)
  add_pokit_unit_test(${name} ${ARGN})
  target_include_directories(test${name} PRIVATE ${CMAKE_SOURCE_DIR}/src/app)
  target_link_libraries(test${name} PRIVATE PokitApp)
endfunction()

add_pokit_app_unit_test(
  ScanCommand
  testscancommand.cpp
  testscancommand.h
)
