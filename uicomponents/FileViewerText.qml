import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import fscstyle
import fsCleaner

ApplicationWindow {
    id: root
    visible: true
    width: 800
    height: 600
    title: "High Performance Text Framework"
    required property string filePath
    property string fileName: uiHelper.getFileName(filePath)

    Rectangle {
        anchors.fill: parent
        color: Theme.surfaceContainerLowest
    }

    ColumnLayout {
        id: rootLayout
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        RowLayout {
            spacing: 10
            Layout.fillWidth: true

            Label {
                text: root.fileName
                typography: "headline-md"
            }

            Item {
                id: searchSeparator
                Layout.fillWidth: true
            }
        }

        // Rectangle {
        //     Layout.fillWidth: true
        //     implicitHeight: 1
        //     color: Theme.colorOutline
        // }
        // Item {
        //     Layout.fillWidth: true
        //     implicitHeight: 5
        // }

        FullTextArea {
            Layout.fillWidth: true
            Layout.fillHeight: true
            // filePath: "/Users/zaval/projects/roadz/utilities/fleet_simulator/routes/path002_365.json"
            filePath: root.filePath
        }
    }


    UIHelper {
        id: uiHelper
    }

    // Flickable {
    //     id: scrollContainer
    //     anchors.fill: parent
    //     clip: true
    //
    //     // Map Flickable boundaries directly to C++ calculated document content sizes
    //     contentWidth: textEngine.contentWidth
    //     contentHeight: textEngine.contentHeight
    //
    //     FullTextArea {
    //         id: textEngine
    //         width: scrollContainer.width
    //         height: scrollContainer.height
    //
    //         // Pin the painted item to the viewport. The Flickable would
    //         // otherwise physically translate this item by -contentY while
    //         // scrolling; counteracting that here keeps the item fixed so it
    //         // can render the scrolled region itself instead of drifting off
    //         // screen (which previously cut the text to the first screen).
    //         x: scrollContainer.contentX
    //         y: scrollContainer.contentY
    //
    //         // Seamless bi-directional coordinate synchronization
    //         scrollX: scrollContainer.contentX
    //         scrollY: scrollContainer.contentY
    //
    //         Component.onCompleted: {
    //             // Testing simulated scenario with an instantly created sample large text chunk
    //             let largeString = "Line data sample entries...\n".repeat(50000);
    //             textEngine.setText(largeString);
    //         }
    //     }
    // }
}
