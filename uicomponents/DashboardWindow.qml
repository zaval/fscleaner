import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

StackView {
    id: stackView
    clip: true
    initialItem: selectFolderScreen

    StackLayout.onIsCurrentItemChanged: {
        if (StackLayout.isCurrentItem){
            folderSettingsListModel.refreshFolders()
        }
    }

    SelectFolderScreen {
        id: selectFolderScreen
        folderSettingsListModel: folderSettingsListModel
    }

    Component {
        id: scanScreen
        ScanScreen {
        }
    }

    Component {
        id: scanResultScreen
        ScanResultScreen {}
    }

    FolderSettingsListModel {
        id: folderSettingsListModel
        showDisabled: false
    }
}
