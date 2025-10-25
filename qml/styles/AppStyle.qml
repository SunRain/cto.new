pragma Singleton
import QtQuick 2.15

QtObject {
    readonly property bool isDarkMode: true

    readonly property QtObject spacing: QtObject {
        readonly property int xxs: 2
        readonly property int xs: 4
        readonly property int sm: 8
        readonly property int md: 16
        readonly property int lg: 24
        readonly property int xl: 32
        readonly property int xxl: 48
    }

    readonly property QtObject radius: QtObject {
        readonly property int sm: 4
        readonly property int md: 8
        readonly property int lg: 12
        readonly property int xl: 16
    }

    readonly property QtObject fontSize: QtObject {
        readonly property int xs: 11
        readonly property int sm: 13
        readonly property int md: 15
        readonly property int lg: 18
        readonly property int xl: 24
        readonly property int xxl: 32
    }

    readonly property QtObject touch: QtObject {
        readonly property int minTargetSize: 44
        readonly property int idealTargetSize: 48
    }

    readonly property QtObject animation: QtObject {
        readonly property int fast: 150
        readonly property int normal: 250
        readonly property int slow: 350
        readonly property int easing: Easing.OutCubic
    }

    readonly property QtObject colors: isDarkMode ? darkPalette : lightPalette

    readonly property QtObject darkPalette: QtObject {
        readonly property color background: "#121212"
        readonly property color backgroundElevated: "#1E1E1E"
        readonly property color surface: "#2C2C2C"
        readonly property color surfaceVariant: "#3A3A3A"

        readonly property color primary: "#BB86FC"
        readonly property color primaryVariant: "#9965D4"
        readonly property color secondary: "#03DAC6"
        readonly property color secondaryVariant: "#018786"

        readonly property color onBackground: "#E1E1E1"
        readonly property color onSurface: "#E1E1E1"
        readonly property color onPrimary: "#000000"
        readonly property color onSecondary: "#000000"

        readonly property color textPrimary: "#FFFFFF"
        readonly property color textSecondary: "#B3B3B3"
        readonly property color textDisabled: "#666666"

        readonly property color border: "#404040"
        readonly property color borderFocus: "#BB86FC"
        readonly property color divider: "#2C2C2C"

        readonly property color error: "#CF6679"
        readonly property color success: "#4CAF50"
        readonly property color warning: "#FFC107"
    }

    readonly property QtObject lightPalette: QtObject {
        readonly property color background: "#FAFAFA"
        readonly property color backgroundElevated: "#FFFFFF"
        readonly property color surface: "#FFFFFF"
        readonly property color surfaceVariant: "#F5F5F5"

        readonly property color primary: "#6200EE"
        readonly property color primaryVariant: "#3700B3"
        readonly property color secondary: "#03DAC6"
        readonly property color secondaryVariant: "#018786"

        readonly property color onBackground: "#000000"
        readonly property color onSurface: "#000000"
        readonly property color onPrimary: "#FFFFFF"
        readonly property color onSecondary: "#000000"

        readonly property color textPrimary: "#000000"
        readonly property color textSecondary: "#757575"
        readonly property color textDisabled: "#BDBDBD"

        readonly property color border: "#E0E0E0"
        readonly property color borderFocus: "#6200EE"
        readonly property color divider: "#E0E0E0"

        readonly property color error: "#B00020"
        readonly property color success: "#4CAF50"
        readonly property color warning: "#FF9800"
    }
}
