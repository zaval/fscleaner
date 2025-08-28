import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Controls.impl

Dialog {
    id: addFolderDialog
    property string folderPath: ""
    property string folderName: ""
    required property var settingsPage

    title: "Add Folder"
    // anchors.centerIn: parent
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    implicitWidth: dialogRowLayout.implicitWidth + 100
    implicitHeight: 150
    contentItem: Item {

        // implicitWidth: 300
        RowLayout {
            id: dialogRowLayout
            anchors.fill: parent
            spacing: 15
            anchors.margins: 10


            IconLabel {
                spacing: 10

                icon.name: "folder"
                icon.width: 32
                icon.height: 32
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true
                spacing: 10
                TextField {
                    id: folderNameField
                    text: addFolderDialog.folderName
                    font.weight: Font.Bold
                    Layout.fillWidth: true

                }
                Label {
                    text: addFolderDialog.folderPath
                    elide: Text.ElideMiddle
                }
            }

        }
    }

    onAccepted: {
        settingsPage.existingFolders.append({name: folderNameField.text, path: addFolderDialog.folderPath, enabled: true, iconName: "folder"});
        addFolderDialog.folderPath = ""
        addFolderDialog.folderName = ""

        settingsPage.saveFolders()
    }
    onRejected: console.log("Cancel clicked")
}