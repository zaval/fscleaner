pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: "File System Cleaner"


    header: ToolBar {
        id: toolbar
        RowLayout {
            anchors.fill: parent
            spacing: 10

            ToolButton {
                icon.name: "back"
                icon.color: window.palette.text
                visible: stack.depth > 1
                onClicked: {
                    fmd.fileName = ""
                    stack.pop()
                }
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                icon.name: "settings"
                icon.color: window.palette.text
                onClicked: {
                    stack.push(settingsPage)
                }
            }
        }
    }

    StackView {
        id: stack
        initialItem: folderPage
        // initialItem: filesTreePage
        anchors.fill: parent

    }


    Component {
        id: filesTreePage

        RowLayout {
            id: filesSplitView

            FilesTreeView {
                id: filesTreeView
                Layout.fillWidth: true
                Layout.minimumWidth: 200
                fsModel: fileSystemModel
                fileMetaData: fmd
            }

            Rectangle {
                implicitWidth: 4
                Layout.fillHeight: true
                color: palette.mid
                visible: fmd.fileName !== ""
            }

            FileInfo {
                id: fInfo
                visible: fmd.fileName !== ""
                fileMetaData: fmd
                // Layout.fillWidth: true
                Layout.fillHeight: true
                // Layout.preferredWidth: fmd.fileName !== "" ? 320 : 0
                implicitWidth: fmd.fileName !== "" ? 320 : 0
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 10
            }
        }
    }

    Component {
        id: folderPage

        SelectFolderPage {
            id: checkFolderPage
            stackView: stack
            fileSystemModel: fileSystemModel
            filesTreePage: filesTreePage

        }
    }

    Component {
        id: settingsPage

        SettingsPage {
            Layout.maximumWidth: 1200
            id: settings
        }
    }

    FileSystemModel {
        id: fileSystemModel
    }

    FileMetaData{
        id: fmd
    }
}
