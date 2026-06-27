// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T

T.Label {
    id: control

    property string type: ""
    property string typography: ""

    color: control.type === "" ? Theme.colorOnSurface : Theme.getColor(control.type)
    font.family: Theme.getFontFamily(control.typography)
    font.pointSize: Theme.getFontSize(control.typography)
    font.weight: Theme.getFontWeight(control.typography)

    // linkColor: control.palette.link
}
