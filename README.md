# Qt Quick Application

A modern Qt 6 + QML desktop application with touch-friendly UI and gamepad navigation support.

## Features

- **Navigation Shell**: Home, Library, Favorites, Recent, Running, and Settings views
- **Dark/Light Theme Support**: Adaptive color palettes with scalable typography
- **60 FPS Performance**: Optimized render loop for smooth animations
- **Controller Focus States**: Clear visual feedback for keyboard and gamepad navigation
- **Touch-Friendly**: Minimum 44px touch targets throughout the interface
- **Internationalization**: Translation hooks ready for multi-language support

## Build Prerequisites

### Required

- **Qt 6.2 or later** with the following modules:
  - Qt Quick
  - Qt Quick Controls 2
  - Qt DBus
  - Qt Linguist Tools
  - Qt Core5Compat (for graphical effects)
- **CMake 3.21+**
- **C++17 compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)

### Linux

```bash
# Ubuntu/Debian
sudo apt-get install qt6-base-dev qt6-declarative-dev qt6-tools-dev \
                     qt6-5compat-dev libqt6dbus6 cmake build-essential

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qttools-devel \
                 qt6-qt5compat-devel cmake gcc-c++

# Arch Linux
sudo pacman -S qt6-base qt6-declarative qt6-tools qt6-5compat cmake
```

## Building

```bash
# Configure the build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build --parallel

# Run the application
./build/QtQuickApp
```

### Debug Build

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel
./build-debug/QtQuickApp
```

## Running Tests

```bash
cd build
ctest --output-on-failure
```

Or use the CI build script:

```bash
./scripts/ci_build.sh
```

## Project Structure

```
.
├── CMakeLists.txt          # Top-level build configuration
├── cmake/                  # CMake helper modules
│   └── QtHelpers.cmake
├── src/                    # C++ source files
│   ├── main.cpp            # Application entry point
│   └── ui/                 # View models and data models
│       ├── NavigationModel.{h,cpp}
│       ├── DataModel.{h,cpp}
│       └── TranslationStore.{h,cpp}
├── qml/                    # QML interface files
│   ├── AppShell.qml        # Main application shell
│   ├── components/         # Reusable UI components
│   │   ├── FocusOutline.qml
│   │   └── NavigationButton.qml
│   ├── styles/             # Theme and styling
│   │   ├── AppStyle.qml
│   │   └── qmldir
│   └── views/              # View templates
│       └── SectionView.qml
├── resources/              # Assets and resources
├── tests/                  # Test suite
│   └── qmltests/           # QML smoke tests
└── scripts/                # Build and CI scripts
    └── ci_build.sh
```

## Navigation

### Keyboard

- **Tab / Shift+Tab**: Navigate between UI elements
- **Arrow Keys**: Navigate within lists and menus
- **Enter / Return**: Activate focused button
- **Escape**: Close dialogs and menus

### Controller

- **D-Pad**: Navigate between buttons
- **A Button (South)**: Activate
- **B Button (East)**: Back/Cancel

## Development

### Adding New Views

1. Create a new QML file in `qml/views/`
2. Add it to `qml/qml.qrc`
3. Register the route in `NavigationModel.cpp`
4. Add a `SectionView` entry in `AppShell.qml`

### Customizing Theme

Edit `qml/styles/AppStyle.qml` to modify colors, spacing, fonts, and animation timings.

### Translation

1. Mark translatable strings with `qsTr("Text")`
2. Extract strings: `lupdate . -ts translations/app_en.ts`
3. Translate with Qt Linguist
4. Compile: `lrelease translations/app_en.ts`

## License

Copyright (c) 2024. All rights reserved.
