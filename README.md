# QtPokit

[![Build Status](https://github.com/pcolby/qtpokit/actions/workflows/build.yaml/badge.svg?branch=main)](https://github.com/pcolby/qtpokit/actions/workflows/build.yaml?query=branch%3Amain)
[![Static Analysis](https://github.com/pcolby/qtpokit/actions/workflows/static.yaml/badge.svg?branch=main)](https://github.com/pcolby/qtpokit/actions/workflows/static.yaml?query=branch%3Amain)
[![Documentation](https://github.com/pcolby/qtpokit/actions/workflows/docs.yaml/badge.svg?branch=main)](https://github.com/pcolby/qtpokit/actions/workflows/docs.yaml?query=branch%3Amain)
[![Codacy Grade](https://img.shields.io/codacy/grade/50f86ac1809f438eae51d294ad11f36f?label=Code%20Quality&logo=codacy)](https://www.codacy.com/gh/pcolby/qtpokit/dashboard)
[![Coverage Status](https://img.shields.io/coveralls/github/pcolby/qtpokit/main?label=Coverage&logo=coveralls)](https://coveralls.io/github/pcolby/qtpokit?branch=main)

QtPokit is a cross-platform [Qt][] library, and console application for accessing [Pokit][]
measuring devices. The QtPokit library is intended to be suitable for other projects -
the console application is really just an extensive example to demonstrate the library's
use. In particular, a GUI application using the QtPokit library would be most welcome ;)

Most [Pokit Meter][] and [Pokit Pro][] functions are supported, including the multimeter,
oscilloscope and data logger functions. The one real feature missing currently is
[Pokit Pro][]'s capacitance mode, which is not currently documented by [Pokit][] (support will
be added as soon as [Pokit][] updates their documentation to include it... or I get around to
reverse engineering it).

Also worth noting, is that while this project has extensive automated tests that run
regularly on Linux, macOS and Windows, using a range of Qt versions from 5.9 to 6.4, most of
the hands-on testing so far has been performed on Linux only. Testers with [Pokit][] devices
on macOS and Windows would be most welcome to help!

## Usage

### QtPokit Library

For shared library usage (for developers to create their own Pokit device applications), see
the latest [API docs][].

### `pokit` Command

The `pokit` CLI command is executed like:

```sh
pokit <command> [options]
```

Where `<command>` is one of: `info`, `status`, `meter`, `dso`, `logger-start`, `logger-stop`,
`logger-fetch`, `scan`, `set-name`, `flash-led`, or `calibrate`

For example, to get a device's status:

```sh
pokit status
```

Outputs like:

```text
Device name:           PokitMeter
Firmware version:      1.4
Maximum voltage:       60
Maximum current:       2
Maximum resistance:    1000
Maximum sampling rate: 1000
Sampling buffer size:  8192
Capability mask:       0
MAC address:           84:2E:14:2C:03:A8
Device status:         Idle (0)
Battery voltage:       2.76221
Battery status:        N/A (255)
```

Or, you can output in CSV, or JSON too, like:

```sh
pokit status --output json
```

```json
{
    "battery": {
        "level": 2.760070562362671
    },
    "capabilityMask": 0,
    "deviceName": "PokitMeter",
    "deviceStatus": {
        "code": 0,
        "label": "Idle"
    },
    "firmwareVersion": {
        "major": 1,
        "minor": 4
    },
    "macAddress": "84:2E:14:2C:03:A8",
    "maximumCurrent": 2,
    "maximumResistance": 1000,
    "maximumSamplingRate": 1000,
    "maximumVoltage": 60,
    "samplingBufferSize": 8192
}
```

By default, the `pokit` command will use the first Pokit device it finds. However, if you have more than
one device, you can specify the device's name, or MAC address, or (on macOS) device UUID, such as:

```sh
pokit status --device RedPokitPro
```

Tip: You can rename Pokit devices via the official Pokit app, or the `set-name` command, like:

```sh
pokit set-name --device Pokit --new-name PokitMeter
```

Here's a more complex usage example:

```sh
pokit meter --mode Vac --range 10V --samples 10 --output csv
```

This will fetch 10 AC meter readings, on the nearest range that can support 10Vac, and output those
readings in CSV format:

For full usage information (albeit brief), use the `--help` option, which currently outputs something like:

```text
Usage: pokit <command> [options]

Options:
  --color <yes|no|auto>    Colors the console output. Valid options are: yes,
                           no and auto. The default is auto.
  --debug                  Enable debug output.
  -d, --device <device>    Set the name, hardware address or macOS UUID of
                           Pokit device to use. If not specified, the first
                           discovered Pokit device will be used.
  -h, --help               Displays help on commandline options.
  --help-all               Displays help including Qt specific options.
  --interval <interval>    Set the update interval for DOS, meter and logger
                           modes. Suffixes such as 's' and 'ms' (for seconds and
                           milliseconds) may be used. If no suffix is present,
                           the units will be inferred from the magnitide of the
                           given interval. If the option itself is not
                           specified, a sensible default will be chosen
                           according to the selected command.
  --mode <mode>            Set the desired operation mode for meter, dso and
                           logger modes. Supported modes are: AC Voltage, DC
                           Voltage, AC Current, DC Current, Resistance, Diode,
                           Continuity, and Temperature. All are case
                           insensitive. Only the first four options are
                           available for dso and logger commands; the rest are
                           available in meter mode only.
  --new-name <name>        Give the desired new name for the set-name command.
  --output <format>        Set the format for output. Supported formats are:
                           CSV, JSON and Text. All are case insenstitve. The
                           default is Text.
  --range <range>          Set the desired measurement range. Pokit devices
                           support specific ranges, such as 0 to 300mV. Specify
                           the desired upper limit, and the best range will be
                           selected, or use 'auto' to enable the Pokit device's
                           auto-range feature. The default is 'auto'.
  --samples <count>        Set the number of samples to acquire.
  --temperature <degrees>  Set the current ambient temperature for the
                           calibration command.
  --timeout <period>       Set the device discovery scan timeout.Suffixes such
                           as 's' and 'ms' (for seconds and milliseconds) may be
                           used. If no suffix is present, the units will be
                           inferred from the magnitide of the given interval.
                           The default behaviour is no timeout.
  --timestamp <period>     Set the optional starting timestamp for data
                           logging. Default to 'now'.
  --trigger-level <level>  Set the DSO trigger level.
  --trigger-mode <mode>    Set the DSO trigger mode. Supported modes are: free,
                           rising and falling. The default is free.
  -v, --version            Displays version information.

Command:
  info                     Get Pokit device information
  status                   Get Pokit device status
  meter                    Access Pokit device's multimeter mode
  dso                      Access Pokit device's DSO mode
  logger-start             Start Pokit device's data logger mode
  logger-stop              Stop Pokit device's data logger mode
  logger-fetch             Fetch Pokit device's data logger samples
  scan                     Scan Bluetooth for Pokit devices
  set-name                 Set Pokit device's name
  flash-led                Flash Pokit device's LED
  calibrate                Calibrate Pokit device temperature
```

## Requirements

* Qt5 v5.4+[^minQt5] or Qt6 v6.2+[^minQt6]
* a Qt-supported platform, such as Linux, macOS or Windows
* a [Pokit][] device, such as a [Pokit Meter][] or [Pokit Pro][]
* [CMake][] (for building the QtPokit itself) 3.0+
  * some unit tests won't be built unless using CMake 3.12 or later.

## Building from Source

Prototypical [CMake][]-based out-of-source build and test process, for CMake 3.13 or later:

```sh
cmake -D CMAKE_BUILD_TYPE=Release -S <path-to-cloned-repo> -B <tmp-build-dir>
cmake --build <tmp-build-dir>
ctest --test-dir <tmp-build-dir> --verbose
```

For CMake versions earlier than 3.13 (ie before CMake added the `-B <build-dir>` command line option):

```sh
cmake -E make_directory <tmp-build-dir>
cd <tmp-build-dir>
cmake -D CMAKE_BUILD_TYPE=Release -S <path-to-cloned-repo>
cmake --build <tmp-build-dir>
ctest --test-dir <tmp-build-dir> --verbose
```

### Documentation

Configure the same as above, but build the `doc` and (optionally) `doc-internal` targets, for example:

```sh
cmake -S <path-to-cloned-repo> -B <tmp-build-dir>
cmake --build <tmp-build-dir> --target doc doc-internal
# ls <tmp-build-dir>/doc/public    # Library end-user documentation
# ls <tmp-build-dir>/doc/internal  # Internal developer documentation
```

These are regularly published to Github Pages via Github Actions:

* [public end-user docs](https://pcolby.github.io/qtpokit/main/doc/index.html)
* [internal dev docs](https://pcolby.github.io/qtpokit/main/int/index.html)

## License

QtPocket is freely available under the [LGPL].

[^minQt5]: The Qt BLE API was first [added in v5.4](https://doc.qt.io/qt-5/qtbluetooth-le-overview.html)
[^minQt6]: The Qt Bluetooth module was [ported to Qt6 in v6.2](https://www.qt.io/blog/qt-6.2-lts-released)

[API docs]:    https://pcolby.github.io/qtpokit/ "QtPokit API Documentation"
[CMake]:       https://cmake.org/
[LGPL]:        https://www.gnu.org/licenses/lgpl-3.0.html "GNU Lesser General Public License"
[Pokit]:       https://www.pokitinnovations.com/ "Pokit Innovations"
[Pokit Meter]: https://www.pokitinnovations.com/pokit-meter/
[Pokit Pro]:   https://www.pokitinnovations.com/pokit-pro/
[Qt]:          https://www.qt.io/
