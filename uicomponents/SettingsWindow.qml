import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCore
import fscstyle
import fsCleaner


Item {
    id: root
    property int folderIndex: -1

    StackLayout.onIsCurrentItemChanged: {
        if (StackLayout.isCurrentItem){
            folderSettingsListModel.refreshFolders()
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            spacing: 20
            width: scrollView.availableWidth

            ColumnLayout {
                Layout.margins: 10
                spacing: 5
                Layout.fillHeight: true
                Label {
                    text: "General Settings"
                    typography: "headline-md"
                }
                Label {
                    text: "Manage how the file system scanner behaves across your machine."
                    typography: "body-sm"
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.margins: 10

                RowLayout {
                    spacing: 10
                    anchors.fill: parent
                    ColumnLayout {
                        spacing: 5
                        Layout.minimumWidth: 40
                        clip: true
                        Label {
                            Layout.fillWidth: true
                            text: "Hide mounted directories"
                            typography: "body-md"
                            font.weight: Font.Bold
                            wrapMode: Text.WordWrap
                        }
                        Label {
                            Layout.fillWidth: true
                            text: "Excludes network drives and external storage from routine system scans."
                            typography: "body-sm"
                            wrapMode: Text.WordWrap

                        }
                    }
                    Item {
                        Layout.fillWidth: true
                    }

                    Switch {
                        id: hideMountedDirectoriesSwitch
                    }
                }
            }

            RowLayout {
                Layout.margins: 10
                Layout.alignment: Qt.AlignBaseline | Qt.AlignLeft
                spacing: 10

                Label {
                    text: "Predefined Folders"
                    typography: "title-sm"
                    font.weight: Font.Bold
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    type: "primary"
                    text: "Add Folder"
                    radius: Theme.roundedMd
                    implicitHeight: 30
                    font.weight: Font.Bold
                    icon.name: "add"

                    onClicked: {
                        folderDialog.showFolderSelector()
                    }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.margins: 10
                clip: true
                padding: 1
                borderWidth: 1
                borderColor: Theme.surfaceDim
                elevated: false
                ListView {

                    implicitHeight: contentHeight

                    anchors.fill: parent
                    spacing: 0
                    id: predefinedFolders
                    boundsBehavior: Flickable.StopAtBounds
                    interactive: false
                    model: folderSettingsListModel

                    delegate: FolderSettingsRowDelegate {
                        implicitWidth: predefinedFolders.width
                        implicitHeight: 70
                        checkedChanged: (index, checked) => {
                            folderSettingsListModel.setEnabled(index, checked)
                        }

                        deleteClicked: (index) => {
                            root.folderIndex = index;
                            confirmDeleteDialog.showQuestion();
                        }
                    }
                }
            }


            Item {
                Layout.fillHeight: true
            }


            UIHelper {
                id: uiHelper
            }

            Settings {
                id: settings
                property alias ignoreMounted: hideMountedDirectoriesSwitch.checked
            }

            MessageBox {
                id: confirmDeleteDialog
                title: "Delete Folder"
                text: "Are you sure you want to delete this folder?"
                buttons: MessageBox.No | MessageBox.Yes
                onAccepted: {
                    if (root.folderIndex >= 0) {
                        folderSettingsListModel.remove(root.folderIndex)
                        root.folderIndex = -1;
                    }
                }
                onRejected: {
                    root.folderIndex = -1;
                }
            }

            MessageBox {
                id: folderNameDialog
                title: "Add Folder"
                text: "Enter the name of the folder you want to add."
                property string folderPath: ""
                buttons: MessageBox.Cancel | MessageBox.Ok
                onTextInputAccepted: (inputText) => {
                    folderSettingsListModel.addFolder(
                        inputText,
                        folderPath,
                        "folder-home"
                    )
                }
            }

            FileSelector {
                id: folderDialog
                folder: uiHelper.homeDirectory
                caption: "Select Folder"
                onAccepted: (path) => {
                    folderNameDialog.folderPath = path
                    folderNameDialog.textPlaceholder = uiHelper.getFolderName(path)
                    folderNameDialog.showTextInput()
                }
            }

            FolderSettingsListModel {
                id: folderSettingsListModel
            }
        }

    }
}


