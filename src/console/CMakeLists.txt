add_executable(
  pokit
  discover.cpp
  discover.h
  main.cpp
)

target_link_libraries(
  pokit
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
