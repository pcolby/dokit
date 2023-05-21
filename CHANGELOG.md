# Changelog[^1]

## [0.3.0][] (2023-05-21)

This release breaks backward compatibility by changing some QtPokit APIs to support [Pokit Pro]'s
divergent (and not at all documented by Pokit) measurement range enumeration values.

### Added

- Support for [Pokit Pro] measurement ranges (eg 600V).
- Support for capacitance measurement.
- Support for external temperature measurement.

### Changed

- Update QtPokit API to support separate measurement ranges for [Pokit Meter] and [Pokit Pro].

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

[Unreleased]: https://github.com/pcolby/dokit/compare/v0.3.0...HEAD
[0.3.0]: https://github.com/pcolby/dokit/releases/tag/v0.3.0
[0.2.0]: https://github.com/pcolby/dokit/releases/tag/v0.2.0
[0.1.2]: https://github.com/pcolby/dokit/releases/tag/v0.1.2
[0.1.1]: https://github.com/pcolby/dokit/releases/tag/v0.1.1
[0.1.0]: https://github.com/pcolby/dokit/releases/tag/v0.1.0

[Pokit Meter]: https://www.pokitinnovations.com/pokit-meter/
[Pokit Pro]:   https://www.pokitinnovations.com/pokit-pro/
[Qt]:          https://www.qt.io/

[^1]: Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and
  project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
