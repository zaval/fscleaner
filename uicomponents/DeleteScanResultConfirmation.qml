import QtQuick
import uicomponents


MessageBox {
    id: root
    required property FSModel model

    property var modelIndex: null
    property int itemId: 0
    property string path: ""

    title: qsTr("Delete File")
    text: qsTr("Are you sure you want to delete\n") + root.path + "?"
    buttons: MessageBox.Yes | MessageBox.No
    onRejected: {
        root.itemId = 0
        root.path = ""
    }

    onAccepted: {
        console.log("deleting ", modelIndex)
        model["removeIndex"](root.modelIndex)

    }
}