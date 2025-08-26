import QtQuick
import Qt.labs.platform
import QtQml

Menu {

    id: root
    property var viewDelegate: null
    property var confirmDialog: null

    MenuItem {
        text: root.viewDelegate.hasChildren ? qsTr("Open in %1").arg("Explorer") : qsTr("Open in default application")
        shortcut: StandardKey.Open
        onTriggered: {
            Qt.openUrlExternally("file://" + root.viewDelegate.model.filePath);
        }
    }

    MenuItem {
        text: qsTr("Delete")
        shortcut: StandardKey.Delete
        onTriggered: {
            confirmDialog.filePath = root.viewDelegate.model.filePath
            confirmDialog.open()
        }
    }


}