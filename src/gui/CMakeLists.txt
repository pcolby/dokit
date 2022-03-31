add_executable(
  pokit-gui
  main.cpp
)

target_link_libraries(
  pokit-gui
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Gui
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
