import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

Item {
    id: wizard
    anchors.fill: parent
    signal completed()

    property var steamPaths: application.steamLibraryPaths()
    property string legendaryPath: application.legendaryPath()
    property var winePrefixes: application.winePrefixes()

    function toLocalPath(value) {
        if (!value)
            return ""
        return value.startsWith("file://") ? decodeURIComponent(value.replace("file://", "")) : value
    }

    ListModel {
        id: steamPathsModel
    }

    ListModel {
        id: winePrefixesModel
    }

    Component.onCompleted: {
        steamPathsModel.clear()
        for (var i = 0; i < steamPaths.length; ++i) {
            steamPathsModel.append({ path: steamPaths[i] })
        }

        winePrefixesModel.clear()
        for (var j = 0; j < winePrefixes.length; ++j) {
            winePrefixesModel.append({ path: winePrefixes[j] })
        }
    }

    StackView {
        id: wizardStack
        anchors.fill: parent
        initialItem: welcomePage
    }

    Component {
        id: welcomePage
        Page {
            title: qsTr("Welcome")
            ColumnLayout {
                anchors.centerIn: parent
                width: Math.min(parent.width * 0.6, 640)
                spacing: 24

                Label {
                    text: qsTr("Welcome to GamePerf Manager")
                    font.pixelSize: 32
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: qsTr("This wizard will walk you through the initial configuration of your game libraries and performance services.")
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("You will be asked to:\n• Register Steam library folders\n• Provide Legendary CLI location (optional)\n• Specify Wine prefixes (optional)\n• Verify the performance daemon")
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 18
                    Button {
                        text: qsTr("Get started")
                        onClicked: wizardStack.push(steamSetupPage)
                    }
                }
            }
        }
    }

    Component {
        id: steamSetupPage
        Page {
            title: qsTr("Steam libraries")
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16

                Label {
                    text: qsTr("Steam Library Paths")
                    font.pixelSize: 26
                }

                Label {
                    text: qsTr("Add the Steam library folders that contain your games. These will be scanned for titles and metadata.")
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: 12
                    Button {
                        text: qsTr("Auto-detect")
                        onClicked: {
                            const detected = sourceManager.autoDetectSteamLibraries()
                            if (detected && detected.length > 0) {
                                for (let i = 0; i < detected.length; ++i) {
                                    if (wizard.steamPaths.indexOf(detected[i]) === -1) {
                                        wizard.steamPaths.push(detected[i])
                                        steamPathsModel.append({ path: detected[i] })
                                    }
                                }
                            }
                        }
                    }
                    Button {
                        text: qsTr("Clear")
                        enabled: steamPathsModel.count > 0
                        onClicked: {
                            wizard.steamPaths = []
                            steamPathsModel.clear()
                        }
                    }
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: steamPathsModel
                    delegate: ItemDelegate {
                        width: ListView.view.width
                        text: model.path
                        contentItem: RowLayout {
                            width: parent.width
                            spacing: 8
                            Label {
                                Layout.fillWidth: true
                                text: model.path
                                elide: Text.ElideMiddle
                            }
                            ToolButton {
                                icon.name: "edit-delete"
                                onClicked: {
                                    const idx = index
                                    wizard.steamPaths.splice(idx, 1)
                                    steamPathsModel.remove(idx)
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    spacing: 12
                    Layout.fillWidth: true
                    TextField {
                        id: steamPathField
                        Layout.fillWidth: true
                        placeholderText: qsTr("/path/to/steam/library")
                    }
                    Button {
                        text: qsTr("Browse")
                        onClicked: steamFolderDialog.open()
                    }
                    Button {
                        text: qsTr("Add")
                        enabled: steamPathField.text.length > 0
                        onClicked: {
                            const path = steamPathField.text
                            if (sourceManager.validateSteamPath(path)) {
                                if (wizard.steamPaths.indexOf(path) === -1) {
                                    wizard.steamPaths.push(path)
                                    steamPathsModel.append({ path: path })
                                }
                                steamPathField.text = ""
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.topMargin: 12
                    spacing: 12
                    Button {
                        text: qsTr("Back")
                        onClicked: wizardStack.pop()
                    }
                    Item { Layout.fillWidth: true }
                    Button {
                        text: qsTr("Next")
                        onClicked: wizardStack.push(legendarySetupPage)
                    }
                }
            }

            FileDialog {
                id: steamFolderDialog
                title: qsTr("Select Steam Library Folder")
                fileMode: FileDialog.Directory
                onAccepted: {
                    const folder = selectedFiles.length > 0 ? toLocalPath(selectedFiles[0]) : ""
                    if (folder) {
                        steamPathField.text = folder
                    }
                }
            }
        }
    }

    Component {
        id: legendarySetupPage
        Page {
            title: qsTr("Legendary CLI")
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16

                Label {
                    text: qsTr("Legendary CLI (optional)")
                    font.pixelSize: 26
                }

                Label {
                    text: qsTr("Legendary is an open-source Epic Games Store client. Provide the path to the `legendary` executable if you use it.")
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                RowLayout {
                    spacing: 12
                    Layout.fillWidth: true
                    TextField {
                        id: legendaryPathField
                        Layout.fillWidth: true
                        text: wizard.legendaryPath
                        placeholderText: qsTr("/usr/bin/legendary")
                    }
                    Button {
                        text: qsTr("Browse")
                        onClicked: legendaryFileDialog.open()
                    }
                    Button {
                        text: qsTr("Verify")
                        enabled: legendaryPathField.text.length > 0
                        onClicked: {
                            const valid = sourceManager.validateLegendaryInstallation(legendaryPathField.text)
                            verificationLabel.text = valid
                                    ? qsTr("Legendary executable verified")
                                    : qsTr("Legendary executable not found or not executable")
                            verificationLabel.color = valid ? "green" : "tomato"
                            if (valid) {
                                wizard.legendaryPath = legendaryPathField.text
                            }
                        }
                    }
                }

                Label {
                    id: verificationLabel
                    text: wizard.legendaryPath.length > 0
                            ? qsTr("Legendary executable saved")
                            : qsTr("Legendary CLI not configured (optional)")
                    color: wizard.legendaryPath.length > 0 ? "green" : "gray"
                    wrapMode: Text.WordWrap
                }

                Item { Layout.fillHeight: true }

                RowLayout {
                    spacing: 12
                    Button {
                        text: qsTr("Back")
                        onClicked: wizardStack.pop()
                    }
                    Item { Layout.fillWidth: true }
                    Button {
                        text: qsTr("Next")
                        onClicked: {
                            wizard.legendaryPath = legendaryPathField.text
                            wizardStack.push(winePrefixSetupPage)
                        }
                    }
                }
            }

            FileDialog {
                id: legendaryFileDialog
                title: qsTr("Select Legendary Executable")
                fileMode: FileDialog.OpenFile
                onAccepted: {
                    const file = selectedFiles.length > 0 ? toLocalPath(selectedFiles[0]) : ""
                    if (file) {
                        legendaryPathField.text = file
                    }
                }
            }
        }
    }

    Component {
        id: winePrefixSetupPage
        Page {
            title: qsTr("Wine prefixes")
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16

                Label {
                    text: qsTr("Wine Prefixes (optional)")
                    font.pixelSize: 26
                }

                Label {
                    text: qsTr("Add Wine prefixes to integrate Proton or custom Wine installations with GamePerf Manager.")
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: winePrefixesModel
                    delegate: ItemDelegate {
                        width: ListView.view.width
                        contentItem: RowLayout {
                            width: parent.width
                            spacing: 8
                            Label {
                                Layout.fillWidth: true
                                text: model.path
                                elide: Text.ElideMiddle
                            }
                            ToolButton {
                                icon.name: "edit-delete"
                                onClicked: {
                                    const idx = index
                                    wizard.winePrefixes.splice(idx, 1)
                                    winePrefixesModel.remove(idx)
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    spacing: 12
                    Layout.fillWidth: true
                    TextField {
                        id: winePathField
                        Layout.fillWidth: true
                        placeholderText: qsTr("~/.wine")
                    }
                    Button {
                        text: qsTr("Browse")
                        onClicked: wineFolderDialog.open()
                    }
                    Button {
                        text: qsTr("Add")
                        enabled: winePathField.text.length > 0
                        onClicked: {
                            const path = winePathField.text
                            if (sourceManager.validateWinePrefix(path)) {
                                wizard.winePrefixes.push(path)
                                winePrefixesModel.append({ path: path })
                                winePathField.text = ""
                            }
                        }
                    }
                }

                RowLayout {
                    spacing: 12
                    Button {
                        text: qsTr("Back")
                        onClicked: wizardStack.pop()
                    }
                    Item { Layout.fillWidth: true }
                    Button {
                        text: qsTr("Next")
                        onClicked: wizardStack.push(daemonCheckPage)
                    }
                }
            }

            FileDialog {
                id: wineFolderDialog
                title: qsTr("Select Wine Prefix")
                fileMode: FileDialog.Directory
                onAccepted: {
                    const folder = selectedFiles.length > 0 ? toLocalPath(selectedFiles[0]) : ""
                    if (folder) {
                        winePathField.text = folder
                    }
                }
            }
        }
    }

    Component {
        id: daemonCheckPage
        Page {
            title: qsTr("Daemon verification")
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16

                Label {
                    text: qsTr("Performance daemon")
                    font.pixelSize: 26
                }

                Label {
                    text: qsTr("The GamePerf daemon provides metrics and hardware control. Verify it is installed and running to unlock all features.")
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Check daemon status")
                    onClicked: {
                        const running = application.checkDaemonStatus()
                        daemonStatus.text = running
                                ? qsTr("✓ Daemon is running and accessible")
                                : qsTr("⚠ Daemon not running. Run install script or enable the service.")
                        daemonStatus.color = running ? "green" : "tomato"
                    }
                }

                Label {
                    id: daemonStatus
                    text: application.daemonAvailable
                        ? qsTr("✓ Daemon detected")
                        : qsTr("Daemon status unknown")
                    color: application.daemonAvailable ? "green" : "gray"
                    wrapMode: Text.WordWrap
                }

                Label {
                    text: qsTr("Installation tip: Run `scripts/install-performance-daemon.sh` with administrator permissions to install the service after building the package.")
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    color: "#666"
                }

                Item { Layout.fillHeight: true }

                RowLayout {
                    spacing: 12
                    Button {
                        text: qsTr("Back")
                        onClicked: wizardStack.pop()
                    }
                    Item { Layout.fillWidth: true }
                    Button {
                        text: qsTr("Finish setup")
                        enabled: wizard.steamPaths.length > 0
                        onClicked: {
                            application.saveSteamLibraries(wizard.steamPaths)
                            application.saveLegendaryPath(wizard.legendaryPath)
                            application.saveWinePrefixes(wizard.winePrefixes)
                            wizard.completed()
                        }
                    }
                }
            }
        }
    }
}
