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
            LeftMenuItem { title: "Settings"; icon: "settings"}
            // LeftMenuItem { title: "Scan result TEST"; icon: "settings"}
        ]
    }

    StackLayout {
        id: stackedLayout
        anchors.left: leftMenu.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        currentIndex: leftMenu.currentIndex

        DashboardWindow {
            id: dashboardWindow
        }

        Item {
            id: scanHistory
            Label { text: "ScanHistory"; typography: "headline-md" }
        }


        SettingsWindow {
            id: settingsWindow
            // anchors.fill: parent
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        // ScanResultScreen {
        //     id: scanResultScreen
        //     dbPath: "/Users/zaval/Library/Application Support/fscleaner/FSCleaner/filesystem.db"
        //     stackView: null
        //     Component.onCompleted: {
        //         scanResultScreen.fsModel.initialize("/Users/zaval/Library/Application Support/fscleaner/FSCleaner/filesystem.db")
        //     }
        // }

    }
}
