import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
// import fsCleaner
import fscstyle

ScrollView {
    id: root

    property string filePath: ""
    property int itemId: -1

    signal deleteRequested(int itemId, string filePath)

    clip: true
    contentWidth: availableWidth

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Item {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: 100
            Layout.preferredWidth: 100
            visible: !fmd.isImage

            Image {
                anchors.fill: parent
                smooth: true
                source: "image://theme/" + fmd.icon
                sourceSize.height: 256
                sourceSize.width: 256
            }
        }
        Item {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            visible: fmd.isImage

            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                smooth: true
                source: fmd.isImage ? "file://" + fmd.path : ""
                sourceSize.height: fmd.imageHeight
                sourceSize.width: fmd.imageWidth
            }
        }
        Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: 300
            clip: true
            horizontalAlignment: Text.AlignHCenter
            text: fmd.fileName
            typography: "headline-md"
            wrapMode: Text.WrapAnywhere
        }

        // buttons
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.preferredWidth: viewButton.width + deleteButton.width + 20
            spacing: 10

            Button {
                id: viewButton

                bottomPadding: 8
                icon.name: "visibility"
                leftPadding: 24
                radius: Theme.roundedDefault
                rightPadding: 24
                text: "View"
                topPadding: 8
                type: "primary"
                onClicked: {
                    if (fmd.isArchive){
                        // let component = Qt.createComponent("FileViewerArchive.qml");
                        let window = fileViewerArchive.createObject(null, {filePath: root.filePath})
                        window.show()
                    }
                    if (fmd.isPlainText){
                        let window = fileViewerText.createObject(null, {filePath: root.filePath})
                        window.show()
                    }

                }
            }
            Button {
                id: deleteButton

                bottomPadding: 8
                icon.name: "delete"
                leftPadding: 24
                radius: Theme.roundedDefault
                rightPadding: 24
                text: "Delete"
                topPadding: 8
                type: "error"

                onClicked: {
                    root.deleteRequested(root.itemId, root.filePath);
                }
            }
        }
        // /buttons

        Item {
            Layout.fillWidth: true
            implicitHeight: 20
        }

        // metadata
        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            Rectangle {
                Layout.fillWidth: true
                color: Theme.colorOutline
                implicitHeight: 1
            }
            Label {
                color: Theme.colorOutline
                text: "METADATA"
                typography: "body-sm"
            }
            Rectangle {
                Layout.fillWidth: true
                color: Theme.colorOutline
                implicitHeight: 1
            }
        }
        // /metadata

        MetadataRow {
            name: "Size"
            value: fmd.formattedSize
        }
        MetadataRow {
            name: "Created"
            value: fmd.createdTime
        }
        MetadataRow {
            name: "Modified"
            value: fmd.modifiedTime
        }
        MetadataRow {
            name: "Owner"
            value: fmd.user
        }
        MetadataRow {
            name: "Group"
            value: fmd.group
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 20
            visible: fmd.isImage

            MetadataRow {
                name: "Image Size"
                value: fmd.imageWidth + " X " + fmd.imageHeight
            }
            MetadataRow {
                name: "Color Depth"
                value: fmd.depth + " bits"
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 20
            visible: fmd.isVideo

            MetadataRow {
                name: "Video Duration"
                value: fmd.videoDuration
            }
            MetadataRow {
                name: "Image Size"
                value: fmd.videoWidth + " X " + fmd.videoHeight
            }
            MetadataRow {
                name: "Video Bitrate"
                value: fmd.videoBitRate + " b/s"
            }
            MetadataRow {
                name: "Video Sample Rate"
                value: fmd.videoSampleRate
            }
            MetadataRow {
                name: "Video Codec"
                value: fmd.videoCodecName
            }
            MetadataRow {
                name: "Video Format"
                value: fmd.videoFormatName
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 20
            visible: fmd.isAudio

            MetadataRow {
                name: "Audio Duration"
                value: fmd.audioDuration
            }
            MetadataRow {
                name: "Audio Bitrate"
                value: fmd.audioBitRate + " b/s"
            }
            MetadataRow {
                name: "Audio Sample Rate"
                value: fmd.audioSampleRate + " Hz"
            }
            MetadataRow {
                name: "Audio Codec"
                value: fmd.audioCodecName
            }
            MetadataRow {
                name: "Audio Format"
                value: fmd.audioFormatName
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            Layout.maximumHeight: root.contentHeight
            spacing: 20
            visible: fmd.isPlainText

            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                Rectangle {
                    Layout.fillWidth: true
                    color: Theme.colorOutline
                    implicitHeight: 1
                }
                Label {
                    color: Theme.colorOutline
                    text: "FILE CONTENT"
                    typography: "body-sm"
                }
                Rectangle {
                    Layout.fillWidth: true
                    color: Theme.colorOutline
                    implicitHeight: 1
                }
            }
            TextArea {
                id: hashArea

                Layout.fillWidth: true
                // Layout.fillHeight: true
                font.family: Theme.monoDataFontFamily
                padding: 10
                readOnly: true
                text: fmd.textContent
                wrapMode: TextArea.WrapAnywhere
                // palette.highlightedText: Theme.colorOnSurface
                // palette.text: Theme.colorOnSurface
                // palette.highlight: Theme.surfaceDim
                // background: Rectangle {
                //     color: Theme.surface
                //     radius: Theme.roundedDefault
                //     border.color: Theme.colorOutline
                // }

            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            Rectangle {
                Layout.fillWidth: true
                color: Theme.colorOutline
                implicitHeight: 1
            }
            Label {
                color: Theme.colorOutline
                text: "FILE INTEGRITY"
                typography: "body-sm"
            }
            Rectangle {
                Layout.fillWidth: true
                color: Theme.colorOutline
                implicitHeight: 1
            }
        }
        HashMetadataRow {
            Layout.maximumWidth: 300
            name: "MD5 Hash"
            value: fmd.md5Hash
        }
        HashMetadataRow {
            Layout.maximumWidth: 300
            name: "SHA256 Hash"
            value: fmd.sha256Hash
        }
        Item {
            Layout.fillWidth: true
            implicitHeight: 20
        }
    }
    FileMetaData {
        id: fmd

        path: root.filePath
    }
    Component {
        id: fileViewerArchive

        FileViewerArchive {
        }
    }
    Component {
        id: fileViewerText

        FileViewerText {
        }
    }
}
