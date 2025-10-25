import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import App.Shared 1.0
import App.Settings 1.0

FocusScope {
    id: root
    focus: true

    property alias scaleSlider: scaleSlider

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: AppTheme.margin
        spacing: AppTheme.spacing * 1.5

        Label {
            text: "Display"
            font.pixelSize: AppTheme.titleFontSize
            color: AppTheme.textColor
        }

        Label {
            text: "Configure screen scaling and visual preferences."
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
                spacing: AppTheme.spacing * 2

                Label {
                    text: "Display Scale: " + (Math.round(SettingsStore.displayScale * 100)) + "%"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                Slider {
                    id: scaleSlider
                    Layout.fillWidth: true
                    from: 0.75
                    to: 2.0
                    stepSize: 0.25
                    value: SettingsStore.displayScale
                    onValueChanged: {
                        if (pressed) {
                            SettingsStore.displayScale = value
                        }
                    }

                    background: Rectangle {
                        x: scaleSlider.leftPadding
                        y: scaleSlider.topPadding + scaleSlider.availableHeight / 2 - height / 2
                        width: scaleSlider.availableWidth
                        height: 4
                        radius: 2
                        color: AppTheme.borderColor

                        Rectangle {
                            width: scaleSlider.visualPosition * parent.width
                            height: parent.height
                            color: AppTheme.primaryColor
                            radius: 2
                        }
                    }

                    handle: Rectangle {
                        x: scaleSlider.leftPadding + scaleSlider.visualPosition * (scaleSlider.availableWidth - width)
                        y: scaleSlider.topPadding + scaleSlider.availableHeight / 2 - height / 2
                        width: 20
                        height: 20
                        radius: 10
                        color: scaleSlider.pressed ? Qt.lighter(AppTheme.primaryColor, 1.2) : AppTheme.primaryColor
                        border.color: scaleSlider.activeFocus ? AppTheme.focusColor : "transparent"
                        border.width: AppTheme.focusBorderWidth
                    }
                }

                Row {
                    spacing: 8
                    Repeater {
                        model: SettingsStore.displayScaleOptions
                        delegate: Button {
                            text: Math.round(parseFloat(modelData) * 100) + "%"
                            onClicked: SettingsStore.displayScale = parseFloat(modelData)
                            flat: true
                        }
                    }
                }
            }
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
                    text: "Preview"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    color: AppTheme.backgroundColor
                    border.color: AppTheme.borderColor
                    radius: AppTheme.radius

                    ColumnLayout {
                        anchors.centerIn: parent
                        Label {
                            text: "Sample Text"
                            font.pixelSize: AppTheme.titleFontSize
                            color: AppTheme.textColor
                            Layout.alignment: Qt.AlignHCenter
                        }
                        Label {
                            text: "This text scales with your display settings"
                            font.pixelSize: AppTheme.bodyFontSize
                            color: AppTheme.secondaryTextColor
                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
