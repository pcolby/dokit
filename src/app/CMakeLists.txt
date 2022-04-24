add_executable(
  pokit
  abstractcommand.cpp
  abstractcommand.h
  calibratecommand.cpp
  calibratecommand.h
  devicecommand.cpp
  devicecommand.h
  flashledcommand.cpp
  flashledcommand.h
  infocommand.cpp
  infocommand.h
  main.cpp
  scancommand.cpp
  scancommand.h
  setnamecommand.cpp
  setnamecommand.h
  statuscommand.cpp
  statuscommand.h
)

target_link_libraries(
  pokit
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
