import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property int radius: Theme.roundedSm
    padding: 6
    horizontalPadding: padding + 2
    spacing: 6

    property string type: ""

    icon.width: 24
    icon.height: 24
    icon.color: Theme.getOnColor(control.type)

    contentItem: IconLabel {
        id: contentItem
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: Theme.getOnColor(control.type)
    }

    background: Rectangle {
        // implicitWidth: 100
        implicitWidth: contentItem.width + 2 * horizontalPadding
        implicitHeight: 40

        visible: !control.flat || control.down || control.checked || control.highlighted
        color: control.down ? Qt.darker(Theme.getColor(control.type), 1.2) : Theme.getColor(control.type)
        border.color: Theme.colorOutline
        border.width: control.type === "outlined" ? 1 : 0
        radius: control.radius
    }
}
