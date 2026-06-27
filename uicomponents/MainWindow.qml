import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import fscstyle

Rectangle {

    LeftMenu {
        id: leftMenu
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        width: 220

        model: [
            LeftMenuItem { title: "Dashboard"; icon: "dashboard" },
            LeftMenuItem { title: "ScanHistory"; icon: "history" },
            LeftMenuItem { title: "Demo"; icon: "history" },
            LeftMenuItem { title: "Settings"; icon: "settings"},
            LeftMenuItem { title: "Scan result TEST"; icon: "settings"}
        ]
    }

    StackLayout {
        id: stackedLayout
        anchors.left: leftMenu.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        currentIndex: leftMenu.currentIndex

        // SelectFolderScreen {
        //     id: scanScreen
        // }

        DashboardWindow {
            id: dashboardWindow
        }

        Item {
            id: scanHistory
            Label { text: "ScanHistory"; typography: "headline-md" }
        }

        Item {
            id: demo
            RowLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingContainerPadding
                spacing: 10

                LeftMenu {
                    implicitWidth: 200
                    implicitHeight: window.height
                    model: [
                        LeftMenuItem { title: "Item 1";  icon: "settings" },
                        LeftMenuItem { title: "Item 2";  icon: "stop-circle" },
                        LeftMenuItem { title: "Item 3";  icon: "search" },
                        LeftMenuItem { title: "Item 4";  icon: "visibility" },
                    ]
                }

                ColumnLayout {
                    spacing: 10
                    Layout.fillHeight: true
                    Button { text: "Hello" }
                    Button { text: "primary"; type: "primary" }
                    Button { text: "secondary"; type: "secondary" }
                    Button { text: "tertiary"; type: "tertiary" }
                    Button {
                        text: "outlined";
                        type: "outlined"
                        onClicked: {
                            let window = demoText.createObject(null, {filePath: "/Users/zaval/projects/roadz/utilities/fleet_simulator/routes/path002_365.json"})
                            window.show()
                        }
                    }
                    Button {
                        text: "error"
                        type: "error"
                        onClicked: {
                            let window = demoArchive.createObject(null, {filePath: "/Users/zaval/Downloads/DR17-IntroToEditing.zip"})
                            window.show()

                        }
                    }

                    Label { text: "Label" }
                    Label { text: "primary"; type: "primary" }
                    Label { text: "secondary"; type: "secondary" }
                    Label { text: "tertiary"; type: "tertiary" }
                    Label { text: "error"; type: "error" }
                    Label { text: "outlined"; type: "outlined" }

                    Item {
                        Layout.fillHeight: true
                    }
                }

                ColumnLayout {
                    spacing: 10
                    Layout.fillHeight: true
                    Label { text: "display-lg"; typography: "display-lg" }
                    Label { text: "headline-md"; typography: "headline-md" }
                    Label { text: "title-sm"; typography: "title-sm" }
                    Label { text: "body-md"; typography: "body-md" }
                    Label { text: "body-sm"; typography: "body-sm" }
                    Label { text: "label-caps"; typography: "label-caps" }
                    Label { text: "mono-data"; typography: "mono-data" }

                    Item {
                        Layout.fillHeight: true
                    }
                }

                ColumnLayout {
                    spacing: 10
                    Layout.fillHeight: true
                    Card {
                        width: 200
                        height: 200
                        color: Theme.surface
                        Label { text: "Card" }
                    }
                }
            }

            Component {
                id: demoArchive
                FileViewerArchive {}
            }
            Component {
                id: demoText
                FileViewerText {}
            }
        }

        SettingsWindow {
            id: settingsWindow
            // anchors.fill: parent
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ScanResultScreen {
            id: scanResultScreen
            dbPath: "/Users/zaval/Library/Application Support/fscleaner/FSCleaner/filesystem.db"
            stackView: null
            Component.onCompleted: {
                scanResultScreen.fsModel.initialize("/Users/zaval/Library/Application Support/fscleaner/FSCleaner/filesystem.db")
                // scanResultScreen.fsModel.filterByName = "components.xml"
            }
        }

        // FileProperties {
        //     id: fileProperties
        //     filePath: "/Users/zaval/Downloads/Download.mp4"
        //     Layout.fillWidth: true
        //     Layout.fillHeight: true
        // }

    }
}
