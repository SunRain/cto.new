import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import App.Shared 1.0
import App.Settings 1.0

FocusScope {
    id: root
    focus: true

    property alias modeSelector: modeSelector

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: AppTheme.margin
        spacing: AppTheme.spacing * 1.5

        Label {
            text: "Network"
            font.pixelSize: AppTheme.titleFontSize
            color: AppTheme.textColor
        }

        Label {
            text: "Manage connectivity and link quality."
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

                ComboBox {
                    id: modeSelector
                    Layout.fillWidth: true
                    model: SettingsStore.availableNetworkModes
                    currentIndex: Math.max(0, model.indexOf(SettingsStore.networkMode))
                    textRole: ""
                    onActivated: SettingsStore.networkMode = currentText
                    delegate: ItemDelegate {
                        width: parent.width
                        text: modelData
                    }
                }

                Switch {
                    text: "Metered Connection"
                    checked: SettingsStore.networkMetered
                    onToggled: SettingsStore.networkMetered = checked
                }

                Button {
                    text: "Troubleshoot"
                    Layout.preferredWidth: 200
                }
            }
        }

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            background: Rectangle {
                color: AppTheme.surfaceColor
                radius: AppTheme.radius
                border.color: AppTheme.borderColor
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: AppTheme.margin
                spacing: AppTheme.spacing

                Label {
                    text: "Available Networks"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                Repeater {
                    model: [
                        { name: "Home Wi-Fi", strength: 92, secure: true },
                        { name: "Office", strength: 86, secure: true },
                        { name: "Public Hotspot", strength: 43, secure: false }
                    ]

                    delegate: Rectangle {
                        width: parent.width
                        height: 52
                        color: "transparent"
                        border.color: AppTheme.borderColor
                        border.width: 1
                        radius: AppTheme.radius

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: AppTheme.spacing
                            spacing: AppTheme.spacing

                            Label {
                                text: modelData.name
                                color: AppTheme.textColor
                                font.pixelSize: AppTheme.bodyFontSize
                                Layout.fillWidth: true
                            }

                            ProgressBar {
                                Layout.preferredWidth: 160
                                value: modelData.strength / 100
                            }

                            Label {
                                text: modelData.secure ? "Secure" : "Open"
                                color: modelData.secure ? AppTheme.successColor : AppTheme.errorColor
                                font.pixelSize: AppTheme.smallFontSize
                            }
                        }
                    }
                }
            }
        }
    }
}
