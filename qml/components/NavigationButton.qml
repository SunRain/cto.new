import QtQuick 2.15
import QtQuick.Controls 2.15
import "../styles"
import "." as Components

AbstractButton {
    id: root

    property string iconText: ""
    property bool isActive: false

    width: AppStyle.touch.idealTargetSize * 2
    height: AppStyle.touch.idealTargetSize

    hoverEnabled: true
    focusPolicy: Qt.StrongFocus

    background: Rectangle {
        color: {
            if (root.isActive) return AppStyle.colors.primary
            if (root.pressed) return AppStyle.colors.surfaceVariant
            if (root.hovered || root.activeFocus) return AppStyle.colors.surface
            return "transparent"
        }
        radius: AppStyle.radius.md

        border.width: root.activeFocus ? 2 : 0
        border.color: AppStyle.colors.borderFocus

        Behavior on color {
            ColorAnimation { duration: AppStyle.animation.fast }
        }
    }

    contentItem: Row {
        anchors.centerIn: parent
        spacing: AppStyle.spacing.sm

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.iconText
            font.pixelSize: AppStyle.fontSize.lg
            color: root.isActive ? AppStyle.colors.onPrimary : AppStyle.colors.textPrimary
            font.family: "Sans Serif"
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.text
            font.pixelSize: AppStyle.fontSize.md
            color: root.isActive ? AppStyle.colors.onPrimary : AppStyle.colors.textPrimary
        }
    }

    Keys.onReturnPressed: root.clicked()
    Keys.onEnterPressed: root.clicked()

    Components.FocusOutline {
        anchors.fill: parent
        target: root
    }
}
