# We put all but main.cpp into a shared 'object' library so
# our app unit tests can link against the same binary objects.
add_library(PokitApp OBJECT
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
  scancommand.cpp
  scancommand.h
  setnamecommand.cpp
  setnamecommand.h
  statuscommand.cpp
  statuscommand.h
)

target_link_libraries(
  PokitApp
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)

add_executable(pokit main.cpp)

target_link_libraries(
  pokit
  PRIVATE PokitApp
  PRIVATE QtPokit
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Qt${QT_VERSION_MAJOR}::Bluetooth)
