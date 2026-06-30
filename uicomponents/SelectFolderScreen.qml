import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import fscstyle
import fsCleaner

Item {
    id: root

    property var stackView: StackView.view
    required property var folderSettingsListModel

    ScrollView {

        id: scrollView
        anchors.fill: parent

        contentWidth: availableWidth
        // contentHeight: availableHeight
        clip: true
        ColumnLayout {
            // anchors.fill: parent
            spacing: 10
            clip: true
            width: scrollView.availableWidth

            Item {
                Layout.fillWidth: true
                implicitHeight: 10
            }

            Label {
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignHCenter
                Layout.alignment: Qt.AlignVCenter
                text: "Select folder to scan"
                typography: "headline-md"
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Qt.AlignHCenter
                text: "Choose a directory to identify clutter and redundant files."
                typography: "body-sm"
            }
            Item {
                Layout.fillWidth: true
                implicitHeight: 10
            }

            GridView {
                Layout.minimumWidth: 640
                // Layout.minimumHeight: 100
                Layout.preferredHeight: contentHeight
                Layout.alignment: Qt.AlignHCenter
                boundsBehavior: Flickable.StopAtBounds
                interactive: false


                model: folderSettingsListModel
                cellWidth: 320
                cellHeight: 90
                delegate: Card {
                    id: cardDelegate
                    required property var modelData
                    required property int index
                    implicitWidth: 300

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent
                        clip: true

                        Item {
                            implicitWidth: 24
                            implicitHeight: 24
                            Image {
                                anchors.fill: parent
                                source: "image://theme/" + cardDelegate.modelData.iconName
                                smooth: true
                            }
                        }
                        ColumnLayout {
                            id: labelsColumn
                            spacing: 5

                            Label {
                                text: cardDelegate.modelData.name
                                typography: "body-md"
                                font.weight: Font.Bold
                            }
                            Label {
                                text: cardDelegate.modelData.path
                                typography: "mono-data"
                                // width: labelsColumn.width
                                // wrapMode: Text.WrapAnywhere
                                elide: Text.ElideMiddle
                                clip: true

                            }
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                    }

                    TapHandler {
                        onTapped: {
                            root.stackView.push(scanScreen, {"path": modelData.path})
                        }
                    }
                }
            }

            Button {
                Layout.alignment: Qt.AlignHCenter
                icon.name: "add-circle"
                type: "outlined"
                text: "Chose Another Folder"
                // implicitWidth: 300
                implicitHeight: 60
                padding: 10
                radius: Theme.roundedDefault
                onClicked: folderDialog.showFolderSelector()
            }

        }
    }
    // FolderSettingsListModel {
    //     id: folderSettingsListModel
    //     showDisabled: false
    // }

    UIHelper {
        id: uiHelper
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
}
