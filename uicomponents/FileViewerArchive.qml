import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import fscstyle
import fsCleaner

ApplicationWindow {
    id: root

    onClosing: {
        destroy(0)
    }

    Component.onCompleted: {
        fileSystemModel.initialize(filePath)
    }

    required property string filePath
    property string fileName: uiHelper.getFileName(filePath)

    height: 600
    modality: Qt.ApplicationModal
    title: qsTr("View ") + fileName
    width: 800

    Rectangle {
        anchors.fill: parent
        color: Theme.surfaceContainerLowest
    }

    ColumnLayout {
        id: rootLayout
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5
        states: State {
            name: "searchIsActive"
            when: searchField.activeFocus
            PropertyChanges {
                searchField {
                    implicitWidth: 500
                }
            }
        }
        transitions: Transition {
            PropertyAnimation {
                target: searchField
                property: "implicitWidth"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }

        RowLayout {
            spacing: 10
            Layout.fillWidth: true

            Label {
                text: root.fileName
                typography: "headline-md"
            }

            Item {
                id: searchSeparator
                Layout.fillWidth: true
            }

            SearchTextField {
                id: searchField
                implicitHeight: 30
                rightPadding: padding + 20
                Layout.rightMargin: 10
                implicitWidth: 200
                onEdited: (text) => {
                    fileSystemModel.filterByName = text
                }
            }
        }



        HorizontalHeaderView {
            id: header
            syncView: treeView
            clip: true
            movableColumns: false
            Layout.fillWidth: true
        }

        TreeView {
            id: treeView
            model: fileSystemModel
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            columnWidthProvider: function(column) {
                if (!isColumnLoaded(column))
                    return -1
                if (column === 0) {
                    let usedWidth = 0

                    for (let c = 1; c < columns; ++c)
                        usedWidth += implicitColumnWidth(c)

                    return Math.max(100, width - usedWidth)
                }

                return implicitColumnWidth(column)
            }
            selectionMode: TreeView.SelectRows
            selectionModel: ItemSelectionModel {
                // onCurrentChanged: (current, previous) => {
                //     console.log(current)
                // }
            }
            delegate: TreeViewDelegate {

                id: treeDelegate
                ContextMenu.menu: Menu {
                    MenuItem {
                        text: "Extract to"
                        onClicked: {
                            fileExtractor.archiveFilePath = treeDelegate.model.filePath
                            fileExtractor.showFolderSelector()

                        }
                    }
                }
            }
        }
    }

    UIHelper {
        id: uiHelper
    }

    FSModel {
        id: fileSystemModel
        modelLoader: ArchiveFSModelLoader {}
    }

    FileSelector {
        id: fileExtractor
        property string archiveFilePath
        onAccepted: (folder) => {
            console.log(folder)
            fileSystemModel.modelLoader.extract(archiveFilePath, folder)
        }
    }

}