import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    implicitWidth: parent.width // Match parent width
    z: 1 // Ensure header is potentially above scrollbars if needed

    property Item treeView: null

    Label {
        text: "Name"
        font.bold: true
        elide: Text.ElideRight
        padding: 5
        Layout.fillWidth: true
        // Adjust preferred width, considering potential scrollbar width (~20px)
        Layout.preferredWidth: parent.treeView.width * 0.6 - 10 // Approximate
    }
    Label {
        text: "Size"
        font.bold: true
        horizontalAlignment: Text.AlignRight
        padding: 5
        Layout.fillWidth: true
        Layout.preferredWidth: parent.treeView.width * 0.3 - 10 // Approximate
    }
    Item { Layout.preferredWidth: 20 } // Adjust width as needed
}
