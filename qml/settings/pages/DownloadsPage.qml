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
            text: "Downloads"
            font.pixelSize: AppTheme.titleFontSize
            color: AppTheme.textColor
        }

        Label {
            text: "Monitor active downloads from connected stores."
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
                spacing: AppTheme.spacing

                Label {
                    text: "Download Manager"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                Label {
                    text: SettingsStore.downloadProgress > 0 ?
                        "Active download in progress" : "Idle"
                    font.pixelSize: AppTheme.bodyFontSize
                    color: SettingsStore.downloadProgress > 0 ? AppTheme.successColor : AppTheme.secondaryTextColor
                }

                ProgressBar {
                    Layout.fillWidth: true
                    value: SettingsStore.downloadProgress
                }

                RowLayout {
                    spacing: AppTheme.spacing
                    Button {
                        text: "Simulate Progress"
                        Layout.preferredWidth: 180
                        onClicked: SettingsStore.downloadProgress = Math.min(1.0, SettingsStore.downloadProgress + 0.1)
                    }
                    Button {
                        text: "Reset"
                        Layout.preferredWidth: 160
                        onClicked: SettingsStore.downloadProgress = 0
                    }
                }

                Label {
                    text: "API ready: integrate with download service to bind realtime progress."
                    font.pixelSize: AppTheme.smallFontSize
                    color: AppTheme.secondaryTextColor
                    wrapMode: Text.Wrap
                    Layout.fillWidth: true
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
