pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.impl
import QtQuick.Dialogs
import QtCore
import fsCleaner

Item {

    id: root
    property ListModel existingFolders: ListModel {}

    UIHelper {
        id: uiHelper
    }


    function saveFolders() {
        let folders = [];
        for (let i = 0; i < existingFolders.count; i++){
            folders.push(JSON.stringify(existingFolders.get(i)))
        }
        AppSettings.folders = folders
        AppSettings.sync()
    }

    Component.onCompleted: {
        const folders = AppSettings.folders;
        if (folders.length === 0) {
        // const folders = [];
        // if (true) {
            console.log("empty list")
            folders.push(JSON.stringify({name: "Home", path: uiHelper.homeDirectory, enabled: true, iconName: "folder-home"}));
            folders.push(JSON.stringify({name: "Documents", path: uiHelper.documentsDirectory, enabled: true, iconName: "folder-documents"}));
            folders.push(JSON.stringify({name: "Downloads", path: uiHelper.downloadsDirectory, enabled: true, iconName: "folder-downloads"}));
            folders.push(JSON.stringify({name: "Cache", path: uiHelper.cacheDirectory, enabled: true, iconName: "folder-cache"}));

            AppSettings.folders = folders;
        }
        // console.log(JSON.stringify(folders))
        for (let folder of folders) {
            // console.log(folder);
            existingFolders.append(JSON.parse(folder));
        }


    }

    ColumnLayout {
        id: columnLayout
        anchors.centerIn: parent
        // anchors.bottom: parent.bottom
        anchors.margins: 40
        spacing: 15


        height: parent.height
        width: Math.min(parent.width / 2, 600)

        Item {
            // Layout.fillHeight: true
            implicitHeight: 100
        }
        Label {
            text: qsTr("General Settings")
            // Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.bold: true
            font.pointSize: 18
        }

        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Hide mounted directories"
            }
            Item {
                Layout.fillWidth: true
            }
            Switch {
                id: hideMountedDirectoriesSwitch
                checked: AppSettings.ignoreMounted
                onCheckedChanged: {
                    AppSettings.ignoreMounted = checked
                }
            }
        }

        ListView {
            id: foldersListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10
            clip: true
            highlight: Rectangle {
                color: palette.highlight;
                radius: 5

            }
            focus: true
            model: root.existingFolders
            header: RowLayout {
                width: parent.width
                height: headerLabel.height + 30
                Label {
                    id: headerLabel
                    text: qsTr("Predefined Folders")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    font.bold: true
                    font.pointSize: 18
                    Layout.topMargin: 10
                    Layout.bottomMargin: 10
                }
                Item {
                    Layout.fillWidth: true
                }
                ToolButton {
                    text: "+"
                    font.weight: Font.Bold
                    font.pointSize: 16
                    onClicked: {
                        folderDialog.open()
                    }
                }
            }
            delegate: Item {
                id: delegateItem
                required property string name
                required property string path
                required property bool enabled
                required property int index
                width: foldersListView.width
                height: delegateRowLayout.height + 5

                // Item { width: foldersListView.width + 15; height: delegateRowLayout.height + 15 }


                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        foldersListView.currentIndex = delegateItem.index
                    }
                }
                RowLayout {
                    anchors.centerIn: parent
                    id: delegateRowLayout

                    spacing: 5

                    // anchors.margins: 20
                    width: foldersListView.width

                    CheckBox {
                        checked: delegateItem.enabled
                        onToggled: {
                            console.log("toggled " + delegateItem.enabled)
                            root.existingFolders.setProperty(delegateItem.index, "enabled", !delegateItem.enabled)
                            root.saveFolders()
                        }
                    }
                    IconLabel {
                        id: iconLabel
                        spacing: 10

                        icon.name: "folder"
                        icon.width: 32
                        icon.height: 32
                    }
                    ColumnLayout {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        spacing: 5
                        Label {
                            id: caption
                            text: delegateItem.name
                            font.weight: Font.Bold
                            elide: Text.ElideRight
                        }

                        Label {
                            id: description
                            text: delegateItem.path
                            elide: Text.ElideRight
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                    ToolButton {
                        icon.name: "trash-empty"
                        icon.color: root.palette.text
                        onClicked: {
                            confirmDeleteDialog.folderIndex = delegateItem.index;
                            confirmDeleteDialog.open()
                        }
                    }
                }

            }
        }
    }

    SettingsAddFolderDialog {
        id: addFolderDialog
        anchors.centerIn: parent
        settingsPage: root
    }

    FolderDialog {
        id: folderDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onAccepted: {
            let selectedFolder = folderDialog.selectedFolder.toString();
            selectedFolder = selectedFolder.replace("file://", "")
            addFolderDialog.folderPath = selectedFolder
            addFolderDialog.folderName = uiHelper.getFolderName(selectedFolder)
            addFolderDialog.open()
        }
    }

    MessageDialog {
        id: confirmDeleteDialog
        property int folderIndex: -1
        title: "Delete Folder"
        text: qsTr("Are you sure you want to delete this folder?")
        buttons: MessageDialog.No | MessageDialog.Yes
        // standardButtons: Dialog.Yes | Dialog.No
        // implicitWidth: confirmDialogLabel.width + 50
        // // implicitHeight: 150
        // anchors.centerIn: parent
        // modal: true
        // contentItem: Item {
        //     anchors.margins: 15
        //     Label {
        //         id: confirmDialogLabel
        //         text: qsTr("Are you sure you want to delete this folder?")
        //         anchors.centerIn: parent
        //     }
        // }

        onAccepted: {
            if (confirmDeleteDialog.folderIndex >= 0) {
                root.existingFolders.remove(confirmDeleteDialog.folderIndex)
                confirmDeleteDialog.folderIndex = -1;
                root.saveFolders()
            }
        }
        onRejected: console.log("Cancel clicked")
    }


}







