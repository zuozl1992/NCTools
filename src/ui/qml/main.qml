import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Item {
    id: root
    width: parent ? parent.width : 1080
    height: parent ? parent.height : 720

    property bool autoSendActive: false
    property int autoSendIntervalMs: parseInt(autoSendInterval.text, 10) || 0

    Timer {
        id: autoSendTimer
        interval: root.autoSendIntervalMs
        repeat: true
        running: root.autoSendActive && root.autoSendIntervalMs > 0
        onTriggered: sendInputPayload()
    }

    FileDialog {
        id: openFileDialog
        title: qsTr("选择文件")
        nameFilters: [qsTr("所有文件 (*)")]
        onAccepted: {
            var path = openFileDialog.selectedFile.toString()
            if (path.length > 0)
                fileField.text = path
        }
    }

    FileDialog {
        id: saveFileDialog
        title: qsTr("保存数据")
        fileMode: FileDialog.SaveFile
        nameFilters: [qsTr("文本文件 (*.txt)"), qsTr("所有文件 (*)")]
        onAccepted: {
            var path = saveFileDialog.selectedFile.toString()
            if (path.length > 0)
                console.log("TODO: save payload to", path)
        }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        Item {
            SplitView.preferredWidth: 330
            SplitView.fillHeight: true

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                GroupBox {
                    title: qsTr("连接设置")
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8

                        ComboBox {
                            id: transportSelector
                            Layout.fillWidth: true
                            model: transportManager.availableTransports()
                            textRole: "name"
                            onCurrentIndexChanged: {
                                if (currentIndex >= 0)
                                    transportManager.activeTransportIndex = currentIndex
                            }
                        }

                        Label {
                            Layout.fillWidth: true
                            text: transportManager.connected ? transportManager.connectionMessage : qsTr("未连接")
                            wrapMode: Text.WordWrap
                            color: transportManager.connected ? "#1b9e5a" : "#cc5a5a"
                        }

                        Button {
                            text: transportManager.connected ? qsTr("断开") : qsTr("连接")
                            Layout.fillWidth: true
                            onClicked: {
                                if (transportManager.connected)
                                    transportManager.closeCurrentTransport()
                                else
                                    transportManager.openCurrentTransport()
                            }
                        }
                    }
                }

                GroupBox {
                    title: qsTr("串口选项")
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 6

                        RowLayout {
                            Layout.fillWidth: true
                            TextField {
                                id: portNameField
                                Layout.fillWidth: true
                                placeholderText: qsTr("串口号 (如 COM3、/dev/tty.usbmodem)")
                            }
                            Button {
                                text: qsTr("应用")
                                onClicked: {
                                    serialTransport.setPortName(portNameField.text)
                                    serialTransport.setBaudRate(parseInt(baudRateField.text, 10) || 9600)
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            TextField {
                                id: baudRateField
                                Layout.fillWidth: true
                                placeholderText: qsTr("波特率")
                                text: "9600"
                                inputMethodHints: Qt.ImhDigitsOnly
                            }
                            Button {
                                text: qsTr("更多")
                                onClicked: serialMoreDialog.open()
                            }
                        }
                    }
                }

                GroupBox {
                    title: qsTr("网络选项")
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 6

                        TextField {
                            id: hostField
                            placeholderText: qsTr("目标地址 / 本机绑定地址")
                            Layout.fillWidth: true
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            TextField {
                                id: portField
                                Layout.fillWidth: true
                                placeholderText: qsTr("端口")
                                inputMethodHints: Qt.ImhDigitsOnly
                            }
                            Button {
                                text: qsTr("应用")
                                onClicked: {
                                    var value = parseInt(portField.text, 10) || 0
                                    tcpClientTransport.setTarget(hostField.text, value)
                                    tcpServerTransport.setListenPort(value)
                                    udpTransport.setBindAddress(hostField.text, value)
                                    udpTransport.setTargetAddress(hostField.text, value)
                                }
                            }
                        }
                    }
                }

                GroupBox {
                    title: qsTr("发送设置")
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 6

                        Switch {
                            id: hexSendSwitch
                            text: qsTr("HEX 发送")
                        }

                        Switch {
                            id: timestampSwitch
                            text: qsTr("加时间戳和分包显示")
                        }

                        Switch {
                            id: autoSendSwitch
                            text: qsTr("定时发送")
                            onCheckedChanged: root.autoSendActive = checked
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            TextField {
                                id: autoSendInterval
                                Layout.fillWidth: true
                                text: "1000"
                                inputMethodHints: Qt.ImhDigitsOnly
                                onTextChanged: root.autoSendIntervalMs = parseInt(text, 10) || 0
                            }
                            Label { text: "ms" }
                        }

                        Switch {
                            id: autoClearSwitch
                            text: qsTr("发送后清空发送框")
                        }
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }

        Item {
            SplitView.fillWidth: true
            SplitView.fillHeight: true

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Button { text: qsTr("清除屏幕"); onClicked: outputArea.clear() }
                    Button { text: qsTr("刷新端口"); onClicked: transportManager.refreshAvailablePorts() }
                    Button { text: qsTr("保存数据"); onClicked: saveFileDialog.open() }
                    Button { text: qsTr("打开文件"); onClicked: openFileDialog.open() }
                    TextField {
                        id: fileField
                        Layout.fillWidth: true
                        placeholderText: qsTr("文件路径")
                        readOnly: true
                    }
                    Button { text: qsTr("发送文件"); onClicked: sendFilePayload(fileField.text) }
                    Button { text: qsTr("扩展"); onClicked: expDrawer.visible = !expDrawer.visible }
                }

                ProgressBar {
                    id: fileProgress
                    Layout.fillWidth: true
                    from: 0
                    to: 100
                    value: 0
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 8
                    color: "#10131a"
                    border.color: "#262b36"

                    Flickable {
                        id: outputFlick
                        anchors.fill: parent
                        anchors.margins: 10
                        contentWidth: width
                        contentHeight: outputArea.implicitHeight
                        flickableDirection: Flickable.VerticalFlick
                        clip: true

                        TextEdit {
                            id: outputArea
                            width: parent.width
                            wrapMode: TextEdit.Wrap
                            readOnly: true
                            color: "#f5f6f8"
                            font.pixelSize: 14
                            textFormat: TextEdit.PlainText
                        }

                        onContentHeightChanged: {
                            if (contentHeight > height)
                                contentY = contentHeight - height
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        RowLayout {
                            spacing: 6
                            Button { text: qsTr("扩展面板"); onClicked: expDrawer.visible = !expDrawer.visible }
                            Button { text: qsTr("打开文件"); onClicked: openFileDialog.open() }
                            Button { text: qsTr("保存数据"); onClicked: saveFileDialog.open() }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 160
                            radius: 8
                            color: "#0e1117"
                            border.color: "#222835"

                            Flickable {
                                anchors.fill: parent
                                anchors.margins: 8
                                contentWidth: width
                                contentHeight: inputArea.implicitHeight
                                flickableDirection: Flickable.VerticalFlick
                                clip: true

                                TextEdit {
                                    id: inputArea
                                    width: parent.width
                                    wrapMode: TextEdit.Wrap
                                    color: "#f5f6f8"
                                    font.pixelSize: 14
                                    textFormat: TextEdit.PlainText
                                }
                            }
                        }
                    }

                    Button {
                        text: qsTr("发送")
                        Layout.preferredWidth: 120
                        Layout.fillHeight: true
                        onClicked: sendInputPayload()
                    }
                }

                Drawer {
                    id: expDrawer
                    edge: Qt.RightEdge
                    width: 340
                    height: parent.height

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 8

                        Label {
                            text: qsTr("扩展面板")
                            font.pixelSize: 16
                            font.bold: true
                        }

                        Repeater {
                            model: 20
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8
                                TextField {
                                    id: expField
                                    Layout.fillWidth: true
                                    placeholderText: qsTr("命令 %1").arg(index + 1)
                                }
                                Switch {
                                    id: expHexSwitch
                                    text: qsTr("HEX")
                                }
                                Button {
                                    text: qsTr("发送")
                                    onClicked: appendOutput(qsTr("[扩展 %1] %2").arg(index + 1).arg(expField.text))
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Dialog {
        id: serialMoreDialog
        title: qsTr("串口高级设置")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        contentItem: ColumnLayout {
            spacing: 10

            GroupBox {
                title: qsTr("数据位")
                Layout.fillWidth: true
                ComboBox { id: dataBitsBox; model: ["Data_5", "Data_6", "Data_7", "Data_8"]; currentIndex: 3 }
            }

            GroupBox {
                title: qsTr("停止位")
                Layout.fillWidth: true
                ComboBox { id: stopBitsBox; model: ["OneStop", "OneAndHalfStop", "TwoStop"]; currentIndex: 0 }
            }

            GroupBox {
                title: qsTr("校验位")
                Layout.fillWidth: true
                ComboBox { id: parityBox; model: ["NoParity", "EvenParity", "OddParity", "SpaceParity", "MarkParity"]; currentIndex: 0 }
            }

            GroupBox {
                title: qsTr("流控制")
                Layout.fillWidth: true
                ComboBox { id: flowControlBox; model: ["NoFlowControl", "HardwareControl", "SoftwareControl"]; currentIndex: 0 }
            }
        }

        onAccepted: {
            var dataBits = [5, 6, 7, 8]
            var stopBits = [1, 3, 2]
            var parity = [0, 2, 3, 4, 5]
            var flowControl = [0, 1, 2]

            var settings = {
                "dataBits": dataBits[dataBitsBox.currentIndex] ?? 8,
                "stopBits": stopBits[stopBitsBox.currentIndex] ?? 1,
                "parity": parity[parityBox.currentIndex] ?? 0,
                "flowControl": flowControl[flowControlBox.currentIndex] ?? 0
            }

            serialTransport.applySettings(settings)
            appendOutput(qsTr("已更新串口高级配置"))
        }
    }

    Connections {
        target: transportManager
        function onPayloadReady(payload, transportName, direction) {
            var text = root.hexArrayToDisplay(payload, direction === 0)
            appendOutput(text)
        }
        function onErrorOccurred(message) {
            appendOutput(qsTr("[错误] %1").arg(message))
        }
    }

    function sendInputPayload() {
        var payload = inputArea.text
        if (payload.length === 0)
            return

        var data = hexSendSwitch.checked ? hexStringToBytes(payload) : stringToBytes(payload)
        if (!transportManager.sendData(data))
            appendOutput(qsTr("[发送失败] 当前连接不可用"))

        if (autoClearSwitch.checked)
            inputArea.clear()
    }

    function sendFilePayload(path) {
        if (path.length === 0) {
            appendOutput(qsTr("未选择文件"))
            return
        }
        appendOutput(qsTr("文件发送占位：%1").arg(path))
    }

    function appendOutput(message) {
        outputArea.append(message)
    }

    function stringToBytes(text) {
        var result = []
        for (var i = 0; i < text.length; ++i)
            result.push(text.charCodeAt(i) & 0xFF)
        return result
    }

    function hexStringToBytes(hex) {
        var cleaned = hex.replace(/[^0-9a-fA-F]/g, '')
        if (cleaned.length % 2 !== 0)
            cleaned = '0' + cleaned
        var result = []
        for (var i = 0; i < cleaned.length; i += 2)
            result.push(parseInt(cleaned.substr(i, 2), 16))
        return result
    }

    function hexArrayToDisplay(payload, isReceive) {
        var prefix = isReceive ? qsTr("[接收] ") : qsTr("[发送] ")
        var view = new DataView(payload)
        var hex = []
        for (var i = 0; i < view.byteLength; ++i) {
            var value = view.getUint8(i).toString(16).toUpperCase()
            hex.push(value.length < 2 ? '0' + value : value)
        }
        return prefix + hex.join(' ')
    }
}
