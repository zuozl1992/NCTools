import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import NCTools 1.0

ApplicationWindow {
    id: root
    width: 1100
    height: 700
    visible: true
    title: {
        var _ = LanguageManager.currentLanguage
        return qsTr("NCTools - 串口网络调试工具")
    }
    color: "#f5f5f5"

    // Style properties
    readonly property color bgColor: "#f5f5f5"
    readonly property color panelColor: "#ffffff"
    readonly property color accentColor: "#2196F3"
    readonly property color textColor: "#333333"
    readonly property color borderColor: "#e0e0e0"
    readonly property color successColor: "#4CAF50"
    readonly property color errorColor: "#f44336"
    readonly property font defaultFont: Qt.font({ pixelSize: 13 })
    readonly property font headerFont: Qt.font({ pixelSize: 14, weight: Font.Bold })
    readonly property int defaultRadius: 4
    readonly property int defaultSpacing: 8

    // Menu bar
    menuBar: MenuBar {
        Menu {
            title: {
                var _ = LanguageManager.currentLanguage
                return qsTr("帮助(&H)")
            }
            Action {
                text: {
                    var _ = LanguageManager.currentLanguage
                    return qsTr("关于(&A)")
                }
                onTriggered: aboutDialog.open()
            }
        }
        Menu {
            title: {
                var _ = LanguageManager.currentLanguage
                return qsTr("语言")
            }
            Action {
                text: "中文"
                checkable: true
                checked: LanguageManager.currentLanguage === "zh_CN"
                onTriggered: LanguageManager.switchLanguage("zh_CN")
            }
            Action {
                text: "English"
                checkable: true
                checked: LanguageManager.currentLanguage === "en_US"
                onTriggered: LanguageManager.switchLanguage("en_US")
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: root.defaultSpacing
        spacing: root.defaultSpacing

        // Header bar - mode selection
        HeaderBar {
            id: headerBar
            Layout.fillWidth: true
        }

        // Toolbar
        MyToolBar {
            id: toolBar
            Layout.fillWidth: true
        }

        Connections {
            target: toolBar
            function onExpandClicked() {
                quickSendPanel.visible = !quickSendPanel.visible
            }
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
                color: root.panelColor
                radius: root.defaultRadius
                border.color: root.borderColor

                StackLayout {
                    id: configStack
                    anchors.fill: parent
                    anchors.margins: root.defaultSpacing
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
                spacing: root.defaultSpacing

                // Data display area
                DataDisplay {
                    id: dataDisplay
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                // Send options
                RowLayout {
                    Layout.fillWidth: true
                    spacing: root.defaultSpacing

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
                    Layout.minimumHeight: 100
                }
            }
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

    // Quick send panel (independent window)
    QuickSendPanel {
        id: quickSendPanel
        visible: false
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
        anchors.centerIn: Overlay.overlay
        property alias text: errorLabel.text
        Label {
            id: errorLabel
            wrapMode: Text.Wrap
        }
    }
}
