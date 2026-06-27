import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import fscstyle

Rectangle {
    id: root
    required property var modelData
    // property var modelData
    required property int index

    property bool checked:  modelData.enabled
    property var deleteClicked: function(index: int){}
    property var checkedChanged: function(index: int, checked: bool){}


    color: hoverHandler.hovered ? Theme.surfaceContainerLowest : Theme.surfaceContainerLow
    HoverHandler {
        id: hoverHandler

    }
    topLeftRadius: index === 0 ? Theme.roundedSm : 0
    topRightRadius: index === 0 ? Theme.roundedSm : 0
    bottomLeftRadius: index === modelData.length - 1 ? Theme.roundedDefault : 0
    bottomRightRadius: index === modelData.length - 1 ? Theme.roundedSm : 0

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        CheckBox {
            checked: root.modelData.enabled
            onToggled: root.checkedChanged(root.index, checked)

            // onCheckedChanged: root.modelData.setEnabled(root.index, root.checked)
        }
        Item {
            implicitWidth: 24
            implicitHeight: 24
            Image {
                anchors.fill: parent
                source: "image://theme/" + root.modelData.iconName
                smooth: true
            }
        }
        ColumnLayout {
            spacing: 5

            Label {
                text: root.modelData.name
                typography: "body-md"
                font.weight: Font.Bold
            }
            Label {
                text: root.modelData.path
                typography: "mono-data"

            }
        }


        Item {
            Layout.fillWidth: true
        }

        Button {
            visible: hoverHandler.hovered
            type: "error"
            icon.name: "delete"
            onClicked: {
                console.log("delete clicked: ", root.index)
                root.deleteClicked(root.index)
            }
        }
    }
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        color: Theme.surfaceDim
        height: 1
        visible: root.index !== root.modelData.length - 1
    }
}
