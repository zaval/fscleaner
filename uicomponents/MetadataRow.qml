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

    Label {
        text: root.name
        typography: "body-md"
    }
    Item {
        Layout.fillWidth: true
    }
    Label {
        text: root.value
        typography: "mono-data"
    }
}
