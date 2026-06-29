import QtQuick
import QtQuick.Controls
import uicomponents


ApplicationWindow {
    id: window
    visible: true
    width: 1024
    height: 768
    title: qsTr("FS Cleaner")

    MainWindow {
        anchors.fill: parent
    }
}