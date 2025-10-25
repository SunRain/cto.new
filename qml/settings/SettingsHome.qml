import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import App.Shared 1.0
import App.Settings 1.0

FocusScope {
    id: root
    focus: true

    property int currentPageIndex: 0
    readonly property var navigationModel: navModel

    Keys.onLeftPressed: {
        if (!menuList.activeFocus) {
            menuList.forceActiveFocus()
            event.accepted = true
        }
    }

    Keys.onRightPressed: {
        if (menuList.activeFocus && pageLoader.item && pageLoader.item.forceActiveFocus) {
            pageLoader.item.forceActiveFocus()
            event.accepted = true
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: sidebar
            Layout.fillHeight: true
            Layout.preferredWidth: 260 * AppTheme.displayScale
            color: AppTheme.surfaceColor

            ListView {
                id: menuList
                anchors.fill: parent
                anchors.margins: AppTheme.margin
                spacing: AppTheme.spacing
                focus: true
                clip: true
                keyNavigationWraps: true
                activeFocusOnTab: true

                model: ListModel {
                    id: navModel
                    ListElement { title: "Network"; icon: "⚡"; page: "network" }
                    ListElement { title: "Display"; icon: "🖥"; page: "display" }
                    ListElement { title: "Audio"; icon: "🔊"; page: "audio" }
                    ListElement { title: "Language"; icon: "🌐"; page: "language" }
                    ListElement { title: "Accessibility"; icon: "♿"; page: "accessibility" }
                    ListElement { title: "Accounts"; icon: "👤"; page: "accounts" }
                    ListElement { title: "Downloads"; icon: "📥"; page: "downloads" }
                }

                delegate: ItemDelegate {
                    id: menuItem
                    width: menuList.width
                    height: 52 * AppTheme.displayScale

                    background: Rectangle {
                        color: menuItem.activeFocus ? AppTheme.focusColor :
                               (menuItem.hovered ? AppTheme.borderColor : "transparent")
                        border.color: menuItem.activeFocus ? AppTheme.focusColor : "transparent"
                        border.width: AppTheme.focusBorderWidth
                        radius: AppTheme.radius
                    }

                    contentItem: RowLayout {
                        spacing: AppTheme.spacing

                        Text {
                            text: icon
                            font.pixelSize: AppTheme.headerFontSize
                            color: AppTheme.textColor
                            Layout.preferredWidth: 32 * AppTheme.displayScale
                        }

                        Text {
                            text: title
                            font.pixelSize: AppTheme.bodyFontSize
                            color: menuItem.activeFocus ? AppTheme.backgroundColor : AppTheme.textColor
                            Layout.fillWidth: true
                        }
                    }

                    onClicked: {
                        menuList.currentIndex = index
                    }

                    Keys.onReturnPressed: clicked()
                    Keys.onEnterPressed: clicked()
                }

                onCurrentIndexChanged: {
                    if (currentIndex < 0 || currentIndex >= count) {
                        return
                    }
                    const entry = model.get(currentIndex)
                    root.currentPageIndex = currentIndex
                    pageLoader.setSource(getPageSource(entry.page))
                }

                Keys.onRightPressed: {
                    if (pageLoader.item && pageLoader.item.forceActiveFocus) {
                        pageLoader.item.forceActiveFocus()
                        event.accepted = true
                    }
                }

                highlight: Rectangle {
                    color: AppTheme.primaryColor
                    radius: AppTheme.radius
                }
                highlightMoveDuration: 150

                Component.onCompleted: {
                    currentIndex = 0
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: AppTheme.backgroundColor

            Loader {
                id: pageLoader
                anchors.fill: parent
                anchors.margins: AppTheme.margin * 2
                asynchronous: true

                onLoaded: {
                    if (item && item.forceActiveFocus) {
                        item.forceActiveFocus()
                    }
                }

                Component.onCompleted: {
                    setSource(getPageSource("network"))
                }
            }
        }
    }

    function getPageSource(page) {
        switch (page) {
        case "network": return "pages/NetworkPage.qml"
        case "display": return "pages/DisplayPage.qml"
        case "audio": return "pages/AudioPage.qml"
        case "language": return "pages/LanguagePage.qml"
        case "accessibility": return "pages/AccessibilityPage.qml"
        case "accounts": return "pages/AccountsPage.qml"
        case "downloads": return "pages/DownloadsPage.qml"
        default: return "pages/NetworkPage.qml"
        }
    }
}
