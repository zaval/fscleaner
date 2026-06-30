import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import fscstyle
import fsCleaner

Page {
    id: root
    background: Rectangle {
        color: Theme.surfaceContainerLowest
        anchors.fill: parent
    }

    property string path: ""
    property var stackView: StackView.view
    property var canceled: false


    onPathChanged: {
        // console.log("path changed: ", path)
        if (path.length){
            processRunner.processFolder(root.path, AppSettings.ignoreMounted)
            root.canceled = false
        }

    }

    footer: ToolBar {
        id: bootomToolBar
        implicitHeight: contentHeight + 20
        RowLayout {
            spacing: 10
            anchors.fill: parent
            anchors.margins: 10

            ColumnLayout {
                spacing: 2
                Layout.margins: 10
                Layout.preferredWidth: 50
                Label {
                    text: "CPU Usage"
                    typography: "body-sm"
                    font.weight: Font.Bold
                }
                RowLayout {
                    spacing: 10
                    ProgressBar {
                        Layout.fillWidth: true
                        value: processRunner.cpuUsage / 100
                    }
                    Label {
                        text: parseInt(processRunner.cpuUsage) + "%"
                        typography: "mono-data"
                    }
                }
            }

            Rectangle {
                implicitWidth: 1
                implicitHeight: 40
                color: Theme.surfaceDim
            }

            ColumnLayout {
                spacing: 2
                Layout.margins: 10
                Layout.preferredWidth: 50
                Label {
                    text: "Memory Usage"
                    typography: "body-sm"
                    font.weight: Font.Bold
                }
                RowLayout {
                    spacing: 10
                    ProgressBar {
                        Layout.fillWidth: true
                        value: processRunner.memoryUsage / 100
                    }
                    Label {
                        text: parseInt(processRunner.memoryUsage) + "%"
                        typography: "mono-data"
                    }
                }
            }

            Rectangle {
                implicitWidth: 1
                implicitHeight: 40
                color: Theme.surfaceDim
            }

            ColumnLayout {
                spacing: 2
                Layout.margins: 10

                Label {
                    text: "Time Elapsed"
                    typography: "body-sm"
                    font.weight: Font.Bold
                }
                Label {
                    text: processRunner.timeElapsed
                    typography: "mono-data"
                }
            }


            Item {
                Layout.fillWidth: true
            }

            Button {
                text: "Stop"
                type: "error"
                icon.name: "stop-circle"
                leftPadding: 30
                rightPadding: 30
                radius: Theme.roundedDefault
                onClicked: {
                    root.canceled = true
                    processRunner.stopProcess()
                    root.stackView.pop()
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        Layout.margins: 20

        Card {
            Layout.preferredWidth: parent.width * 0.8
            Layout.alignment: Qt.AlignHCenter

            ColumnLayout {
                spacing: 20
                anchors.fill: parent

                // Animation Icon
                Item {
                    Layout.preferredWidth: 256
                    Layout.preferredHeight: 256
                    Layout.alignment: Qt.AlignHCenter

                    Image {
                        id: iconBg
                        anchors.fill: parent
                        source: "image://theme/search-animation-bg"
                        smooth: true
                        sourceSize.width: 1024
                        sourceSize.height: 1024
                        // visible: false

                        layer.enabled: true
                        layer.effect: MultiEffect {
                            source: iconBg
                            anchors.fill: iconBg
                            colorization: 1.0
                            colorizationColor: Theme.primary
                        }
                    }

                    Item {
                        id: foregroundItem
                        width: 100
                        height: 100
                        anchors.centerIn: iconBg

                        // anchors.topMargin: 20
                        Image {
                            id: iconFg
                            anchors.fill: parent
                            source: "image://theme/search-animation-fg"
                            smooth: true
                            sourceSize.width: 1024
                            sourceSize.height: 1024
                            layer.enabled: true
                            layer.effect: MultiEffect {
                                source: iconFg
                                anchors.fill: iconFg

                                // 3. Enable maximum colorization and pick your color
                                colorization: 1.0
                                colorizationColor: Theme.primary
                            }
                        }

                        NumberAnimation {
                            id: animateRotation
                            target: foregroundItem
                            properties: "rotation"
                            from: 0
                            to: 360
                            duration: 1000
                            loops: Animation.Infinite
                            // easing {type: Easing.OutBack; overshoot: 500}
                            easing {type: Easing.InOutQuart}
                            running: true
                        }
                    }


                }
                // /Animation Icon

                Item {
                    Layout.preferredWidth: 400
                    Layout.preferredHeight: 20
                    Layout.alignment: Qt.AlignHCenter
                    RowLayout {
                        anchors.fill: parent
                        visible: processRunner.isRunning

                        spacing: 0
                        Label {
                            text: processRunner.logMessageLine
                            font.weight: Font.Bold
                            typography: "body-sm"
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }

    ProcessRunner {
        id: processRunner
        onProcessFinished: {
            console.log("process finished")
            if (!root.canceled)
                root.stackView.push(scanResultScreen, {"dbPath": processRunner.dbPath})
        }
    }

}