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
            text: "Language"
            font.pixelSize: AppTheme.titleFontSize
            color: AppTheme.textColor
        }

        Label {
            text: "Choose your preferred display language."
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
                    text: "Interface Language"
                    font.pixelSize: AppTheme.headerFontSize
                    color: AppTheme.textColor
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 280
                    clip: true
                    spacing: 8

                    model: ListModel {
                        ListElement { code: "en-US"; name: "English (US)"; flag: "🇺🇸" }
                        ListElement { code: "es-ES"; name: "Español"; flag: "🇪🇸" }
                        ListElement { code: "fr-FR"; name: "Français"; flag: "🇫🇷" }
                        ListElement { code: "de-DE"; name: "Deutsch"; flag: "🇩🇪" }
                        ListElement { code: "ja-JP"; name: "日本語"; flag: "🇯🇵" }
                        ListElement { code: "zh-CN"; name: "简体中文"; flag: "🇨🇳" }
                    }

                    delegate: ItemDelegate {
                        width: ListView.view.width
                        height: 56

                        background: Rectangle {
                            color: SettingsStore.language === model.code ? AppTheme.primaryColor : 
                                   (parent.hovered ? AppTheme.borderColor : "transparent")
                            border.color: parent.activeFocus ? AppTheme.focusColor : "transparent"
                            border.width: AppTheme.focusBorderWidth
                            radius: AppTheme.radius
                        }

                        contentItem: RowLayout {
                            spacing: AppTheme.spacing

                            Text {
                                text: model.flag
                                font.pixelSize: AppTheme.headerFontSize
                                color: AppTheme.textColor
                                Layout.preferredWidth: 40
                            }

                            Label {
                                text: model.name
                                font.pixelSize: AppTheme.bodyFontSize
                                color: SettingsStore.language === model.code ? 
                                    AppTheme.backgroundColor : AppTheme.textColor
                                Layout.fillWidth: true
                            }

                            Label {
                                text: SettingsStore.language === model.code ? "✓" : ""
                                font.pixelSize: AppTheme.headerFontSize
                                color: SettingsStore.language === model.code ? 
                                    AppTheme.backgroundColor : AppTheme.successColor
                            }
                        }

                        onClicked: {
                            SettingsStore.language = model.code
                        }
                    }
                }

                Label {
                    text: "Application restart may be required for full language change."
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
