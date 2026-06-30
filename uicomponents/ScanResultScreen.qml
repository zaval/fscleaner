import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

import fscstyle

Item {
    id: root

    property var stackView: StackView.view
    property var dbPath: ""

    property alias fsModel: fileSystemModel

    StackView.onActivated: {
        fileSystemModel.initialize(root.dbPath)
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            // anchors.fill: parent

            RowLayout {
                spacing: 10
                Layout.fillWidth: true
                ToolButton {
                    icon.name: "arrow-back"
                    onClicked: {
                        root.stackView.popToItem(selectFolderScreen)
                    }
                    background: Rectangle {
                        color: "transparent"
                    }
                    // type: "outlined"
                }
                Label {
                    text: "Scan Results"
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
                        id: contextMenu
                        MenuItem {
                            text: treeDelegate.hasChildren ? qsTr("Open in %1").arg("Explorer") : qsTr("Open in default application")
                            onTriggered: {
                                Qt.openUrlExternally("file://" + treeDelegate.model.filePath);
                            }
                        }

                        MenuItem {
                            text: qsTr("Delete")
                            onTriggered: {
                                // console.log(treeView.index(treeDelegate.row, 0))
                                deleteConfirmation.itemId = treeDelegate.model.itemId
                                deleteConfirmation.path = treeDelegate.model.filePath
                                deleteConfirmation.modelIndex = treeView.index(treeDelegate.row, 0)
                                contextMenu.close()
                                deleteConfirmation.showQuestion()
                            }
                        }
                    }

                    TapHandler {
                        acceptedButtons: Qt.LeftButton
                        onSingleTapped: (eventPoint, button) => {
                            switch (button) {
                                case Qt.LeftButton:
                                    // console.log("Left button pressed")
                                    if (!treeDelegate.hasChildren){
                                        fileProperties.filePath = treeDelegate.model.filePath
                                        fileProperties.itemId = treeDelegate.model.itemId
                                    } else {
                                        fileProperties.filePath = ""
                                        fileProperties.itemId = -1

                                    }
                                    break
                                case Qt.RightButton:
                                    // console.log("Right button pressed")
                                    break
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            implicitWidth: 1
            Layout.fillHeight: true
            color: Theme.colorOutline
        }
        FileProperties {
            id: fileProperties
            visible: fileProperties.filePath !== ""
            filePath: ""
            Layout.preferredWidth: 350
            Layout.fillHeight: true
            onDeleteRequested: (itemId, filePath) => {
                // console.log(Object.keys(treeView.selectionModel.currentIndex))
                deleteConfirmation.itemId = itemId
                deleteConfirmation.path = filePath
                deleteConfirmation.modelIndex = treeView.selectionModel.currentIndex
                deleteConfirmation.showQuestion()
            }
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        width: 50
        height: 50
        visible: fileSystemModel.isLoading
    }

    DeleteScanResultConfirmation {
        id: deleteConfirmation
        model: fileSystemModel
    }

    FSModel {
        id: fileSystemModel
        modelLoader: SqliteFSModelLoader {}
    }

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

}