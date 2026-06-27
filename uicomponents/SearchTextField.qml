import QtQuick
// import QtQuick.Controls.Basic
import QtQuick.Effects
import fscstyle

TextField {
    id: root
    placeholderText: "Search..."

    signal edited(string text)

    onEditingFinished: {
        // fileSystemModel.filterByName = root.text
        edited(root.text)
    }

    Image {
        id: searchIcon
        source: root.text === "" ? "image://theme/search" : "image://theme/close"
        width: 20
        height: 20
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.rightMargin: 5

        layer.enabled: true
        layer.effect: MultiEffect {
            source: searchIcon
            anchors.fill: searchIcon
            colorization: 1.0
            colorizationColor: Theme.colorOutline
        }
        HoverHandler {
            cursorShape: Qt.ArrowCursor
        }

        TapHandler {

            onSingleTapped: {
                if (root.text === ""){
                    return
                }
                root.text = ""
                // fileSystemModel.filterByName = ""
                edited("")
            }
        }
    }
}