import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../styles"

FocusScope {
    id: root
    property alias title: titleLabel.text
    property string description: ""
    default property alias content: contentFrame.data

    Pane {
        id: pane
        anchors.fill: parent
        padding: AppStyle.spacing.lg
        background: Rectangle {
            radius: AppStyle.radius.lg
            color: AppStyle.colors.surface
            border.color: root.activeFocus ? AppStyle.colors.borderFocus : AppStyle.colors.border
            border.width: root.activeFocus ? 2 : 1
        }

        ColumnLayout {
            id: layout
            anchors.fill: parent
            spacing: AppStyle.spacing.md

            Label {
                id: titleLabel
                text: qsTr("Section")
                font.pixelSize: AppStyle.fontSize.xl
                color: AppStyle.colors.textPrimary
                Layout.alignment: Qt.AlignLeft
            }

            Label {
                text: root.description
                font.pixelSize: AppStyle.fontSize.md
                color: AppStyle.colors.textSecondary
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            Item {
                id: contentFrame
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
