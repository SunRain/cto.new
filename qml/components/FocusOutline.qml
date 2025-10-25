import QtQuick 2.15
import QtQuick.Controls 2.15
import "../styles"

Rectangle {
    id: root
    property Item target: null
    visible: target && target.activeFocus
    anchors.fill: target ? target : parent
    color: "transparent"
    border.color: AppStyle.colors.borderFocus
    border.width: 2
    radius: AppStyle.radius.md
}
