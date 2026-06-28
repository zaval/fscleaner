// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

import QtQuick
import QtQuick.Templates as T
import QtQuick.Shapes

T.CheckBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    spacing: 6
    padding: 0

    indicator: Rectangle {
        id: indicatorRect
        implicitWidth: 14
        implicitHeight: 14
        x: control.leftPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        radius: 3

        readonly property bool isCheckedOrPartially: control.checkState !== Qt.Unchecked

        color: {
            if (!control.enabled) {
                return isCheckedOrPartially ? "#dcdcdc" : "#f5f5f5"
            }
            if (isCheckedOrPartially) {
                return "transparent"
            }
            return "#ffffff"
        }

        gradient: (control.enabled && isCheckedOrPartially) ? blueGradient : null

        Gradient {
            id: blueGradient
            GradientStop {
                position: 0.0
                color: control.pressed ? "#2f8aff" : (control.hovered ? "#3f9aff" : "#4fa4ff")
            }
            GradientStop {
                position: 1.0
                color: control.pressed ? "#005ecb" : (control.hovered ? "#006ee0" : "#007aff")
            }
        }

        border.width: 1
        border.color: {
            if (!control.enabled) {
                return isCheckedOrPartially ? "transparent" : "#e0e0e0"
            }
            if (isCheckedOrPartially) {
                return control.pressed ? "#005ecb" : (control.hovered ? "#006ee0" : "#0062cc")
            }
            return control.pressed ? "#909090" : (control.hovered ? "#a8a8a8" : "#c3c3c3")
        }

        Behavior on border.color { ColorAnimation { duration: 80 } }

        Shape {
            id: checkmark
            anchors.centerIn: parent
            width: 8
            height: 8
            visible: control.checkState === Qt.Checked
            antialiasing: true

            ShapePath {
                strokeWidth: 1.8
                strokeColor: control.enabled ? "#ffffff" : "#a0a0a0"
                fillColor: "transparent"
                joinStyle: ShapePath.RoundJoin
                capStyle: ShapePath.RoundCap
                strokeStyle: ShapePath.SolidLine

                startX: 1.2
                startY: 4.2
                PathLine { x: 3.2; y: 6.2 }
                PathLine { x: 6.8; y: 1.5 }
            }
        }

        Rectangle {
            id: dash
            anchors.centerIn: parent
            width: 7
            height: 2
            radius: 1
            color: control.enabled ? "#ffffff" : "#a0a0a0"
            visible: control.checkState === Qt.PartiallyChecked
        }

        Rectangle {
            anchors.fill: parent
            anchors.margins: -3
            radius: parent.radius + 3
            color: "transparent"
            border.color: "#007aff"
            border.width: 2.2
            opacity: 0.5
            visible: control.visualFocus
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.4
        color: Theme.colorOnSurface
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
