import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

Dialog {
    id: root
    title: "串口高级设置"
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    anchors.centerIn: parent

    property int dataBits: 8
    property int stopBits: 1
    property int parity: 0
    property int flowControl: 0

    contentItem: ColumnLayout {
        spacing: 12

        // Data bits
        RowLayout {
            spacing: 8

            Label {
                text: "数据位:"
                Layout.preferredWidth: 60
            }

            ComboBox {
                id: cbData
                Layout.fillWidth: true
                model: ["5", "6", "7", "8"]
                currentIndex: 3 // Data8
            }
        }

        // Stop bits
        RowLayout {
            spacing: 8

            Label {
                text: "停止位:"
                Layout.preferredWidth: 60
            }

            ComboBox {
                id: cbStop
                Layout.fillWidth: true
                model: ["1", "1.5", "2"]
                currentIndex: 0
            }
        }

        // Parity
        RowLayout {
            spacing: 8

            Label {
                text: "校验位:"
                Layout.preferredWidth: 60
            }

            ComboBox {
                id: cbParity
                Layout.fillWidth: true
                model: ["无", "偶校验", "奇校验", "空格", "标记"]
                currentIndex: 0
            }
        }

        // Flow control
        RowLayout {
            spacing: 8

            Label {
                text: "流控制:"
                Layout.preferredWidth: 60
            }

            ComboBox {
                id: cbFlow
                Layout.fillWidth: true
                model: ["无", "硬件", "软件"]
                currentIndex: 0
            }
        }
    }

    onAccepted: {
        // Map combo box indices to QSerialPort values
        var dataMap = [5, 6, 7, 8]
        var stopMap = [1, 3, 2]  // OneStop, OneAndHalfStop, TwoStop
        var parityMap = [0, 2, 3, 4, 5]  // NoParity, EvenParity, OddParity, SpaceParity, MarkParity
        var flowMap = [0, 1, 2]  // NoFlowControl, HardwareControl, SoftwareControl

        root.dataBits = dataMap[cbData.currentIndex]
        root.stopBits = stopMap[cbStop.currentIndex]
        root.parity = parityMap[cbParity.currentIndex]
        root.flowControl = flowMap[cbFlow.currentIndex]

        // Update settings
        AppController.settings.serialDataBits = root.dataBits
        AppController.settings.serialStopBits = root.stopBits
        AppController.settings.serialParity = root.parity
        AppController.settings.serialFlowControl = root.flowControl
    }
}
