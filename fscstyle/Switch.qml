import QtQuick
import QtQuick.Templates as T

T.Switch {
    id: control


    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: 6
    spacing: 6


    indicator: Rectangle {
        id: indicator
        implicitWidth: 48
        implicitHeight: 26
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 13
        color: control.checked ? Theme.primary : Theme.colorOutline
        // color: control.checked ? Theme.primary : Theme.colorOnSurfaceVariant
        // border.color: control.checked ? Theme.primary : Theme.colorOnSurfaceVariant
        border.color: control.checked ? Theme.primary : Theme.colorOutline

        Rectangle {
            id: handle
            x: control.checked ? parent.width - width : 0
            width: 26
            height: 26
            radius: 13
            color: control.down ? Qt.darker(Theme.surface, 1.2) : Theme.surface
            border.color: control.checked ? (control.down ? Qt.darker(Theme.primary) : Theme.primary) : Theme.colorOutline
            // border.color: control.checked ? (control.down ? Qt.darker(Theme.primary) : Theme.primary) : Theme.colorOnSurfaceVariant
        }

        states: State {
            name: "checked"
            when: control.checked
            PropertyChanges {
                target: handle
                x: indicator.width - width
            }

            PropertyChanges {
                target: indicator
                color: Theme.primary
            }
        }

        transitions: Transition {
            ColorAnimation {
                targets: [indicator, handle]
                property: "color"
                duration: 200
                easing.type: Easing.InOutQuad
            }

            PropertyAnimation {
                targets: handle
                property: "x"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.down ? Qt.darker(Theme.primary, 1.2) : Theme.primary
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
