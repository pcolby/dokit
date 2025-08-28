# Changelog[^1]

## [Unreleased][]

### Changed

- Upgrade to Qt 6.9.2
- Support for Qt 6.10, including support for the `LANG` environment variable on MinGW ([d0cb2eb][])

### Fixed

- Support for (optional) `QTPOKIT_NAMESPACE` ([9a4d1cf][])

## [0.5.5][] (2025-03-09)

### Added

- Support for the `LANG` environment variable on macOS ([58d82dd][])
- CI tests for localisations ([1bca548][])

### Removed

- Support for localisations on Linux with GCC and Qt 6.2. This was, in fact, never working. But only now revealed by
  recently-added CI tests. If you must use the CLI application with Qt 6.2 on Linux, then Clang builds are recommended.
  Of course, a more recent Qt version would be even better.

### Fixed

- Output buffering issue for MinGW builds with Qt 6.5, 6.6 and 6.7 ([cc5ff57][])
- Builds broken by Qt 6.9.0 Beta 3 CMake macro changes ([45bd46d][])

## [0.5.4][] (2025-02-01)

### Added

- Linux AArch64 / ARM64 binaries ([d54fbbc][])

### Changed

- Return `quint32` from all `maxValue` methods, instead of `QVariant` ([db8b841][])
- Upgrade to Doxygen 1.13.1.
- Upgrade to Qt 6.8.2, and 6.9.0 beta.

## [0.5.3][] (2024-12-23)

### Added

- Support for LLVM-based coverage reporting with Clang.

### Changed

- Internationalisation improvements.
- Upgrade to Doxygen 1.11.0.
- Upgrade to Qt 6.8.0, and 6.7.3.

### Fixed

- Correctly round CLI arguments ([899b007][])

## [0.5.2][] (2024-05-06)

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

[Unreleased]: https://github.com/pcolby/dokit/compare/v0.5.5...HEAD
[0.5.5]: https://github.com/pcolby/dokit/releases/tag/v0.5.5
[0.5.4]: https://github.com/pcolby/dokit/releases/tag/v0.5.4
[0.5.3]: https://github.com/pcolby/dokit/releases/tag/v0.5.3
[0.5.2]: https://github.com/pcolby/dokit/releases/tag/v0.5.2
[0.5.1]: https://github.com/pcolby/dokit/releases/tag/v0.5.1
[0.5.0]: https://github.com/pcolby/dokit/releases/tag/v0.5.0
[0.4.0]: https://github.com/pcolby/dokit/releases/tag/v0.4.0
[0.3.0]: https://github.com/pcolby/dokit/releases/tag/v0.3.0
[0.2.0]: https://github.com/pcolby/dokit/releases/tag/v0.2.0
[0.1.2]: https://github.com/pcolby/dokit/releases/tag/v0.1.2
[0.1.1]: https://github.com/pcolby/dokit/releases/tag/v0.1.1
[0.1.0]: https://github.com/pcolby/dokit/releases/tag/v0.1.0

[073bce8]:     https://github.com/pcolby/dokit/commit/073bce82014a85e033639862fdb061c72b7971fd "Include switch and charging status in CLI output"
[1bca548]:     https://github.com/pcolby/dokit/commit/1bca548c7abf9b9e59dd2ab2e5a27aac0dfa5119 "Add workflow steps for testing localisations"
[4094bfb]:     https://github.com/pcolby/dokit/commit/4094bfb218df20e1d34222b0b701fad8f57b0f2b "Warn when requesting an unsupported scan timeout"
[45bd46d]:     https://github.com/pcolby/dokit/commit/45bd46d85433a3d3b8e04b9a1f8ba500fc3dcc5a "Work around Qt 6.9.0 beta 3 L10n bug"
[58d82dd]:     https://github.com/pcolby/dokit/commit/58d82ddb70b166212e3c8c0a778dbd32a255db29 "Support the LANG environment variable on macOS"
[7f70715]:     https://github.com/pcolby/dokit/commit/7f7071554cdb8170aeeaa7fb51fec26535f00321 "Correctly exit on discovery agent errors"
[899b007]:     https://github.com/pcolby/dokit/commit/899b007e73e64e724f2785acafddd2d3fc6f893e "Round command line options correctly"
[9a4d1cf]:     https://github.com/pcolby/dokit/commit/9a4d1cf13198fbf9a4d0bb0e8c3dd3001cefdf65 "Complete support for (optional) QTPOKIT_NAMESPACE"
[cc5ff57]:     https://github.com/pcolby/dokit/commit/cc5ff57bc2f94f4306687d88307cda4f3fbb3612 "Use fputs() instead of std::err for CLI error output"
[d0cb2eb]:     https://github.com/pcolby/dokit/commit/d0cb2ebe264ed035357a2dc12bde8e04e446423d "Add Qt 6.10 builds"
[d54fbbc]:     https://github.com/pcolby/dokit/commit/d54fbbcb2eacbce6b4e9bfb8a23a9ce43fd7524a "Support GitHub's Linux arm64 preview hosts"
[db8b841]:     https://github.com/pcolby/dokit/commit/db8b84197d00b5a357974043f2dc0f3c0b7dfeff "Return quint32 from all maxValue methods"

[#9]:          https://github.com/pcolby/dokit/issues/9
[#17]:         https://github.com/pcolby/dokit/issues/17
[#18]:         https://github.com/pcolby/dokit/issues/18
[Pokit Meter]: https://www.pokitinnovations.com/pokit-meter/
[Pokit Pro]:   https://www.pokitinnovations.com/pokit-pro/
[Qt]:          https://www.qt.io/
[QTBUG-112204]: https://bugreports.qt.io/browse/QTBUG-112204 "windeployqt error when creating translations"

[^1]: Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and
  project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
