import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import NCTools 1.0

ApplicationWindow {
    id: root
    width: 900
    height: 650
    visible: true
    title: "NCTools - 串口网络调试工具"
    color: Style.backgroundColor

    // Menu bar
    menuBar: MenuBar {
        Menu {
            title: "帮助(&H)"
            Action {
                text: "关于(&A)"
                onTriggered: aboutDialog.open()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.defaultSpacing
        spacing: Style.defaultSpacing

        // Header bar - port/mode selection
        HeaderBar {
            id: headerBar
            Layout.fillWidth: true
        }

        // Toolbar
        ToolBar {
            id: toolBar
            Layout.fillWidth: true
        }

        // Main content area
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            // Left panel - configuration
            Rectangle {
                SplitView.preferredWidth: 250
                SplitView.minimumWidth: 200
                color: Style.panelColor
                radius: Style.defaultRadius
                border.color: Style.borderColor

                StackLayout {
                    id: configStack
                    anchors.fill: parent
                    anchors.margins: Style.panelPadding
                    currentIndex: headerBar.connectionType

                    SerialConfigPanel {}
                    TcpServerConfigPanel {}
                    TcpClientConfigPanel {}
                    UdpConfigPanel {}
                }
            }

            // Right panel - data display and send
            ColumnLayout {
                SplitView.fillWidth: true
                spacing: Style.defaultSpacing

                // Data display area
                DataDisplay {
                    id: dataDisplay
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                // Send options
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Style.defaultSpacing

                    CheckBox {
                        id: cbHexShow
                        text: "Hex显示"
                        checked: AppController.settings.hexShow
                        onCheckedChanged: AppController.settings.hexShow = checked
                    }

                    CheckBox {
                        id: cbHexSend
                        text: "Hex发送"
                        checked: AppController.settings.hexSend
                        onCheckedChanged: AppController.settings.hexSend = checked
                    }

                    CheckBox {
                        id: cbAutoClear
                        text: "发送后清空"
                        checked: AppController.settings.autoClearSend
                        onCheckedChanged: AppController.settings.autoClearSend = checked
                    }

                    CheckBox {
                        id: cbTimestamp
                        text: "加时间戳"
                        checked: AppController.settings.addTimestamp
                        onCheckedChanged: AppController.settings.addTimestamp = checked
                    }

                    Item { Layout.fillWidth: true }

                    CheckBox {
                        id: cbAutoSend
                        text: "定时发送"
                        checked: AppController.settings.autoSend
                        onCheckedChanged: {
                            AppController.settings.autoSend = checked
                            if (checked) {
                                AppController.startAutoSend(parseInt(leInterval.text))
                            } else {
                                AppController.stopAutoSend()
                            }
                        }
                    }

                    TextField {
                        id: leInterval
                        width: 60
                        text: AppController.settings.autoSendInterval.toString()
                        placeholderText: "ms"
                        validator: IntValidator { bottom: 10 }
                        onEditingFinished: AppController.settings.autoSendInterval = parseInt(text)
                    }

                    Label {
                        text: "ms"
                    }
                }

                // Send panel
                SendPanel {
                    id: sendPanel
                    Layout.fillWidth: true
                }
            }
        }

        // Quick send panel (collapsible)
        QuickSendPanel {
            id: quickSendPanel
            Layout.fillWidth: true
            visible: false
        }

        // File send progress
        FileSendProgress {
            id: fileSendProgress
            Layout.fillWidth: true
        }

        // Status bar
        StatusBar {
            id: statusBar
            Layout.fillWidth: true
        }
    }

    // Dialogs
    AboutDialog {
        id: aboutDialog
    }

    SerialSettingsDialog {
        id: serialSettingsDialog
    }

    FileDialog {
        id: openFileDialog
        title: "选择文件"
        nameFilters: ["所有文件 (*)"]
        onAccepted: {
            toolBar.filePath = selectedFile
        }
    }

    FileDialog {
        id: saveFileDialog
        title: "保存数据"
        fileMode: FileDialog.SaveFile
        nameFilters: ["文本文件 (*.txt)", "所有文件 (*)"]
        onAccepted: AppController.saveDataToFile(selectedFile)
    }

    // Connections
    Connections {
        target: AppController
        function onErrorMessage(msg) {
            errorDialog.text = msg
            errorDialog.open()
        }
    }

    Dialog {
        id: errorDialog
        title: "警告"
        modal: true
        standardButtons: Dialog.Ok
        property alias text: errorLabel.text
        Label {
            id: errorLabel
            wrapMode: Text.Wrap
        }
    }
}
