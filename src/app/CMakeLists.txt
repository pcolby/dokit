add_executable(
  pokit
  abstractcommand.cpp
  abstractcommand.h
  infocommand.cpp
  infocommand.h
  main.cpp
  scancommand.cpp
  scancommand.h
)

target_link_libraries(
  pokit
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
