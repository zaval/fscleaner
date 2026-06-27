**User**: 
Fix the ScrollView. It is scrolling GridView only and overlap the upper ColumnLayout


**Assistant:** 

The problem is that the `ScrollView` is given a fixed `contentHeight: parent.height`, and the `GridView` is still acting as its own scrollable/flickable area. Make the `ScrollView` own the scrolling, and let the `GridView` expand to its full content height.

Update `SelectFolderScreen.qml` like this:


```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import fscstyle


Item {

    ScrollView {
        id: scrollView

        anchors.fill: parent
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: scrollView.availableWidth
            spacing: 10

            Label {
                Layout.fillWidth: true
                Layout.topMargin: 10
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Qt.AlignHCenter
                Layout.alignment: Qt.AlignVCenter
                text: "Select folder to scan"
                typography: "headline-md"
            }

            Label {
                Layout.fillWidth: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Qt.AlignHCenter
                text: "Choose a directory to identify clutter and redundant files."
                typography: "body-sm"
            }

            GridView {
                Layout.minimumWidth: 640
                Layout.preferredHeight: contentHeight
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 10

                interactive: false
                boundsBehavior: Flickable.StopAtBounds

                model: folderSettingsListModel
                cellWidth: 320
                cellHeight: 90

                delegate: Card {
                    id: cardDelegate
                    required property var modelData
                    required property int index
                    implicitWidth: 300

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent

                        Item {
                            implicitWidth: 24
                            implicitHeight: 24
                            Image {
                                anchors.fill: parent
                                source: "image://theme/" + cardDelegate.modelData.iconName
                                smooth: true
                            }
                        }

                        ColumnLayout {
                            spacing: 5

                            Label {
                                text: cardDelegate.modelData.name
                                typography: "body-md"
                                font.weight: Font.Bold
                            }

                            Label {
                                text: cardDelegate.modelData.path
                                typography: "mono-data"
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }

    FolderSettingsListModel {
        id: folderSettingsListModel
    }
}
```

Key changes:

- Removed `contentHeight: parent.height`.
- Removed `anchors.fill: parent` from the `ColumnLayout`.
- Set `ColumnLayout.width` to `scrollView.availableWidth`.
- Disabled `GridView` scrolling with `interactive: false`.
- Made the `GridView` height follow `contentHeight`, so the outer `ScrollView` scrolls the whole page, including the labels above it.


