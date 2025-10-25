import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects 1.0
import App.Models 1.0
import "components"
import "styles"
import "views"

Window {
    id: appWindow
    width: 1280
    height: 720
    visible: true
    title: qsTr("Qt Quick App")
    color: AppStyle.colors.background

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        Rectangle {
            id: topBar
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            color: AppStyle.colors.backgroundElevated

            layer.enabled: true
            layer.effect: DropShadow {
                verticalOffset: 2
                color: Qt.rgba(0, 0, 0, 0.3)
                radius: 8
            }

            RowLayout {
                anchors.fill: parent
                anchors.margins: AppStyle.spacing.md
                spacing: AppStyle.spacing.lg

                Text {
                    text: "⚡"
                    font.pixelSize: AppStyle.fontSize.xxl
                    color: AppStyle.colors.primary
                }

                Text {
                    text: qsTr("Qt Quick App")
                    font.pixelSize: AppStyle.fontSize.lg
                    font.bold: true
                    color: AppStyle.colors.textPrimary
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: qsTr("Quick Menu")
                    flat: true
                    font.pixelSize: AppStyle.fontSize.md
                    onClicked: quickMenu.visible = !quickMenu.visible
                }
            }
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            Rectangle {
                id: navigationPane
                SplitView.preferredWidth: 240
                SplitView.minimumWidth: 200
                SplitView.maximumWidth: 320
                color: AppStyle.colors.backgroundElevated

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: AppStyle.spacing.md
                    spacing: AppStyle.spacing.xs

                    NavigationButton {
                        id: homeButton
                        text: qsTr("Home")
                        iconText: "🏠"
                        isActive: NavigationModel.currentIndex === 0
                        onClicked: NavigationModel.setCurrentIndex(0)
                        Layout.fillWidth: true
                        KeyNavigation.down: libraryButton
                    }

                    NavigationButton {
                        id: libraryButton
                        text: qsTr("Library")
                        iconText: "📚"
                        isActive: NavigationModel.currentIndex === 1
                        onClicked: NavigationModel.setCurrentIndex(1)
                        Layout.fillWidth: true
                        KeyNavigation.up: homeButton
                        KeyNavigation.down: favoritesButton
                    }

                    NavigationButton {
                        id: favoritesButton
                        text: qsTr("Favorites")
                        iconText: "⭐"
                        isActive: NavigationModel.currentIndex === 2
                        onClicked: NavigationModel.setCurrentIndex(2)
                        Layout.fillWidth: true
                        KeyNavigation.up: libraryButton
                        KeyNavigation.down: recentButton
                    }

                    NavigationButton {
                        id: recentButton
                        text: qsTr("Recent")
                        iconText: "🕒"
                        isActive: NavigationModel.currentIndex === 3
                        onClicked: NavigationModel.setCurrentIndex(3)
                        Layout.fillWidth: true
                        KeyNavigation.up: favoritesButton
                        KeyNavigation.down: runningButton
                    }

                    NavigationButton {
                        id: runningButton
                        text: qsTr("Running")
                        iconText: "▶️"
                        isActive: NavigationModel.currentIndex === 4
                        onClicked: NavigationModel.setCurrentIndex(4)
                        Layout.fillWidth: true
                        KeyNavigation.up: recentButton
                        KeyNavigation.down: settingsButton
                    }

                    Item { Layout.fillHeight: true }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: AppStyle.colors.divider
                    }

                    NavigationButton {
                        id: settingsButton
                        text: qsTr("Settings")
                        iconText: "⚙️"
                        isActive: NavigationModel.currentIndex === 5
                        onClicked: NavigationModel.setCurrentIndex(5)
                        Layout.fillWidth: true
                        KeyNavigation.up: runningButton
                        KeyNavigation.down: homeButton
                    }
                }
            }

            Rectangle {
                id: contentPane
                SplitView.fillWidth: true
                color: AppStyle.colors.background

                StackLayout {
                    id: contentStack
                    anchors.fill: parent
                    anchors.margins: AppStyle.spacing.lg
                    currentIndex: NavigationModel.currentIndex

                    Behavior on currentIndex {
                        NumberAnimation {
                            duration: AppStyle.animation.normal
                            easing.type: AppStyle.animation.easing
                        }
                    }

                    SectionView {
                        title: qsTr("Home")
                        description: qsTr("Welcome to your media center")
                        
                        ListView {
                            id: homeList
                            anchors.fill: parent
                            model: DataModel.items
                            spacing: AppStyle.spacing.sm
                            clip: true

                            delegate: ItemDelegate {
                                width: homeList.width
                                height: AppStyle.touch.idealTargetSize + AppStyle.spacing.md

                                background: Rectangle {
                                    color: parent.pressed ? AppStyle.colors.surfaceVariant :
                                           parent.hovered ? AppStyle.colors.surface : "transparent"
                                    radius: AppStyle.radius.sm
                                }

                                contentItem: ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: AppStyle.spacing.sm

                                    Label {
                                        text: modelData.title || ""
                                        font.pixelSize: AppStyle.fontSize.md
                                        font.bold: true
                                        color: AppStyle.colors.textPrimary
                                        Layout.fillWidth: true
                                    }

                                    Label {
                                        text: modelData.subtitle || ""
                                        font.pixelSize: AppStyle.fontSize.sm
                                        color: AppStyle.colors.textSecondary
                                        Layout.fillWidth: true
                                    }
                                }
                            }

                            ScrollBar.vertical: ScrollBar {
                                policy: ScrollBar.AsNeeded
                            }
                        }
                    }

                    SectionView {
                        title: qsTr("Library")
                        description: qsTr("Browse your media collection")
                    }

                    SectionView {
                        title: qsTr("Favorites")
                        description: qsTr("Quick access to your favorite items")
                    }

                    SectionView {
                        title: qsTr("Recent")
                        description: qsTr("Recently accessed media")
                    }

                    SectionView {
                        title: qsTr("Running")
                        description: qsTr("Currently playing media")
                    }

                    SectionView {
                        title: qsTr("Settings")
                        description: qsTr("Customize your experience")
                    }
                }
            }
        }

        Rectangle {
            id: statusBar
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            color: AppStyle.colors.backgroundElevated

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: AppStyle.spacing.md
                anchors.rightMargin: AppStyle.spacing.md
                spacing: AppStyle.spacing.md

                Label {
                    text: qsTr("Ready")
                    font.pixelSize: AppStyle.fontSize.sm
                    color: AppStyle.colors.textSecondary
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: "60 FPS"
                    font.pixelSize: AppStyle.fontSize.sm
                    color: AppStyle.colors.success
                }
            }
        }
    }

    Popup {
        id: quickMenu
        x: parent.width - width - AppStyle.spacing.md
        y: topBar.height + AppStyle.spacing.sm
        width: 280
        height: 200
        modal: false
        focus: false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            color: AppStyle.colors.surface
            radius: AppStyle.radius.md
            border.color: AppStyle.colors.border
            border.width: 1
        }

        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: AppStyle.spacing.md
            spacing: AppStyle.spacing.sm

            Label {
                text: qsTr("Quick Actions")
                font.pixelSize: AppStyle.fontSize.lg
                font.bold: true
                color: AppStyle.colors.textPrimary
            }

            ItemDelegate {
                text: qsTr("Refresh")
                Layout.fillWidth: true
                onClicked: {
                    DataModel.clearItems()
                    DataModel.addPlaceholderItem(qsTr("Refreshed"), qsTr("Content updated"))
                    quickMenu.close()
                }
            }

            ItemDelegate {
                text: qsTr("About")
                Layout.fillWidth: true
                onClicked: quickMenu.close()
            }

            Item { Layout.fillHeight: true }
        }
    }

    Timer {
        id: frameCounter
        interval: 16
        running: true
        repeat: true
    }

    Component.onCompleted: {
        homeButton.forceActiveFocus()
    }
}
