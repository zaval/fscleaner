import QtQuick
import QtQuick.Controls.Basic
import uicomponents
import fscstyle

Item {
    id: root
    property string filePath: ""

    Rectangle {
        anchors.fill: parent
        color: Theme.background
        border.color: Theme.colorOutline
        border.width: 1
        radius: Theme.roundedDefault
    }

    Flickable {
        id: scrollContainer
        anchors.fill: parent
        clip: true

        // Map Flickable boundaries directly to C++ calculated document content sizes
        contentWidth: textEngine.contentWidth
        contentHeight: textEngine.contentHeight

        FullTextAreaEngine {
            id: textEngine
            width: scrollContainer.width
            height: scrollContainer.height
            anchors.margins: 15

            // Pin the painted item to the viewport. The Flickable would
            // otherwise physically translate this item by -contentY while
            // scrolling; counteracting that here keeps the item fixed so it
            // can render the scrolled region itself instead of drifting off
            // screen (which previously cut the text to the first screen).
            x: scrollContainer.contentX
            y: scrollContainer.contentY

            // Seamless bi-directional coordinate synchronization
            scrollX: scrollContainer.contentX
            scrollY: scrollContainer.contentY

            filePath: root.filePath

            // Component.onCompleted: {
            //     // Testing simulated scenario with an instantly created sample large text chunk
            //     // Only load sample string if filePath is not set
            //     if (root.filePath === "") {
            //         let largeString = "Line data sample entries...\n".repeat(50000);
            //         textEngine.setText(largeString);
            //     }
            // }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }
        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AsNeeded
        }
    }

    // Indexing progress indicator overlay
    BusyIndicator {
        anchors.centerIn: parent
        running: textEngine.isIndexing
        visible: textEngine.isIndexing
    }
}


