import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import fscstyle

ColumnLayout {

    id: root
    required property string name
    required property string value
    clip: true

    spacing: 5
    Label {
        text: root.name
        typography: "body-md"
    }

    TextArea {
        id: hashArea
        text: root.value
        readOnly: true
        padding: 10
        Layout.fillWidth: true
        font.family: Theme.monoDataFontFamily
        wrapMode: TextArea.WrapAnywhere
        palette.highlightedText: Theme.colorOnSurface
        palette.text: Theme.colorOnSurface
        palette.highlight: Theme.surfaceDim
        background: Rectangle {
            color: Theme.surface
            radius: Theme.roundedDefault
            border.color: Theme.colorOutline
        }
    }
}