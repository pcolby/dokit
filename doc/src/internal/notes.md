# Notes

## Pokit Bluetooth API

See also: <a href="pokit.html">Pokit API Errata</a>

### Undocumented Services

Pokit Meter and Pokit Pro both include the following services with no official documentation to match:

* `00001801-0000-1000-8000-00805f9b34fb` - aka Generic Attribute, or `0x1801`.
* `1d14d6ee-fd63-4fa1-bfa4-8f47b42119f0` - appears to be a Silicon Labs (formerly BlueGiga) OTA Service
  for updating firmware, which would make sense.

Additionally, the Pokit Meter includes the following undocumented service:

* `831d0001-e98e-1fed-e73e-4bb7596f776e` - no idea.

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


## CMake Versions

Building QtPokit requires [CMake] 3.12+.

| Version| Dependency                                                                           |
|:------:|--------------------------------------------------------------------------------------|
|  3.1   | Required by Qt5.                                                                     |
|  3.12  | [Object Libraries] for sharing binary objects between the console app and unit tests.|
|  3.16  | Required by Qt6+                                                                     |
|  3.18  | Not actually a requirement, but this is the minimum version that's regularly tested. |


[cmake]: https://cmake.org/
[not added until v5.4]: https://doc.qt.io/qt-5/qtbluetooth-le-overview.html
[Object Libraries]: https://cmake.org/cmake/help/latest/command/add_library.html#object-libraries
[ported to Qt6 in v6.2]: https://www.qt.io/blog/qt-6.2-lts-released
