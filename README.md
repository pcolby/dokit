# QtPokit

QtPokit (pronounced "cute pocket") is a set of cross-platform utilities for [Pokit] measuring devices,
consisting of the following sub-projects:

1. `QtPocket` - native [Qt] library for accessing [Pokit] devices via the [Pokit Bluetooth API].
2. `pokit` - CLI application for accessing [Pokit] devices via the `QtPocket` libaray.
3. `pokit-gui` - GUI application for accessing [Pokit] devices via the `QtPocket` libaray.

This project is in very early stages of development, but aims to support both [Pokit Meter] and
[Pokit Pro], on Linux, Mac and Windows.

*To do: some notes on device x feature x platform x app support matrix.*

## Requirements

* Qt5 v5.5+ or Qt6 *(note, will probably increase the v5.x version requirement sometime)*
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

## License

The standalone, shared `QtPocket` library is freely available under the [LGPL]. And the rest of this
project, including the dependant console and GUI apps, are freely available under the [GPL].

[CMake]:       https://cmake.org/
[GPL]:         https://www.gnu.org/licenses/gpl-3.0.en.html "GNU General Public License"
[LGPL]:        https://www.gnu.org/licenses/lgpl-3.0.html "GNU Lesser General Public License"
[Pokit]:       https://www.pokitinnovations.com/ "Pokit Innovations"
[Pokit Bluetooth API]: https://help.pokitmeter.com/hc/en-us/community/posts/360023523213-Bluetooth-API-Documentation
[Pokit Meter]: https://www.pokitinnovations.com/pokit-meter/
[Pokit Pro]:   https://www.pokitinnovations.com/pokit-pro/
[Qt]:          https://www.qt.io/
