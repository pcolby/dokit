# QtPokit

[![Build Status](https://github.com/pcolby/qtpokit/actions/workflows/build.yaml/badge.svg?branch=main)](
  https://github.com/pcolby/qtpokit/actions/workflows/build.yaml?query=branch%3Amain)
[![Static Analysis](https://github.com/pcolby/qtpokit/actions/workflows/static.yaml/badge.svg?branch=main)](
  https://github.com/pcolby/qtpokit/actions/workflows/static.yaml?query=branch%3Amain)
[![Static Analysis](https://github.com/pcolby/qtpokit/actions/workflows/docs.yaml/badge.svg?branch=main)](
  https://github.com/pcolby/qtpokit/actions/workflows/docs.yaml?query=branch%3Amain)
[![Coverage Status](https://img.shields.io/coveralls/github/pcolby/qtpokit/main?label=Coverage&logo=coveralls)](
  https://coveralls.io/github/pcolby/qtpokit?branch=main)

QtPokit is a cross-platform [Qt] library, and console application for accessing [Pokit] measuring devices.

This project is in very early stages of development, but aims to support both [Pokit Meter] and
[Pokit Pro], on Linux, Mac and Windows. A separate project (to launch later) will aim to provide a
cross-platform desktop GUI application using the QtPokit library.

## Requirements

* Qt5 v5.4+[^minQt5] or Qt6 v6.2+[^minQt6]
* a Qt-supported platform, such as Linux, MacOS or Windows
* a [Pokit] device, such as a [Pokit Meter] or [Pokit Pro]

## Building from Source

Prototypical [CMake]-based out-of-source build and test process:

~~~{.sh}
cmake -E make_directory <tmp-build-dir>
cmake -D CMAKE_BUILD_TYPE=Release -S <path-to-cloned-repo> -B <tmp-build-dir>
cmake --build <tmp-build-dir>
ctest --test-dir <tmp-build-dir> --verbose
~~~

### Documentation

Configure the same as above, but build the `doc` and (optionally) `doc-internal` targets.

~~~{.sh}
cmake -E make_directory <tmp-build-dir>
cmake -S <path-to-cloned-repo> -B <tmp-build-dir>
cmake --build <tmp-build-dir> --target doc doc-internal
# ls <tmp-build-dir>/doc/public    # Library end-user documentation
# ls <tmp-build-dir>/doc/internal  # Internal developer documentation
~~~

These are regularly published to Github Pages via Github Actions:

* [public end-user docs](https://pcolby.github.io/qtpokit/main/doc/index.html)
* [internal dev docs](https://pcolby.github.io/qtpokit/main/int/index.html)

## License

QtPocket is freely available under the [LGPL]

[^minQt5]: The Qt BLE API was first [added in v5.4](https://doc.qt.io/qt-5/qtbluetooth-le-overview.html)
[^minQt6]: The Qt Bluetooth module was [ported to Qt6 in v6.2](https://www.qt.io/blog/qt-6.2-lts-released)

[CMake]:       https://cmake.org/
[GPL]:         https://www.gnu.org/licenses/gpl-3.0.en.html "GNU General Public License"
[LGPL]:        https://www.gnu.org/licenses/lgpl-3.0.html "GNU Lesser General Public License"
[Pokit]:       https://www.pokitinnovations.com/ "Pokit Innovations"
[Pokit Bluetooth API]: https://help.pokitmeter.com/hc/en-us/community/posts/360023523213-Bluetooth-API-Documentation
[Pokit Meter]: https://www.pokitinnovations.com/pokit-meter/
[Pokit Pro]:   https://www.pokitinnovations.com/pokit-pro/
[Qt]:          https://www.qt.io/
