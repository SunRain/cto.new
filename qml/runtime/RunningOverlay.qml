import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: overlayWindow
    visible: true
    width: 1200
    height: 800
    title: qsTr("Running Games")
    color: "#1a1a1a"
    focus: true

    property var runningManager: null
    property var gamescopeLauncher: null

    Keys.onPressed: function(event) {
        if (event.key === Qt.Key_Guide) {
            overlayWindow.visible = !overlayWindow.visible
            event.accepted = true
        }
    }

    Shortcut {
        sequences: [StandardKey.Quit, "Ctrl+Shift+G"]
        onActivated: overlayWindow.visible = !overlayWindow.visible
    }

    Rectangle {
        anchors.fill: parent
        color: "#1a1a1a"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            Label {
                text: qsTr("Running Games Manager")
                font.pixelSize: 32
                font.bold: true
                color: "#ffffff"
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Button {
                    text: qsTr("Toggle Overlay")
                    Layout.preferredWidth: 160
                    onClicked: {
                        if (gamescopeLauncher) {
                            gamescopeLauncher.toggleOverlay("")
                        }
                    }
                }

                Button {
                    text: qsTr("Exit to Shell")
                    Layout.preferredWidth: 160
                    enabled: runningManager && runningManager.games.length > 0
                    onClicked: {
                        if (gamescopeLauncher) {
                            gamescopeLauncher.exitToShell("")
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }
            }

            Row {
                spacing: 15
                Layout.fillWidth: true

                Rectangle {
                    width: 80
                    height: 35
                    color: "#2a2a2a"
                    radius: 5
                    Label {
                        anchors.centerIn: parent
                        text: qsTr("CPU")
                        font.pixelSize: 14
                        color: "#aaaaaa"
                    }
                }

                Rectangle {
                    width: 80
                    height: 35
                    color: "#2a2a2a"
                    radius: 5
                    Label {
                        anchors.centerIn: parent
                        text: qsTr("GPU")
                        font.pixelSize: 14
                        color: "#aaaaaa"
                    }
                }

                Rectangle {
                    width: 80
                    height: 35
                    color: "#2a2a2a"
                    radius: 5
                    Label {
                        anchors.centerIn: parent
                        text: qsTr("RAM")
                        font.pixelSize: 14
                        color: "#aaaaaa"
                    }
                }

                Rectangle {
                    width: 80
                    height: 35
                    color: "#2a2a2a"
                    radius: 5
                    Label {
                        anchors.centerIn: parent
                        text: qsTr("TEMP")
                        font.pixelSize: 14
                        color: "#aaaaaa"
                    }
                }

                Rectangle {
                    width: 80
                    height: 35
                    color: "#2a2a2a"
                    radius: 5
                    Label {
                        anchors.centerIn: parent
                        text: qsTr("FPS")
                        font.pixelSize: 14
                        color: "#aaaaaa"
                    }
                }
            }

            Rectangle {
                id: alertsContainer
                Layout.fillWidth: true
                Layout.preferredHeight: alertsRepeater.count > 0 ? alertsRepeater.count * 60 + 20 : 0
                color: "transparent"
                visible: alertsRepeater.count > 0

                Column {
                    id: alertsColumn
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    Repeater {
                        id: alertsRepeater
                        model: runningManager ? runningManager.alerts : []

                        Rectangle {
                            width: alertsColumn.width
                            height: 50
                            radius: 8
                            color: modelData.severity === "critical" ? "#663333" : "#665533"
                            border.width: 2
                            border.color: modelData.severity === "critical" ? "#ff4444" : "#ffaa44"

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 15

                                Label {
                                    text: modelData.severity === "critical" ? "⚠️" : "⚡"
                                    font.pixelSize: 24
                                }

                                Label {
                                    text: modelData.message
                                    font.pixelSize: 14
                                    color: "#ffffff"
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }

                                Label {
                                    text: modelData.type
                                    font.pixelSize: 12
                                    color: "#aaaaaa"
                                }
                            }
                        }
                    }
                }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                ListView {
                    id: gamesListView
                    spacing: 15
                    model: runningManager ? runningManager.games : []

                    delegate: Rectangle {
                        width: gamesListView.width
                        height: 150
                        color: "#2a2a2a"
                        radius: 10
                        border.width: modelData.state === "running" ? 2 : 1
                        border.color: modelData.state === "running" ? "#4a90e2" : "#444444"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 10

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 15

                                Label {
                                    text: modelData.displayName
                                    font.pixelSize: 20
                                    font.bold: true
                                    color: "#ffffff"
                                    Layout.fillWidth: true
                                }

                                Label {
                                    text: modelData.state === "running" ? qsTr("Running") : qsTr("Suspended")
                                    font.pixelSize: 14
                                    color: modelData.state === "running" ? "#66ff66" : "#ffaa44"
                                }

                                Label {
                                    text: qsTr("PID: %1").arg(modelData.pid)
                                    font.pixelSize: 12
                                    color: "#888888"
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 20

                                MetricDisplay {
                                    label: qsTr("CPU")
                                    value: modelData.metrics.cpuPercent.toFixed(1) + "%"
                                    color: modelData.metrics.cpuPercent > 90 ? "#ff4444" : "#4a90e2"
                                }

                                MetricDisplay {
                                    label: qsTr("GPU")
                                    value: modelData.metrics.gpuPercent.toFixed(1) + "%"
                                    color: modelData.metrics.gpuPercent > 90 ? "#ff4444" : "#4a90e2"
                                }

                                MetricDisplay {
                                    label: qsTr("RAM")
                                    value: modelData.metrics.ramMb.toFixed(0) + " MB"
                                    color: modelData.metrics.ramPercent > 90 ? "#ff4444" : "#4a90e2"
                                }

                                MetricDisplay {
                                    label: qsTr("Temp")
                                    value: modelData.metrics.temperatureC.toFixed(1) + "°C"
                                    color: modelData.metrics.temperatureC > 85 ? "#ff4444" : "#4a90e2"
                                }

                                MetricDisplay {
                                    label: qsTr("Power")
                                    value: modelData.metrics.powerWatts.toFixed(1) + " W"
                                    color: "#4a90e2"
                                }

                                MetricDisplay {
                                    label: qsTr("FPS")
                                    value: Math.round(modelData.metrics.fps).toString()
                                    color: modelData.metrics.fps < 30 ? "#ffaa44" : "#66ff66"
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 10

                                Button {
                                    text: qsTr("Focus")
                                    enabled: modelData.state === "running"
                                    onClicked: runningManager.focusGame(modelData.titleId)
                                }

                                Button {
                                    text: modelData.state === "running" ? qsTr("Suspend") : qsTr("Resume")
                                    enabled: modelData.supportsSuspend
                                    ToolTip.visible: !modelData.supportsSuspend && hovered
                                    ToolTip.text: modelData.suspendUnsupportedReason || qsTr("Suspend not supported")
                                    onClicked: {
                                        if (modelData.state === "running") {
                                            runningManager.suspendGame(modelData.titleId)
                                        } else {
                                            runningManager.resumeGame(modelData.titleId)
                                        }
                                    }
                                }

                                Button {
                                    text: qsTr("Force Quit")
                                    highlighted: true
                                    onClicked: forceQuitDialog.showForGame(modelData)
                                }

                                Item {
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                }
            }

            Label {
                text: runningManager && runningManager.games.length === 0 ? qsTr("No running games") : ""
                font.pixelSize: 18
                color: "#888888"
                Layout.alignment: Qt.AlignHCenter
                visible: runningManager && runningManager.games.length === 0
            }
        }
    }

    Dialog {
        id: forceQuitDialog
        title: qsTr("Force Quit Game")
        modal: true
        anchors.centerIn: parent
        width: 400

        property var gameData: null

        function showForGame(data) {
            gameData = data
            visible = true
        }

        contentItem: ColumnLayout {
            spacing: 20

            Label {
                text: qsTr("Are you sure you want to force quit \"%1\"?").arg(forceQuitDialog.gameData ? forceQuitDialog.gameData.displayName : "")
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Any unsaved progress will be lost.")
                font.italic: true
                color: "#ffaa44"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Button {
                    text: qsTr("Cancel")
                    Layout.fillWidth: true
                    onClicked: forceQuitDialog.close()
                }

                Button {
                    text: qsTr("Force Quit")
                    highlighted: true
                    Layout.fillWidth: true
                    onClicked: {
                        if (forceQuitDialog.gameData && runningManager) {
                            runningManager.forceQuit(forceQuitDialog.gameData.titleId)
                        }
                        forceQuitDialog.close()
                    }
                }
            }
        }
    }

    component MetricDisplay: ColumnLayout {
        property string label: ""
        property string value: ""
        property color color: "#4a90e2"

        spacing: 5

        Label {
            text: label
            font.pixelSize: 11
            color: "#888888"
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: value
            font.pixelSize: 14
            font.bold: true
            color: parent.color
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
