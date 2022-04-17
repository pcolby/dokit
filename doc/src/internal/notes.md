# Notes

## Pokit Bluetooth API

### Things to Query

#### Status Service UUID

* `57d3a771-267c-4394-8872-78223e92aec4` - claimed by API docs;
* `57d3a771-267c-4394-8872-78223e92aec5` - in reality?

#### Status Characteristic Value Length

Docs claim 5 bytes, but PokitPro is returning 8. Probably docs just not updated to show what those extra
3 bytes represent.

### Flash LED Service

I cannot get this service to work; whenever I try writing the required `\x01` value, I get ATT error `0x80`
from the Bluez stack.  Maybe its not supported by PokitPro devices, or needs additional data for those?

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


[not added until v5.4]: https://doc.qt.io/qt-5/qtbluetooth-le-overview.html
[ported to Qt6 in v6.2]: https://www.qt.io/blog/qt-6.2-lts-released