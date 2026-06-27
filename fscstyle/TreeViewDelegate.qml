// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import QtQuick.Effects

T.TreeViewDelegate {
    id: control

    implicitWidth: leftMargin + __contentIndent + implicitContentWidth + rightPadding + rightMargin
    // implicitHeight: Math.max(indicator ? indicator.height : 0, implicitContentHeight) * 1.25
    implicitHeight: Math.max(indicator ? indicator.height : 0, implicitContentHeight) * 2
    // implicitHeight: 44

    indentation: indicator ? indicator.width : 12
    leftMargin: 4
    rightMargin: 4
    spacing: 4


    topPadding: contentItem ? (height - contentItem.implicitHeight) / 2 : 0
    leftPadding: !mirrored ? leftMargin + __contentIndent : width - leftMargin - __contentIndent - implicitContentWidth

    highlighted: control.selected || control.current
               || ((control.treeView.selectionBehavior === TableView.SelectRows
               || control.treeView.selectionBehavior === TableView.SelectionDisabled)
               && control.row === control.treeView.currentRow)

    required property int row
    required property var model
    readonly property real __contentIndent: !isTreeNode ? 0 : (depth * indentation) + (indicator ? indicator.width + spacing : 0)

    indicator: Item {
        // Create an area that is big enough for the user to
        // click on, since the image is a bit small.
        readonly property real __indicatorIndent: control.leftMargin + (control.depth * control.indentation)
        x: !control.mirrored ? __indicatorIndent : control.width - __indicatorIndent - width
        y: (control.height - height) / 2
        implicitWidth: 20
        implicitHeight: 20 // same as Button.qml
        Image {
            id: indicatorIcon
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            rotation:  control.expanded ? 90 : (control.mirrored ? 180 : 0)
            width: 20
            height: 20
            // source: "qrc:/qt-project.org/imports/QtQuick/Controls/Basic/images/arrow-indicator.png"
            source: "image://theme/arrow-indicator"

            layer.enabled: true
            layer.effect: MultiEffect {
                source: indicatorIcon
                anchors.fill: indicatorIcon

                // 3. Enable maximum colorization and pick your color
                colorization: 1.0
                colorizationColor: Theme.colorOutline
            }
        }
    }

    background: Rectangle {
        implicitHeight: 40 // same as Button.qml
        // border.color: control.current ? control.palette.highlight : control.palette.windowText
        border.color: control.current ? Theme.colorOutline : Theme.colorOnSurface
        border.width: Qt.styleHints.accessibility.contrastPreference !== Qt.HighContrast ? 0 :
                      control.current ? 2 : 1
        color: control.highlighted ? Theme.primary : Theme.surfaceContainerLowest
    }


    contentItem: IconLabel {
        clip: false
        text: control.model.display
        // elide: Text.ElideRight
        color: control.highlighted ? Theme.colorOnPrimary : Theme.colorOnSurface
        visible: !control.editing
        // icon.name: control.model.column === 0 ? "folder" : ""
        icon.name: control.model.icon
        icon.height: 20
        icon.width: 20
        font.family: control.model.column === 0 ? Theme.bodyMdFontFamily : Theme.monoDataFontFamily
        spacing: control.spacing
        alignment: Qt.AlignLeft | Qt.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter

    }

    // The edit delegate is a separate component, and doesn't need
    // to follow the same strict rules that are applied to a control.
    // qmllint disable attached-property-reuse
    // qmllint disable controls-attached-property-reuse
    // qmllint disable controls-sanity
    TableView.editDelegate: FocusScope {
        width: parent.width
        height: parent.height

        readonly property int __role: {
            let model = control.treeView.model
            let index = control.treeView.index(row, column)
            let editText = model.data(index, Qt.EditRole)
            return editText !== undefined ? Qt.EditRole : Qt.DisplayRole
        }

        T.TextField {
            id: textField
            x: control.contentItem.x
            y: (parent.height - height) / 2
            width: control.contentItem.width
            text: control.treeView.model.data(control.treeView.index(row, column), __role)
            focus: true
        }

        TableView.onCommit: {
            let index = TableView.view.index(row, column)
            TableView.view.model.setData(index, textField.text, __role)
        }

        Component.onCompleted: textField.selectAll()
    }
    // qmllint enable attached-property-reuse
    // qmllint enable controls-attached-property-reuse
    // qmllint enable controls-sanity
}
