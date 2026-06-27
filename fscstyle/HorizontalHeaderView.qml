// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default
import QtQuick
import QtQuick.Templates as T

T.HorizontalHeaderView {
    id: control

    implicitWidth: syncView ? syncView.width : 0
    // The contentHeight of TableView will be zero at start-up, until the delegate
    // items have been loaded. This means that even if the implicit height of
    // HorizontalHeaderView should be the same as the content height in the end, we
    // need to ensure that it has at least a height of 1 at start-up, otherwise
    // TableView won't bother loading any delegates at all.
    implicitHeight: Math.max(1, contentHeight)


    delegate: T.HeaderViewDelegate {
        id: delegate

        // same as AbstractButton.qml
        implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
            implicitContentWidth + leftPadding + rightPadding)
        implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
            implicitContentHeight + topPadding + bottomPadding)

        padding: 8

        highlighted: selected

        background: Rectangle {
            // border.color: Theme.colorOutline
            // color: Theme.surfaceDim
            color: Theme.surfaceContainer
        }

        contentItem: T.Label {
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: Theme.colorOnSurface
            text: delegate.model[delegate.headerView.textRole]
            font.weight: Font.Bold
            font.family: Theme.bodyMdFontFamily
        }
    }
}
