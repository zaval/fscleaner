import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


ApplicationWindow {
    id: root
    width: 800
    height: 600
    title: qsTr("View ") + fileName
    modality: Qt.ApplicationModal
    required property string fileName

    BusyIndicator {
        anchors.centerIn: parent
        visible: fsModel.isLoading
    }

    ColumnLayout {
        anchors.margins: 10
        anchors.fill: parent
        spacing: 5
        id: columnLayout
        visible: !fsModel.isLoading

        HorizontalHeaderView {
            Layout.fillWidth: true
            implicitHeight: 25
            // Layout.leftMargin: 10
            clip: true
            movableColumns: false
            syncView: treeView

        }

        TreeView {
            id: treeView
            Layout.fillWidth: true
            Layout.fillHeight: true
            // Layout.leftMargin: 10
            selectionMode: TreeView.SelectRows
            selectionModel: ItemSelectionModel {}
            model: fsModel
            delegate: FoldersTreeViewDelegate {
                implicitHeight: 25
                implicitWidth: model.column === 0 ? treeView.width - 100 : leftMargin + __contentIndent + implicitContentWidth + rightPadding + rightMargin // Adjust based on column
            }
        }
    }

    ArchiveModel {
        id: fsModel
        archiveFileName: fileName
    }

}