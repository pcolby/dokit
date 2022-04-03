add_executable(
  pokit
  main.cpp
  scanner.cpp
  scanner.h
)

target_link_libraries(
  pokit
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
