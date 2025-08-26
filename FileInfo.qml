import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.impl

ScrollView {
    id: root
    required property var fileMetaData

    clip: true

    ColumnLayout {
        id: columnLayout
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 15
        implicitWidth: 300

        Label {
            text: root.fileMetaData.fileName
            font.weight: Font.Bold
            font.pointSize: 20
            elide: Label.ElideMiddle
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            clip: true
            Layout.preferredWidth: parent.width
        }

        IconLabel {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            icon.name: root.fileMetaData.icon
            icon.width: 256
            icon.height: 256
            visible: !root.fileMetaData.isImage
        }

        Image {
            visible: root.fileMetaData.isImage
            source: root.fileMetaData.isImage ? "file://" + root.fileMetaData.path : ""
            sourceSize: Qt.size(256, 256)
            Layout.preferredWidth: 256
            Layout.preferredHeight: 256
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            fillMode: Image.PreserveAspectFit

        }

        RowLayout {
            visible: root.fileMetaData.isArchive
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter

            Button {
                Layout.margins: 5
                text: qsTr("View")
                onClicked: {
                    let component = Qt.createComponent("ArchiveViewerWindow.qml");
                    let window = component.createObject(null, {
                        "fileName": root.fileMetaData.path
                    });
                    window.show()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("Size:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.formattedSize
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Label {

                text: qsTr("Created time:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.createdTime
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Label {

                text: qsTr("Modified time:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.modifiedTime
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Label {

                text: qsTr("Owner:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.user
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Label {

                text: qsTr("Group:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.group
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Label {

                text: qsTr("MD5:")
            }
            Item{
                Layout.fillWidth: true
            }

            TextEdit {

                text: root.fileMetaData.md5Hash
                readOnly: true
                selectByMouse: true
                color: palette.text
                clip: true

                wrapMode: TextEdit.NoWrap
            }

        }

        Label {
            visible: root.fileMetaData.isImage
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true


            font.pointSize: 16
            font.bold: true
            text: qsTr("Image Info")
            horizontalAlignment: Text.AlignHCenter
        }

        RowLayout {
            visible: root.fileMetaData.isImage
            Layout.fillWidth: true
            Label {

                text: qsTr("Width:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.imageWidth + " px"
            }
        }

        RowLayout {
            visible: root.fileMetaData.isImage
            Layout.fillWidth: true
            Label {

                text: qsTr("Height:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.imageHeight + " px"
            }
        }

        RowLayout {
            visible: root.fileMetaData.isImage
            Layout.fillWidth: true
            Label {

                text: qsTr("Color Depth:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.depth + " bpp"
            }
        }

        Label {
            visible: root.fileMetaData.isVideo
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true


            font.pointSize: 16
            font.bold: true
            text: qsTr("Video Info")
            horizontalAlignment: Text.AlignHCenter
        }

        RowLayout {
            visible: root.fileMetaData.isVideo
            Layout.fillWidth: true
            Label {

                text: qsTr("Duration:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.videoDuration
            }
        }


        RowLayout {
            visible: root.fileMetaData.isVideo
            Layout.fillWidth: true
            Label {

                text: qsTr("Codec:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.videoCodecName
            }
        }


        RowLayout {
            visible: root.fileMetaData.isVideo
            Layout.fillWidth: true
            Label {

                text: qsTr("Format:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.videoFormatName
            }
        }

        RowLayout {
            visible: root.fileMetaData.isVideo
            Layout.fillWidth: true
            Label {

                text: qsTr("Bitrate:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.videoBitRate / 1000 + " kb/s"
            }
        }

        RowLayout {
            visible: root.fileMetaData.isVideo
            Layout.fillWidth: true
            Label {

                text: qsTr("Width:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.videoWidth + " px"
            }
        }

        RowLayout {
            visible: root.fileMetaData.isVideo
            Layout.fillWidth: true
            Label {

                text: qsTr("Height:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.videoHeight + " px"
            }
        }

        Label {
            visible: root.fileMetaData.isAudio
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true


            font.pointSize: 16
            font.bold: true
            text: qsTr("Audio Info")
            horizontalAlignment: Text.AlignHCenter
        }

        RowLayout {
            visible: root.fileMetaData.isAudio && !root.fileMetaData.isVideo
            Layout.fillWidth: true
            Label {

                text: qsTr("Duration:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.audioDuration
            }
        }


        RowLayout {
            visible: root.fileMetaData.isAudio
            Layout.fillWidth: true
            Label {

                text: qsTr("Codec:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.audioCodecName
            }
        }


        RowLayout {
            visible: root.fileMetaData.isAudio && !root.fileMetaData.isVideo
            Layout.fillWidth: true
            Label {

                text: qsTr("Format:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.audioFormatName
            }
        }

        RowLayout {
            visible: root.fileMetaData.isAudio
            Layout.fillWidth: true
            Label {

                text: qsTr("Channels:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.audioChannels
            }
        }

        RowLayout {
            visible: root.fileMetaData.isAudio
            Layout.fillWidth: true
            Label {

                text: qsTr("Bitrate:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.audioBitRate / 1000 + " kb/s"
            }
        }

        RowLayout {
            visible: root.fileMetaData.isAudio
            Layout.fillWidth: true
            Label {

                text: qsTr("Sample Rate:")
            }
            Item{
                Layout.fillWidth: true
            }
            Label {

                text: root.fileMetaData.audioSampleRate + " Hz"
            }
        }


        Item {
            Layout.fillHeight: true
            implicitHeight: 20
        }

    }

}
