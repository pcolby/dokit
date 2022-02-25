add_executable(
  poqit
  discover.cpp
  discover.h
  main.cpp
)

target_link_libraries(
  poqit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
