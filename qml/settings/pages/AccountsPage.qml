import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import App.Shared 1.0
import App.Settings 1.0

FocusScope {
    id: root
    focus: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: AppTheme.margin
        spacing: AppTheme.spacing * 1.5

        Label {
            text: "Accounts"
            font.pixelSize: AppTheme.titleFontSize
            color: AppTheme.textColor
        }

        Label {
            text: "Connect your gaming platform accounts."
            font.pixelSize: AppTheme.bodyFontSize
            color: AppTheme.secondaryTextColor
        }

        Frame {
            Layout.fillWidth: true
            background: Rectangle {
                color: AppTheme.surfaceColor
                radius: AppTheme.radius
                border.color: AppTheme.borderColor
            }
            padding: AppTheme.margin

            ColumnLayout {
                width: parent.width
                spacing: AppTheme.spacing * 1.5

                Label {
                    text: "Steam"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                RowLayout {
                    spacing: AppTheme.spacing

                    Rectangle {
                        Layout.preferredWidth: 12
                        Layout.preferredHeight: 12
                        radius: 6
                        color: SettingsStore.steamLoggedIn ? AppTheme.successColor : AppTheme.errorColor
                    }

                    Label {
                        text: SettingsStore.steamLoggedIn ? "Connected" : "Not Connected"
                        font.pixelSize: AppTheme.bodyFontSize
                        color: SettingsStore.steamLoggedIn ? AppTheme.successColor : AppTheme.errorColor
                    }
                }

                Label {
                    text: SettingsStore.steamTroubleshootingHint
                    font.pixelSize: AppTheme.smallFontSize
                    color: AppTheme.secondaryTextColor
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                Button {
                    text: SettingsStore.steamLoggedIn ? "Disconnect" : "Connect"
                    Layout.preferredWidth: 160
                    onClicked: SettingsStore.steamLoggedIn = !SettingsStore.steamLoggedIn
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: AppTheme.borderColor
                }

                Label {
                    text: "Epic Games"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                RowLayout {
                    spacing: AppTheme.spacing

                    Rectangle {
                        Layout.preferredWidth: 12
                        Layout.preferredHeight: 12
                        radius: 6
                        color: SettingsStore.epicLoggedIn ? AppTheme.successColor : AppTheme.errorColor
                    }

                    Label {
                        text: SettingsStore.epicLoggedIn ? "Connected" : "Not Connected"
                        font.pixelSize: AppTheme.bodyFontSize
                        color: SettingsStore.epicLoggedIn ? AppTheme.successColor : AppTheme.errorColor
                    }
                }

                Label {
                    text: SettingsStore.epicTroubleshootingHint
                    font.pixelSize: AppTheme.smallFontSize
                    color: AppTheme.secondaryTextColor
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: AppTheme.spacing
                    Label {
                        text: "Legendary CLI Path:"
                        font.pixelSize: AppTheme.bodyFontSize
                        color: AppTheme.textColor
                        Layout.preferredWidth: 160
                    }
                    TextField {
                        text: SettingsStore.legendaryCliPath
                        Layout.fillWidth: true
                        placeholderText: "/usr/bin/legendary"
                        onEditingFinished: SettingsStore.legendaryCliPath = text
                    }
                }

                Button {
                    text: SettingsStore.epicLoggedIn ? "Disconnect" : "Connect"
                    Layout.preferredWidth: 160
                    onClicked: SettingsStore.epicLoggedIn = !SettingsStore.epicLoggedIn
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
