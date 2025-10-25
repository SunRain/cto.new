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
            text: "Audio"
            font.pixelSize: AppTheme.titleFontSize
            color: AppTheme.textColor
        }

        Label {
            text: "Select playback device and adjust levels."
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
                    Layout.fillWidth: true
                    model: SettingsStore.availableAudioOutputs
                    currentIndex: Math.max(0, model.indexOf(SettingsStore.audioOutput))
                    onActivated: SettingsStore.audioOutput = currentText
                }

                Label {
                    text: "Volume: " + SettingsStore.audioVolume + "%"
                    font.pixelSize: AppTheme.bodyFontSize
                    color: AppTheme.textColor
                }

                Slider {
                    id: volumeSlider
                    Layout.fillWidth: true
                    from: 0
                    to: 100
                    value: SettingsStore.audioVolume
                    onMoved: SettingsStore.audioVolume = Math.round(value)
                    ToolTip.visible: hovered
                    ToolTip.text: Math.round(value) + "%"
                }

                RowLayout {
                    spacing: AppTheme.spacing
                    Label {
                        text: "Legendary CLI Path"
                        font.pixelSize: AppTheme.bodyFontSize
                        color: AppTheme.textColor
                        Layout.preferredWidth: 180
                    }
                    TextField {
                        text: SettingsStore.legendaryCliPath
                        Layout.fillWidth: true
                        placeholderText: "/usr/bin/legendary"
                        onEditingFinished: SettingsStore.legendaryCliPath = text
                    }
                }

                RowLayout {
                    spacing: AppTheme.spacing
                    Button {
                        text: "Test Output"
                        Layout.preferredWidth: 160
                    }
                    Label {
                        text: "Output device ready"
                        font.pixelSize: AppTheme.smallFontSize
                        color: AppTheme.successColor
                    }
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
                    text: "Audio Tips"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                Label {
                    text: "Ensure HDMI displays are powered on to enumerate outputs."
                    wrapMode: Text.Wrap
                    color: AppTheme.secondaryTextColor
                }

                Label {
                    text: "Use Legendary CLI to manage Epic audio middleware downloads."
                    wrapMode: Text.Wrap
                    color: AppTheme.secondaryTextColor
                }
            }
        }
    }
}
