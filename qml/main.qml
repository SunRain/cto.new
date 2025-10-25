import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 1024
    height: 768
    visible: true
    title: "GamePerf Manager"

    StackView {
        id: stack
        anchors.fill: parent

        initialItem: application.firstRun ? firstRunWizardComponent : dashboardComponent
    }

    Component {
        id: firstRunWizardComponent
        FirstRunWizard {
            onCompleted: {
                application.completeFirstRun()
                stack.replace(dashboardComponent)
            }
        }
    }

    Component {
        id: dashboardComponent
        ColumnLayout {
            anchors.fill: parent
            spacing: 16
            padding: 24

            Label {
                text: "GamePerf Manager"
                font.pixelSize: 32
            }

            RowLayout {
                spacing: 16
                Button {
                    text: "Check daemon status"
                    onClicked: daemonStatus.text = application.checkDaemonStatus() ? "Daemon is running" : "Daemon is not running"
                }
                Label {
                    id: daemonStatus
                    text: application.daemonAvailable ? "Daemon detected" : "Daemon not detected"
                }
            }
        }
    }
}
