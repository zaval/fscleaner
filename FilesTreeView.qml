import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs


ColumnLayout {
    anchors.margins: 10
    spacing: 5
    property var fsModel: null
    required property var fileMetaData
    id: root

    HorizontalHeaderView {
        id: header
        Layout.leftMargin: 10
        implicitHeight: 25
        syncView: treeView
        clip: true
        movableColumns: false
        Layout.fillWidth: true
    }

    TreeView {
        id: treeView
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.leftMargin: 10
        selectionMode: TreeView.SelectRows
        selectionModel: ItemSelectionModel {}
        model: root.fsModel

        delegate: FoldersTreeViewDelegate {
            id: viewDelegate
            implicitHeight: 25

            implicitWidth: model.column === 0 ? treeView.width - 100: leftMargin + implicitContentWidth  + rightPadding + rightMargin


            TapHandler {
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onSingleTapped: (eventPoint, button) => {
                    switch (button) {
                        case Qt.LeftButton:
                            // treeView.selectionModel.setCurrentIndex(treeView.index(viewDelegate.row, 0), ItemSelectionModel.ClearAndSelect)
                            if (!viewDelegate.hasChildren){
                                root.fileMetaData.path = viewDelegate.model.filePath
                                // FileMetaData.size = viewDelegate.model.formattedSize
                            } else {
                                root.fileMetaData.path = ""
                            }

                            break;
                        case Qt.RightButton:
                            // if (viewDelegate.hasChildren)
                            contextMenu.open();
                            break;
                    }
                }
            }

            FilesTreeMenu {
                id: contextMenu
                viewDelegate: viewDelegate
                confirmDialog: confirmDlg
            }
            
        }

    }


    UIHelper {
        id: uiHelper
    }

    Dialog {
        id: confirmDlg
        anchors.centerIn: parent
        title: "Delete Folder"
        property string filePath: ""
        standardButtons: Dialog.Yes | Dialog.No
        implicitWidth: confirmDialogLabel.width + 50
        modal: true
        contentItem: Item {
            anchors.margins: 15
            Label {
                id: confirmDialogLabel
                text: qsTr("Are you sure you want to delete\n" + confirmDlg.filePath + "?")
                anchors.centerIn: parent
            }
        }

        onAccepted: {
            // uiHelper.remove(confirmDlg.filePath)
            // root.fsModel.deleteFile(confirmDlg.filePath)
            let modelIndex = treeView.selectionModel.currentIndex
            if (modelIndex.valid) {
                root.fsModel.removeRows(modelIndex.row, 1, modelIndex.parent)
            }

            confirmDlg.filePath = ""
        }
    }
}
