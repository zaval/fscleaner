import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import fscstyle

RowLayout {
    id: root
    Layout.fillWidth: true
    spacing: 10
    required property string name
    required property string value
    clip: true

    Label {
        text: root.name
        typography: "body-md"
        Layout.alignment: Qt.AlignTop
    }
    Item {
        Layout.fillWidth: true
    }
    Label {
        text: root.value
        typography: "mono-data"
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop
        horizontalAlignment: Text.AlignRight
    }
}
