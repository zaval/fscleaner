import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import fscstyle
import fsCleaner

ScrollView {
    id: root
    clip: true

    background: Rectangle {
        color: Theme.surfaceContainer
    }

    property int currentIndex: 0

    property list<LeftMenuItem> model

    ColumnLayout {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        anchors.topMargin: 30
        spacing: 5

        Label{
            text: "System Cleaner"
            typography: "headline-md"
            type: "primary"
        }
        Label{
            text: "v" + uiHelper.version
            typography: "mono-data"
        }
    }

    ListView {
        id: listView
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10

        boundsBehavior: Flickable.StopAtBounds
        model: root.model
        delegate: LeftMenuItemDelegate {

            active: root.currentIndex === index

            width: root.width - 20
            height: 50

            TapHandler{
                onTapped: root.currentIndex = index
            }
        }
    }

    UIHelper {
        id: uiHelper
    }


}