import QtQuick
import QtQuick.Controls
import uicomponents


ApplicationWindow {
    id: window
    visible: true
    width: 1024
    height: 768
    title: "MainTest"

    // background: Rectangle {
    //     anchors.fill: parent
    //     color: Theme.surface
    // }

    MainWindow {
        anchors.fill: parent
    }
}