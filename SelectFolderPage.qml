import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import fsCleaner

ColumnLayout {
    id: root
    spacing: 10

    property StackView stackView: null
    property var fileSystemModel: null
    property Component filesTreePage: null
    property var folders: []

    function loadFolders() {
        var tmpFolders = AppSettings.folders.map((folder) => JSON.parse(folder));
        folders = tmpFolders.filter((folder) => folder.enabled === true)
    }

    Connections {
        target: AppSettings
        function onFoldersChanged() {
            loadFolders()
        }
    }

    Component.onCompleted: {
        loadFolders()
    }
    
    UIHelper {
        id: hlp
    }

    property string currentFolder : ""

    Item {
        Layout.preferredHeight: 200
        Layout.fillHeight: true

    }

    Label {
        id: selectLabel
        text: qsTr("Select folder to scan")
        Layout.alignment: Qt.AlignCenter
        font.pointSize: 20
        // font: Qt.font({ family: "Helvetica", pointSize: 20, weight: Font.Normal})
        states: State {
            name: "hidden"
            when: (root.state != "init")
            PropertyChanges {selectLabel.visible: false}
        }
    }
    RowLayout {
        id: loadingLayout
        visible: false
        // spacing: 10
        Layout.alignment: Qt.AlignCenter
        Layout.maximumWidth: 300
        implicitWidth: 300
        Label {
            id: logLabel
            Layout.fillWidth: true
            text: processRunner.logMessageLine
            // font: Qt.font({ family: "Helvetica", pointSize: 12, weight: Font.Normal})
        }
        Button{
            id: stopProcessButton
            // visible: processRunner.isRunning
            text: "Stop"
            onClicked:{
                processRunner.stopProcess()
                root.state = "init"
            }
        }

        states: State {
            name: "visible"
            when: (root.state != "init")
            PropertyChanges {loadingLayout.visible: true}
        }
    }
    GridLayout {
        columns: Math.max(1, Math.floor(root.width / 350))
        columnSpacing : 10
        rowSpacing: 10
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true


        Repeater {
            model: folders
            delegate: SelectFolderButton {
                required property string name
                required property string path
                required property string iconName
                id: delegateButton
                icon.name: iconName
                icon.width: 32
                icon.height: 32
                text: qsTr(name)
                description: path
                Layout.minimumWidth: 300
                enabled: !processRunner.isRunning
                visible: true
                onClicked: {
                    processRunner.processFolder(delegateButton.description, AppSettings.ignoreMounted)
                    root.state = name + "Loading"
                }
                states: State {
                    name: "hidden"
                    when: (root.state != (delegateButton.name + "Loading") && root.state != "init")
                    PropertyChanges {delegateButton.visible: false}
                }
            }
        }

        SelectFolderButton {
            id: customButton
            icon.name: "folder"
            icon.width: 32
            icon.height: 32
            text: qsTr("Other Folder")
            description: root.currentFolder
            Layout.minimumWidth: 300
            enabled: !processRunner.isRunning
            visible: true

            onClicked: {
                folderDialog.selectedFolder = "file://" + StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
                folderDialog.open()
            }

            states: State {
                name: "hidden"
                when: (root.state != "customLoading" && root.state != "init")
                PropertyChanges {customButton.visible: false}
            }
        }

        Item {
            id: loadingSpacer
            Layout.preferredHeight: 200
            Layout.fillHeight: true
            visible: false

            states: State {
                name: "visible"
                when: (root.state != "init")
                PropertyChanges {loadingSpacer.visible: true}
            }
        }


    }

    Item {
        Layout.preferredHeight: 200
        Layout.fillHeight: true
    }

    FolderDialog {
        id: folderDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        // selectedFolder: "file://" + folderField.text
        onAccepted: {
            // convert selectedFolder url to normal path, get rid of file://
            var selectedFolder = folderDialog.selectedFolder.toString()
            selectedFolder = selectedFolder.replace("file://", "")
            root.currentFolder = selectedFolder
            processRunner.processFolder(selectedFolder, AppSettings.ignoreMounted)
            root.state = "customLoading"
        }
    }

    ProcessRunner {
        id: processRunner
        onProcessFinished: {
           root.state = "init"
           // fileSystemModel.initialize(processRunner.dbPath)
           // stack.push(filesTreePage)
            if (root.fileSystemModel) {
                root.fileSystemModel.initialize(processRunner.dbPath)
            }
            if (root.stackView && root.filesTreePage) {
                root.stackView.push(root.filesTreePage)
            }

        }
    }

    state: "init"
}


