import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Effects
import fscstyle

Pane {
    id: control
    property color color: Theme.surface
    property color borderColor: Theme.surfaceDim
    property int borderWidth: 0
    property int borderRadius: Theme.roundedSm

    property bool elevated: true
    property real elevation: 0.1

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
        implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
        implicitContentHeight + topPadding + bottomPadding)

    padding: 12

    HoverHandler {
        onHoveredChanged: {
            control.elevation = hovered ? 1 : 0.1
        }
    }

    background: Item {

        // 1. The Shadow Component
        RectangularShadow {
            anchors.fill: containerRect
            radius: containerRect.radius
            blur: 5          // How soft/spread out the shadow is
            // spread: 0.1       // Intensity of the shadow edge
            spread: control.elevation       // Intensity of the shadow edge
            color: "#40000000" // Semi-transparent black shadow
            // color: Theme.surfaceDim
            visible: control.elevated
        }

        // 2. The Actual Frame Background
        Rectangle {
            id: containerRect
            anchors.fill: parent
            color: control.color
            border.color: control.borderColor
            border.width: control.borderWidth > 0 ? control.borderWidth : (control.elevated ? 0 : 1)
            radius: control.borderRadius
        }
    }
}