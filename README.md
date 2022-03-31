# QtPokit

QtPokit (pronounced "cute pocket") is a set of cross-platform utilities for [Pokit] measuring devices,
consisting of the following sub-projects:

1. `QtPocket` - native [Qt] library for accessing [Pokit] devices via the [Pokit Bluetooth API].
2. `pokit` - CLI application for accessing [Pokit] devices via the `QtPocket` libaray.
3. `pokit-gui` - GUI application for accessing [Pokit] devices via the `QtPocket` libaray.

This project is in very early stages of development, but aims to support both [Pokit Meter] and
[Pokit Pro], on Linux, Mac and Windows.

*To do: some notes on device x feature x platform x app support matrix.*


[Pokit]:       https://www.pokitinnovations.com/ "Pokit Innovations"
[Pokit Bluetooth API]: https://help.pokitmeter.com/hc/en-us/community/posts/360023523213-Bluetooth-API-Documentation
[Pokit Meter]: https://www.pokitinnovations.com/pokit-meter/
[Pokit Pro]:   https://www.pokitinnovations.com/pokit-pro/
[Qt]:          https://www.qt.io/
