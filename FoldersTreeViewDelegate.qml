pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.impl
import QtQuick.Templates as T
import QtQuick.Layouts


T.TreeViewDelegate {
    id: control

    implicitWidth: leftMargin + __contentIndent + implicitContentWidth + rightPadding + rightMargin  + spacing + 32
    implicitHeight: Math.max(indicator ? indicator.height : 0, implicitContentHeight) * 1.25

    indentation: indicator ? indicator.width : 12
    leftMargin: 4
    rightMargin: 4
    // leftMargin: 0
    // rightMargin: 0
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
        // width: 16
        // height: 16

        readonly property real __indicatorIndent: control.leftMargin + (control.depth * control.indentation)
        x: !control.mirrored ? __indicatorIndent : control.width - __indicatorIndent - width
        y: (control.height - height) / 2
        implicitWidth: 20
        implicitHeight: 40
        ColorImage {
            width: 5
            height: 10
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            rotation:  control.expanded ? 90 : (control.mirrored ? 180 : 0)
            source: "qrc:/qt-project.org/imports/QtQuick/Controls/Basic/images/arrow-indicator.png"
            color: control.palette.windowText
            defaultColor: "#353637"
        }
    }

    background: Rectangle {
        implicitHeight: 40 // same as Button.qml
        implicitWidth: control.implicitContentWidth
        color: control.highlighted
            ? control.palette.highlight
            : control.palette.window
    }

    contentItem: Item {
        clip: true
        implicitHeight: control.implicitHeight
        implicitWidth: label.implicitWidth
        RowLayout {
            anchors.fill: parent
            spacing: control.spacing

            IconLabel {
                icon.name: "folder"
                icon.height: 20
                icon.width: 20

                visible: control.model.column === 0 && control.hasChildren
                Layout.alignment: Qt.AlignHCenter
            }

            IconLabel {
                icon.name: fti.getIcon(control.model.display)
                icon.height: 20
                icon.width: 20
                visible: control.model.column === 0 && !control.hasChildren
                Layout.alignment: Qt.AlignHCenter

            }
            T.Label {
                id: label
                // Layout.alignment: control.model.column === 0 ? Qt.AlignHCenter : Qt.AlignRight
                // Layout.alignment: Qt.AlignRight
                clip: true
                text: control.model.display
                horizontalAlignment: control.model.column === 0 ? Qt.AlignHCenter : Qt.AlignRight
                elide: Text.ElideRight
                color: control.highlighted ? control.palette.highlightedText : control.palette.buttonText
                visible: !control.editing
            }
            Item {
                Layout.fillWidth: true
                visible: control.model.column === 0
            }
        }
    }

    // The edit delegate is a separate component, and doesn't need
    // to follow the same strict rules that are applied to a control.
    // qmllint disable attached-property-reuse
    TableView.editDelegate: FocusScope {
        width: parent.width
        height: parent.height

        readonly property int __role: {
            let model = control.treeView.model
            let index = control.treeView.index(control.model.row, control.model.column)
            let editText = model.data(index, Qt.EditRole)
            return editText !== undefined ? Qt.EditRole : Qt.DisplayRole
        }

        T.TextField {
            id: textField
            x: control.contentItem.x
            y: (parent.height - height) / 2
            width: control.contentItem.width
            // text: control.treeView.model.data(control.treeView.index(row, column), __role)
            text: control.treeView.model.data(control.treeView.index(control.model.row, control.model.column), control.model.role)
            focus: true
        }

        TableView.onCommit: {
            let index = TableView.view.index(control.model.row, control.model.column)
            TableView.view.model.setData(index, textField.text, __role)
        }

        Component.onCompleted: textField.selectAll()
    }

    FileTypeIcon {
        id: fti
    }
    // qmllint enable attached-property-reuse
}
