import QtQuick
import QtQuick.Window

Window {
    width: 1280
    height: 720
    visible: true
    title: "Game Launcher Settings"

    Item {
        anchors.fill: parent
        focus: true

        Loader {
            id: settingsLoader
            anchors.fill: parent
            source: "qml/settings/SettingsHome.qml"
        }

        Keys.onEscapePressed: Qt.quit()
    }
}
