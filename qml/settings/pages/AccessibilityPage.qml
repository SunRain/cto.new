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
            text: "Accessibility"
            font.pixelSize: AppTheme.titleFontSize
            color: AppTheme.textColor
        }

        Label {
            text: "Adjust visibility and assistive options."
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
                spacing: AppTheme.spacing * 1.2

                Label {
                    text: "Text Scaling"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                Slider {
                    id: textScaleSlider
                    Layout.fillWidth: true
                    from: 0.75
                    to: 1.75
                    stepSize: 0.05
                    value: SettingsStore.textScale
                    onMoved: SettingsStore.textScale = value
                }

                Label {
                    text: Math.round(SettingsStore.textScale * 100) + "%"
                    font.pixelSize: AppTheme.bodyFontSize
                    color: AppTheme.secondaryTextColor
                }

                Switch {
                    text: "High Contrast Theme"
                    checked: SettingsStore.highContrast
                    onToggled: SettingsStore.highContrast = checked
                }

                Switch {
                    text: "Captions"
                    checked: SettingsStore.captionsEnabled
                    onToggled: SettingsStore.captionsEnabled = checked
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
            padding: AppTheme.margin

            ColumnLayout {
                width: parent.width
                spacing: AppTheme.spacing

                Label {
                    text: "Preview"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 160
                    color: AppTheme.backgroundColor
                    border.color: AppTheme.borderColor
                    radius: AppTheme.radius

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: AppTheme.spacing
                        Label {
                            text: "This is sample text with current scaling"
                            font.pixelSize: AppTheme.bodyFontSize
                            color: AppTheme.textColor
                        }
                        Label {
                            text: SettingsStore.captionsEnabled ? "Captions are active" : "Captions are disabled"
                            font.pixelSize: AppTheme.smallFontSize
                            color: SettingsStore.captionsEnabled ? AppTheme.successColor : AppTheme.secondaryTextColor
                        }
                    }
                }
            }
        }
    }
}
