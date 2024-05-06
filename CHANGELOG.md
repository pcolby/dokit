# Changelog[^1]

## [0.5.2][2024-05-06]

### Changed

- Remove the `GenericAccessService` class ([#18])
- Upgrade to Qt 6.6.3.
- Raise CMake minimum to 3.15.

## [0.5.1][] (2023-12-28)

### Added

- Include switch and charging status in CLI output ([073bce8][])

### Changed

- Upgrade to Doxygen 1.10.0.
- Upgrade to Qt 6.6.1.

### Fixed

- Correct the Calibrate service's Set Parameter characteristic's UUID ([#17])

Thanks to [@NET-Code-Dev](https://github.com/NET-Code-Dev) for contributing to this release.

## [0.5.0][] (2023-11-25)

### Added

- Warn when requesting an unsupported scan timeout ([4094bfb][])

### Changed

- Switch macOS CLI to Application Bundle ([#9])
- Upgrade to Qt 6.6.0.

### Fixed

- Correctly exit on discovery agent errors ([7f70715][])

## [0.4.0][] (2023-06-03)

### Added

- Support [Pokit Pro] charging status (not documented by Pokit).
- Support [Pokit Pro] physical button(not documented by Pokit).
- Support [Pokit Pro] physical mode switch position (not documented by Pokit).
- Support [Pokit Pro] serial numbers (not documented by Pokit).
- Support [Pokit Pro] torch (not documented by Pokit).
- Additional debug logging to detect undocumented BLE characteristics.

### Changed

- Upgrade to Qt 6.5.1.

## [0.3.0][] (2023-05-21)

This release breaks backward compatibility by changing some QtPokit APIs to support [Pokit Pro]'s
divergent (and not at all documented by Pokit) measurement range enumeration values.

### Added

- Support for [Pokit Pro] measurement ranges (eg 600V).
- Support for capacitance measurement.
- Support for external temperature measurement.

### Changed

- Update QtPokit API to support separate measurement ranges for [Pokit Meter] and [Pokit Pro].
- Upgrade to Doxygen 1.9.7.
- Upgrade to Qt 6.5.0 (except for portable Windows builds; [QTBUG-112204])

## [0.2.0][] (2023-05-07)

### Added

- Temperature support for logger modes.
- Localisation for en_AU, en_GB and en_US (Qt6 only for now).
- Very early GUI app prototype.

### Changed

- Renamed the project to `Dokit` (the shared library is still `QtPokit`).

## [0.1.2][] (2023-03-30)

### Added

- Support AppImage builds on Linux.
- Integrate Codacy and SonarCloud.

### Changed

- Fix minor codestyle and documentation issues.
- Upgrade to Doxygen 1.9.6.
- Upgrade to Qt 6.4.3.

Thanks to [@lnxsr](https://github.com/lnxsr) for contributing to this release.

## [0.1.1][] (2022-11-05)

### Added

- Project build identifiers in `-v` output.

### Changed

- Fix benign "Ignoring `-d` option" warning.
- Improve debug logging.
- Upgrade to Doxygen 1.9.5.

## [0.1.0][] (2022-08-26)

### Added

- Support for [Pokit Meter][] and [Pokit Pro][] devices.
- Support for multimeter, oscilloscope and data-logger modes.
- Support for Linux, macOS and Windows.
- Support for [Qt][] 5.4+ and 6.2+

[Unreleased]: https://github.com/pcolby/dokit/compare/v0.5.1...HEAD
[0.5.1]: https://github.com/pcolby/dokit/releases/tag/v0.5.1
[0.5.0]: https://github.com/pcolby/dokit/releases/tag/v0.5.0
[0.4.0]: https://github.com/pcolby/dokit/releases/tag/v0.4.0
[0.3.0]: https://github.com/pcolby/dokit/releases/tag/v0.3.0
[0.2.0]: https://github.com/pcolby/dokit/releases/tag/v0.2.0
[0.1.2]: https://github.com/pcolby/dokit/releases/tag/v0.1.2
[0.1.1]: https://github.com/pcolby/dokit/releases/tag/v0.1.1
[0.1.0]: https://github.com/pcolby/dokit/releases/tag/v0.1.0

[073bce8]:     https://github.com/pcolby/dokit/commit/073bce82014a85e033639862fdb061c72b7971fd "Include switch and charging status in CLI output"
[4094bfb]:     https://github.com/pcolby/dokit/commit/4094bfb218df20e1d34222b0b701fad8f57b0f2b "Warn when requesting an unsupported scan timeout"
[7f70715]:     https://github.com/pcolby/dokit/commit/7f7071554cdb8170aeeaa7fb51fec26535f00321 "Correctly exit on discovery agent errors"
[#9]:          https://github.com/pcolby/dokit/issues/9
[#17]:         https://github.com/pcolby/dokit/issues/17
[#18]:         https://github.com/pcolby/dokit/issues/18
[Pokit Meter]: https://www.pokitinnovations.com/pokit-meter/
[Pokit Pro]:   https://www.pokitinnovations.com/pokit-pro/
[Qt]:          https://www.qt.io/
[QTBUG-112204]: https://bugreports.qt.io/browse/QTBUG-112204 "windeployqt error when creating translations"

[^1]: Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and
  project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
