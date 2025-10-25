# GamePerf Manager

GamePerf Manager is a Qt Quick-based desktop application that unifies Linux gaming sources (Steam, Epic via Legendary CLI, Wine/Proton) and provides a first-run experience for configuring libraries and verifying the bundled performance daemon. The project ships with packaging assets for AppImage and Debian-based distributions.

## Features

- Qt Quick user interface with responsive layout
- First-run onboarding wizard for Steam libraries, Legendary CLI, and Wine prefixes
- Integration stub for performance daemon (systemd service)
- Polkit policy and udev rules to allow hardware control under managed permissions
- Packaging scripts for AppImage and Debian packages
- GitHub Actions CI workflow to build packaging artifacts

## Build Requirements

- CMake >= 3.16
- Qt 6 (Core, Gui, Qml, Quick, QuickControls2 modules)
- A C++17 capable compiler
- Optional: `linuxdeploy` and `appimagetool` for AppImage creation

## Building the Project

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

This produces the following binaries:

- `build/gameperf-manager` – main application
- `build/libexec/gameperf-daemon` – performance daemon

## Installing (without package)

After building, install to a local prefix:

```bash
cmake --install build --prefix ~/.local
```

## Installing the Performance Daemon

Use the provided helper script to install and enable the daemon, polkit policy, and udev rules:

```bash
scripts/install-performance-daemon.sh
```

By default the script copies files into `/usr` (`PREFIX` environment variable overrides the target prefix).

## AppImage Packaging

1. Build the project as described above.
2. Run the AppImage packaging script:

```bash
scripts/build-appimage.sh
```

The resulting AppImage is placed in `build/packaging/appimage/GamePerf-Manager-<version>-x86_64.AppImage`.

## Debian Packaging

1. Build the project
2. Execute:

```bash
scripts/build-deb.sh
```

The Debian `.deb` packages are collected in `build/packaging/debian/`.

## CI Pipeline

The repository includes a GitHub Actions workflow (`.github/workflows/packaging.yml`) that builds the application, produces AppImage and Debian artifacts, and uploads them for inspection.

## First Run Wizard

On first launch GamePerf Manager presents an onboarding flow located at `qml/onboarding/FirstRunWizard.qml`. It requests Steam library paths, Legendary CLI location, Wine prefixes, and verifies the daemon status.

## License

MIT License – see [LICENSE](LICENSE).
