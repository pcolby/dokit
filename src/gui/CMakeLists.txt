add_executable(
  Poqit
  main.cpp
)

target_link_libraries(
  Poqit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Gui
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
