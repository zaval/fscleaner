import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import fscstyle

ApplicationWindow {
    id: root
    visible: true
    width: 800
    height: 600
    modality: Qt.ApplicationModal

    required property string filePath
    property string fileName: uiHelper.getFileName(filePath)
    title: qsTr("View ") + fileName

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

        FullTextArea {
            Layout.fillWidth: true
            Layout.fillHeight: true
            filePath: root.filePath
        }
    }


    UIHelper {
        id: uiHelper
    }
}
