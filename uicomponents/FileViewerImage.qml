import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import fscstyle
import uicomponents

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

    RowLayout {
        anchors.fill: parent
        spacing: 5
        Item {
            id: imageContainer
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 10
            Image {
                width: imageContainer.width
                height: imageContainer.height
                source: "file://" + filePath
                fillMode: Image.PreserveAspectFit
            }
        }
        Rectangle {
            implicitWidth: 1
            Layout.fillHeight: true
            color: Theme.colorOutline
        }

        ScrollView {
            id: scrollView
            Layout.preferredWidth: 350
            Layout.fillHeight: true

            contentWidth: availableWidth
            clip: true

            ColumnLayout {
                id: metadataContainer
                spacing: 15
                clip: true
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 10
                // Layout.topMargin: 10
                width: scrollView.availableWidth - 20

                Label {
                    Layout.fillWidth: true
                    text: root.fileName
                    typography: "headline-md"
                    Layout.alignment: Qt.AlignHCenter
                    Layout.maximumWidth: 300
                    wrapMode: Text.WrapAnywhere
                    clip: true
                }

                MetadataRow {
                    name: "Image Size"
                    value: fmd.imageWidth + " X " + fmd.imageHeight
                }
                MetadataRow {
                    name: "Color Depth"
                    value: fmd.depth + " bits"
                }
                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 1
                    color: Theme.colorOutline
                }
                MetadataRow {
                    name: "Make"
                    value: exiv2.make
                    visible: exiv2.make !== ""
                }
                MetadataRow {
                    name: "Model"
                    value: exiv2.model
                    visible: exiv2.model !== ""
                }
                MetadataRow {
                    name: "Lens Make"
                    value: exiv2.lensMake
                    visible: exiv2.lensMake !== ""
                }
                MetadataRow {
                    name: "Lens Model"
                    value: exiv2.lensModel
                    visible: exiv2.lensModel !== ""
                }
                MetadataRow {
                    name: "Resolution"
                    value: exiv2.resolution + " dpi"
                    visible: exiv2.resolution !== 0
                }
                MetadataRow {
                    name: "Focal Length"
                    value: exiv2.focalLength + " mm"
                    visible: exiv2.focalLength !== 0
                }
                MetadataRow {
                    name: "Exposure Time"
                    value: exiv2.exposureTime + " s"
                    visible: exiv2.exposureTime !== 0
                }
                MetadataRow {
                    name: "F Number"
                    value: "F" + exiv2.fNumber
                    visible: exiv2.fNumber !== 0
                }
                MetadataRow {
                    name: "Shutter Speed"
                    value: exiv2.shutterSpeed + " s"
                    visible: exiv2.shutterSpeed !== 0
                }
                MetadataRow {
                    name: "Aperture"
                    value: "F" + exiv2.apertureValue
                    visible: exiv2.apertureValue !== 0
                }
                MetadataRow {
                    name: "Distance to object"
                    value: exiv2.subjectDistance + " m"
                    visible: exiv2.subjectDistance !== 0
                }
                MetadataRow {
                    name: "White Balance"
                    value: exiv2.whiteBalance ? exiv2.whiteBalance : "Auto"
                    visible: exiv2.whiteBalance !== 0
                }
                MetadataRow {
                    name: "Exposure Mode"
                    value: exiv2.exposureMode ? exiv2.exposureMode : "Auto"
                    visible: exiv2.exposureMode !== 0
                }
                MetadataRow {
                    name: "Flash"
                    value: "On"
                    visible: exiv2.flash
                }
                MetadataRow {
                    name: "Latitude"
                    value: exiv2.gpsLatitude ? exiv2.gpsLatitude : "N/A"
                    visible: exiv2.gpsLatitude !== ""
                }
                MetadataRow {
                    name: "Longitude"
                    value: exiv2.gpsLongitude ? exiv2.gpsLongitude : "N/A"
                    visible: exiv2.gpsLongitude !== ""
                }
                MetadataRow {
                    name: "Address"
                    value: exiv2.gpsAddress
                    visible: exiv2.gpsAddress !== ""
                }
                RowLayout {
                    Layout.fillWidth: true

                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        visible: exiv2.gpsLatitudeDecimal !== 92 && exiv2.gpsLongitudeDecimal !== 182
                        type: "outlined"
                        text: qsTr("Show on map")
                        onClicked: {
                            let url = "https://www.google.com/maps/place/" + exiv2.gpsLatitudeDecimal + "," + exiv2.gpsLongitudeDecimal + "/@" + exiv2.gpsLatitudeDecimal + "," + exiv2.gpsLongitudeDecimal + ",200m/"
                            Qt.openUrlExternally(url)
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                    implicitHeight: 10
                }
            }
        }
    }

    FileMetaData {
        id: fmd
        path: root.filePath
    }

    UIHelper {
        id: uiHelper
    }

    Exiv2Metadata {
        id: exiv2
        filePath: root.filePath
    }
}
