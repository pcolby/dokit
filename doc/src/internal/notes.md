# Notes

## Pokit Bluetooth API

See also: [Pokit API Errata](pokit.html)

### Undocumented Services

Pokit Meter and Pokit Pro both include the following service(s) with no official documentation to match:

* `1d14d6ee-fd63-4fa1-bfa4-8f47b42119f0` - appears to be a Silicon Labs (formerly BlueGiga) OTA Service
  for updating firmware, which would make sense.

Additionally, the Pokit Meter includes the following undocumented service:

* `831d0001-e98e-1fed-e73e-4bb7596f776e` - no idea.

Additionally, the Pokit Pro includes the following undocumented services (added somewhere between firmware v1.8.1 and
v2.0.2):

* `3852ae6e-a1e6-4132-87f7-8929f6117338` - no idea.
* `ae97e4b0-9c1c-4507-bf7b-eff703c6a85c` - no idea.

Also, the Pokit app contains references to the following UUIDs; these may or may not be BLE services.

* `581a9e6a-d2f6-4d7b-b0d0-5bc3ed9b842e`
* `5f330a08-8e76-4d5a-97fb-22e1807b1888`
* `6e7a7953-3b41-4be5-8052-c93129fa1f35`
* `b4a19bce-96cf-4915-9ba4-a19f4a042c8e`
* `c999d155-c00a-4e8f-a056-cd9523ce39de`
* `ef1b0821-b107-4cae-b17c-df1e99b1667a`
* `f2259466-ac2a-40a9-bf23-f47666ae2b15`

## Qt Versions

QtPokit requires Qt's support Bluetooth Low Energy, in Central mode.

|  Qt | Supported| Notes                                    |
|-----|:--------:|------------------------------------------|
| 5.0 | No       | Qt BLE API was [not added until v5.4].   |
| 5.1 | No       | Qt BLE API was [not added until v5.4].   |
| 5.2 | No       | Qt BLE API was [not added until v5.4].   |
| 5.3 | No       | Qt BLE API was [not added until v5.4].   |
| 5.4 | Probably |                                          |
| 5.5 | Probably |                                          |
| 5.6 | Probably |                                          |
| 5.7 | Probably |                                          |
| 5.8 | Probably |                                          |
| 5.9 | Probably |                                          |
| 5.10| Probably |                                          |
| 5.11| Probably |                                          |
| 5.12| Probably |                                          |
| 5.13| Probably |                                          |
| 5.14| Probably |                                          |
| 5.15| Yes      |                                          |
| 6.0 | No       | Bluetooth module [ported to Qt6 in v6.2].|
| 6.1 | No       | Bluetooth module [ported to Qt6 in v6.2].|
| 6.2 | Yes      |                                          |
| 6.3 | Yes      |                                          |
| 6.4 | Yes      |                                          |
| 6.5 | Yes      |                                          |
| 6.6 | Yes      |                                          |
| 6.7 | Yes      |                                          |
| 6.8 | Yes      |                                          |
| 6.9 | Yes      |                                          |

## CMake Versions

CMake 3.15+ is required for *building* QtPokit.

Some notes on specific features that imply minimum CMake versions:

| Version| Dependency                                                                           |
|:------:|--------------------------------------------------------------------------------------|
| 2.8.9  | [Qt 5.1+](https://github.com/qt/qtbase/blob/v5.1.0/src/corelib/Qt5Config.cmake.in) |
| 2.8.11 | [`target_include_directories`](https://cmake.org/cmake/help/v2.8.11/cmake.html#command:target_include_directories) |
| 2.8.11 | Has `-fPIC` issues with Qt. |
| 2.8.12 | [`add_compile_options`](https://cmake.org/cmake/help/v2.8.12/cmake.html#command:add_compile_options) |
| 2.8.12 | Has compile issues with Qt. |
|  3.0   | `VERSION` option of the `project()` command. |
|  3.1   | [Qt 5.11+](https://github.com/qt/qtbase/blob/v5.11.0/src/corelib/Qt5Config.cmake.in) |
|        | `target_sources` command. |
|  3.11  | `include(FetchContent)` |
|  3.12  | [Object Libraries] for sharing binary objects between the console app and unit tests.|
|  3.13  | `-B` CLI option (handy, but not essential).                                          |
|        | `add_link_options` command (for `--coverage`) |
|  3.14  | `FetchContent_MakeAvailable(FindQtDocs)` |
|  3.15  | `list(PREPEND CMAKE_MODULE_PATH "${findqtdocs_SOURCE_DIR}")` |
|  3.16  | [Qt6.x shared](https://github.com/qt/qtbase/blob/v6.2.0/.cmake.conf)|
|  3.18  | Not actually a requirement, but this is the minimum version that's regularly tested. |
|  3.21  | [Qt6.x static](https://github.com/qt/qtbase/blob/v6.2.0/.cmake.conf)|

[not added until v5.4]: https://doc.qt.io/qt-5/qtbluetooth-le-overview.html
[Object Libraries]: https://cmake.org/cmake/help/latest/command/add_library.html#object-libraries
[ported to Qt6 in v6.2]: https://www.qt.io/blog/qt-6.2-lts-released
