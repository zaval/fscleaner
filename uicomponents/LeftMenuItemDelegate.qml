import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import fscstyle

Rectangle {
    id: control
    required property LeftMenuItem modelData
    property bool active: false
    required property int index

    HoverHandler{
        id: hoverHandler
    }

    color: active ? Theme.primary : hoverHandler.hovered ? Theme.surfaceDim :  Theme.surfaceContainer

    radius: Theme.roundedDefault

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        anchors.margins: 10
        Item {
            implicitWidth: 24
            implicitHeight: 24
            Image {
                id: icon
                anchors.fill: parent
                source: "image://theme/" + control.modelData.icon
                smooth: true
                // visible: false

                layer.enabled: true
                layer.effect: MultiEffect {
                    source: icon
                    anchors.fill: icon

                    // 3. Enable maximum colorization and pick your color
                    colorization: 1.0
                    colorizationColor: control.active ? Theme.colorOnPrimary : Theme.colorOnSurface
                }
            }
        }
        Label {
            id: label
            text: control.modelData.title
            // type: "primary"
            color: control.active ? Theme.colorOnPrimary : Theme.colorOnSurface
        }

        Item {
            Layout.fillWidth: true
        }
    }
}


