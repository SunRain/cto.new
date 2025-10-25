# Running Manager Module

A Qt/QML-based runtime management module for tracking and managing launched games with real-time performance metrics and alerts.

## Features

- **Gamescope Launcher**: Launch games through Gamescope with customizable performance profiles
- **Performance Profiles**: Per-game settings for FPS cap, scaling (FSR), VSync, and window modes
- **Platform Support**: Native, Steam, Epic, Wine, and Proton compatibility layers
- **Process Monitoring**: Track launched games with their PIDs
- **Performance Metrics**: Monitor CPU, GPU, RAM usage, temperatures, power consumption, and FPS
- **Alert System**: Automatic alerts for overheating or abnormal resource usage
- **Suspend/Resume**: Suspend and resume games (where supported)
- **Force Quit**: Forcefully terminate unresponsive games
- **UI Overlay**: QML-based overlay accessible via quick menu
- **Exit to Shell**: Return to shell from running games

## Components

### Backend

- **GamescopeLauncher**: Launches games with Gamescope wrapper and performance profiles
- **PerformanceProfile**: Configurable per-game settings for graphics and performance
- **RunningManager**: Main manager class that tracks running games and monitors metrics
- **ProcessMetricsProvider**: Abstract interface for metrics collection
- **LinuxMetricsProvider**: Linux-specific implementation using `/proc` filesystem

### UI

- **RunningOverlay.qml**: Main UI overlay showing:
  - List of running games with live metrics
  - Performance indicators (CPU, GPU, RAM, Temperature, FPS)
  - Alert banner for warnings and critical issues
  - Controls for switching, suspending/resuming, and force-quitting games

## Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running Tests

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly:

```bash
./runtime_manager_tests
```

## Usage

### Launching a Game with Gamescope

```cpp
Launch::LaunchConfig config;
config.titleId = "game-001";
config.displayName = "My Game";
config.executable = "/usr/bin/mygame";
config.platform = Launch::LaunchPlatform::Native;

Launch::PerformanceProfile profile = Launch::PerformanceProfile::defaultProfile();
profile.fpsCap = 60;
profile.scaling = Launch::ScalingMode::FSR;
profile.vsync = true;
profile.windowMode = Launch::WindowMode::Fullscreen;
config.profile = profile;

gamescopeLauncher->launch(config);
```

### Launching Wine/Proton Games

```cpp
Launch::LaunchConfig config;
config.titleId = "game-002";
config.displayName = "Windows Game";
config.executable = "C:\\Game\\game.exe";
config.platform = Launch::LaunchPlatform::Proton;
config.protonPath = "/path/to/proton";
config.winePrefix = "/home/user/.wine";
config.profile = Launch::PerformanceProfile::defaultProfile();

gamescopeLauncher->launch(config);
```

### Dry-Run Mode for Testing

```cpp
config.dryRun = true;
gamescopeLauncher->launch(config);
```

### Registering a Game

```cpp
runningManager->registerGame(
    "game-id",           // Unique game identifier
    "Game Name",         // Display name
    12345,               // Process ID
    true,                // Supports suspend/resume
    ""                   // Reason if suspend not supported
);
```

### Monitoring Metrics

Metrics are automatically collected at regular intervals (default: 1000ms). Access them via:

```cpp
QVariantMap metrics = runningManager->metricsFor("game-id");
```

### Suspend/Resume

```cpp
runningManager->suspendGame("game-id");
runningManager->resumeGame("game-id");
```

### Force Quit

```cpp
runningManager->forceQuit("game-id");
```

## Alert Thresholds

The following thresholds trigger alerts:

- **CPU Usage**: ≥ 95%
- **GPU Usage**: ≥ 95%
- **RAM Usage**: ≥ 90%
- **Temperature**: ≥ 85°C (Critical at ≥ 90°C)
- **GPU Temperature**: ≥ 85°C (Critical at ≥ 90°C)
- **Power Consumption**: ≥ 120W
- **FPS**: < 15

## Signals

### RunningManager Signals

- `gamesChanged()`: Emitted when the list of games changes
- `alertsChanged()`: Emitted when alerts are raised or cleared
- `focusRequested(titleId, pid)`: Emitted when focus is requested for a game
- `suspendRequested(titleId, pid)`: Emitted when suspend is requested
- `resumeRequested(titleId, pid)`: Emitted when resume is requested
- `forceQuitRequested(titleId, pid)`: Emitted when force quit is requested
- `alertRaised(titleId, alert)`: Emitted when a new alert is raised
- `alertCleared(titleId, type)`: Emitted when an alert is cleared
- `gameSuspended(titleId)`: Emitted when a game is suspended
- `gameResumed(titleId)`: Emitted when a game is resumed
- `gameClosed(titleId)`: Emitted when a game is closed

## Architecture

```
┌─────────────────────────────────────────┐
│         RunningOverlay.qml              │
│         (QML UI Layer)                  │
└──────────────┬──────────────────────────┘
               │
               │ Q_PROPERTY / Q_INVOKABLE
               │
┌──────────────▼──────────────────────────┐
│         RunningManager                  │
│    (Qt Object - Business Logic)         │
└──────────────┬──────────────────────────┘
               │
               │ Abstract Interface
               │
┌──────────────▼──────────────────────────┐
│    ProcessMetricsProvider               │
│         (Abstract)                      │
└──────────────┬──────────────────────────┘
               │
               │ Implementation
               │
┌──────────────▼──────────────────────────┐
│    LinuxMetricsProvider                 │
│    (Linux /proc implementation)         │
└─────────────────────────────────────────┘
```

## Testing

The module includes comprehensive unit tests covering:

- Game registration and lifecycle
- Metrics collection and updates
- Alert thresholds and triggers
- Suspend/resume functionality
- Multiple simultaneous games
- Invalid process handling

All tests use a mock metrics provider to ensure deterministic behavior.

## License

See project LICENSE file.
