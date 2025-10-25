# Game Launcher - Settings Experience

A Qt6 QML-based settings interface for a game launcher application with full controller and touch support.

## Features

### Settings Navigation (`qml/settings/SettingsHome.qml`)
- 7 main sections: Network, Display, Audio, Language, Accessibility, Accounts, Downloads
- Sidebar navigation with clear focus indicators
- Controller-friendly (D-pad navigation, Enter/Return to select)
- Touch-friendly with hover states

### Pages

#### Network (`qml/settings/pages/NetworkPage.qml`)
- Network mode selection (Auto, Wired, Wireless, Offline)
- Metered connection toggle
- Available networks display with signal strength
- Platform integration stub for network manager interface

#### Display (`qml/settings/pages/DisplayPage.qml`)
- Display scaling (75% - 200%)
- Quick presets for common scales
- Live preview of scaling changes
- Platform integration stub for display interface

#### Audio (`qml/settings/pages/AudioPage.qml`)
- Audio output device selection
- Volume control
- Test output functionality
- Platform integration stub for audio device enumeration

#### Language (`qml/settings/pages/LanguagePage.qml`)
- Interface language selection
- Supported: en-US, es-ES, fr-FR, de-DE, ja-JP, zh-CN
- Visual indicators with flags
- Note about application restart requirement

#### Accessibility (`qml/settings/pages/AccessibilityPage.qml`)
- Text scaling (75% - 175%)
- High contrast theme toggle (immediate UI update)
- Captions toggle
- Live preview of changes

#### Accounts (`qml/settings/pages/AccountsPage.qml`)
- Steam account status with troubleshooting hints
- Epic Games account status via Legendary CLI integration
- Legendary CLI path configuration
- Visual connection status indicators

#### Downloads (`qml/settings/pages/DownloadsPage.qml`)
- Download progress display (placeholder)
- Progress API ready for integration
- Simulate and reset controls for testing

### Settings Persistence (`src/settings/SettingsStore.cpp`)

- QSettings-based persistent storage
- Real-time UI updates via Qt signals
- All settings persist across application restarts
- Organized by category (network, display, audio, language, accessibility, accounts, downloads)

### Theme System (`qml/shared/AppTheme.qml`)

- Singleton theme accessible throughout the application
- Reactive to accessibility settings:
  - High contrast mode
  - Text scaling
  - Display scaling
- Immediate UI updates when theme properties change
- Controller focus indicators (larger in high contrast mode)

## Testing

### Persistence Tests (`tests/qml/tst_settingspersistence.qml`)
- Verify all settings persist correctly
- Test restore defaults functionality
- Test sync and reload operations
- Test raw value access

### Navigation Tests (`tests/qml/tst_settingsnavigation.qml`)
- Verify keyboard/controller navigation
- Test page switching
- Verify focus management

### Accessibility Tests (`tests/qml/tst_settingsaccessibility.qml`)
- Verify high contrast theme changes
- Test text and display scaling
- Verify immediate UI updates

## Building

```bash
cmake -B build -S .
cmake --build build
```

## Running Tests

```bash
cd build
ctest
```

Or run individual tests:
```bash
./tst_settingspersistence
./tst_settingsnavigation
./tst_settingsaccessibility
```

## Architecture

- **C++ Backend**: SettingsStore provides QML singleton for settings management
- **QML Frontend**: Modular pages with shared theme
- **Theme Binding**: AppTheme reacts to SettingsStore changes for immediate UI updates
- **Platform Stubs**: Ready for integration with actual platform services

## Controller Support

- D-pad/Arrow keys for navigation
- Enter/Return to select
- Focus indicators with high visibility
- Smooth highlight animations

## Touch Support

- Tap to select
- Hover states for visual feedback
- Large touch targets (48dp minimum)

## Accessibility

- Text scaling up to 175%
- High contrast mode with increased borders
- Caption support
- Keyboard/controller navigation with clear focus
- No frame drops during navigation or setting changes
