import QtQuick
import QtQuick.Templates as T
import QtQuick.Effects

T.BusyIndicator {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    contentItem: Image {
        id: contentIcon

        property color effectColor: Theme.primary

        source: "image://theme/progress-activity"
        width: 48
        height: 48

        layer.enabled: true
        layer.effect: MultiEffect {
            source: contentIcon
            anchors.fill: contentIcon

            // 3. Enable maximum colorization and pick your color
            colorization: 1.0
            colorizationColor: contentIcon.effectColor
        }

    }

    RotationAnimation {
        target: contentIcon
        from: 0
        to: 360
        duration: 1000        // Time in milliseconds for one full turn
        loops: Animation.Infinite
        running: true         // Starts automatically
    }

    SequentialAnimation {
        running: true
        loops: Animation.Infinite

        ColorAnimation {
            target: contentIcon
            property: "effectColor"
            from: Theme.primary
            to: Theme.secondary
            duration: 500
            easing.type: Easing.InOutQuad
        }

        ColorAnimation {
            target: contentIcon
            property: "effectColor"
            from: Theme.secondary
            to: Theme.tertiary
            duration: 500
            easing.type: Easing.InOutQuad
        }

        ColorAnimation {
            target: contentIcon
            property: "effectColor"
            from: Theme.tertiary
            to: Theme.error
            duration: 500
            easing.type: Easing.InOutQuad
        }

        ColorAnimation {
            target: contentIcon
            property: "effectColor"
            from: Theme.error
            to: Theme.primary
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }



}
