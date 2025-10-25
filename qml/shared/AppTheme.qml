pragma Singleton

import QtQuick
import App.Settings 1.0

QtObject {
    id: theme

    readonly property bool highContrast: SettingsStore.highContrast
    readonly property real textScale: SettingsStore.textScale
    readonly property real displayScale: SettingsStore.displayScale

    readonly property color backgroundColor: highContrast ? "#000000" : "#11161d"
    readonly property color surfaceColor: highContrast ? "#1a1a1a" : "#1f2833"
    readonly property color primaryColor: highContrast ? "#ffffff" : "#4aa8ff"
    readonly property color textColor: highContrast ? "#ffffff" : "#f5f7fa"
    readonly property color secondaryTextColor: highContrast ? "#e0e0e0" : "#a7b2c2"
    readonly property color borderColor: highContrast ? "#ffffff" : "#2e3a4a"
    readonly property color focusColor: highContrast ? "#00ffff" : "#4aa8ff"
    readonly property color successColor: highContrast ? "#00ff6a" : "#58d68d"
    readonly property color errorColor: highContrast ? "#ff5f5f" : "#ff7070"

    readonly property int baseFontSize: 14
    readonly property int titleFontSize: Math.round(24 * textScale)
    readonly property int headerFontSize: Math.round(18 * textScale)
    readonly property int bodyFontSize: Math.round(15 * textScale)
    readonly property int smallFontSize: Math.round(13 * textScale)

    readonly property int spacing: Math.round(12 * displayScale)
    readonly property int margin: Math.round(18 * displayScale)
    readonly property int radius: highContrast ? 2 : 8
    readonly property int focusBorderWidth: highContrast ? 3 : 2
}
