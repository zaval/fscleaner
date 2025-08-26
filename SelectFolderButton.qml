import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.impl

// This custom button is built on AbstractButton, giving you the core logic
// for states like pressed, hovered, etc., while allowing for a completely
// custom appearance and behavior.

AbstractButton {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property color buttonColor: control.palette.button
    property color buttonHoveredColor: control.palette.highlight
    property color buttonPressedColor: control.palette.button
    property color textColor: control.palette.buttonText
    property string description: ""

    // Border and Radius
    // property color borderColor: "#9e9e9e"
    property color borderColor: control.palette.placeholderText
    property int borderWidth: 1
    property int borderRadius: 8

    // Icon configuration
    // The icon is sourced from the theme via the 'icon.name' property.
    property int iconSize: 20

    // Font configuration
    // property font textFont: Qt.font({ family: "Arial", pointSize: 14, weight: Font.Normal })

    padding: 10 // Padding around the content for better touch targets
    icon.width: 64
    icon.height: 64

    contentItem:

        RowLayout {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10


            IconLabel {
                id: iconLabel
                spacing: control.spacing
                mirrored: control.mirrored
                display: control.display

                icon: control.icon
                color: control.visualFocus ? control.palette.highlight : control.palette.buttonText
            }


            ColumnLayout {
                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true

                Label {
                    id: caption
                    text: control.text
                    font.weight: Font.Bold
                    // font: Qt.font({ family: "Helvetica", pointSize: 16, weight: Font.Bold})
                    color: control.textColor
                    elide: Text.ElideRight
                    visible: control.display !== AbstractButton.IconOnly
                }
                Label {
                    id: description
                    text: control.description
                    // font: Qt.font({ family: "Helvetica", pointSize: 12, weight: Font.Normal})
                    color: control.textColor
                    elide: Text.ElideRight
                    visible: control.display !== AbstractButton.IconOnly && control.description !== ""
                    // visible: control.display !== AbstractButton.IconOnly
                }
            }

            Item {
                Layout.fillWidth: true
            }



        // }
    }

    // --- Background ---
    // This is the visual representation of the button itself.
    // We use a Rectangle for the main body, border, and radius.
    background: Rectangle {
        // The color changes based on the button's state (pressed, hovered).
        color: control.pressed ? control.buttonPressedColor : (control.hovered ? control.buttonHoveredColor : control.buttonColor)

        // Bind the visual properties to our custom properties.
        radius: control.borderRadius
        border.color: control.borderColor
        border.width: control.borderWidth

        // A smooth transition for color changes.
        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
}
