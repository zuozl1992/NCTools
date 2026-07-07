import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

Dialog {
    id: root
    title: {
        var _ = LanguageManager.currentLanguage
        return qsTr("串口高级设置")
    }
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    anchors.centerIn: parent

    property int dataBits: 8
    property int stopBits: 1
    property int parity: 0
    property int flowControl: 0

    function tr(text) {
        var _ = LanguageManager.currentLanguage
        return qsTr(text)
    }

    contentItem: ColumnLayout {
        spacing: 12

        RowLayout {
            spacing: 8

            Label {
                text: tr("数据位:")
                Layout.preferredWidth: 60
            }

            ComboBox {
                id: cbData
                Layout.fillWidth: true
                model: ["5", "6", "7", "8"]
                currentIndex: 3
            }
        }

        RowLayout {
            spacing: 8

            Label {
                text: tr("停止位:")
                Layout.preferredWidth: 60
            }

            ComboBox {
                id: cbStop
                Layout.fillWidth: true
                model: ["1", "1.5", "2"]
                currentIndex: 0
            }
        }

        RowLayout {
            spacing: 8

            Label {
                text: tr("校验位:")
                Layout.preferredWidth: 60
            }

            ComboBox {
                id: cbParity
                Layout.fillWidth: true
                model: {
                    var _ = LanguageManager.currentLanguage
                    return [qsTr("无"), qsTr("偶校验"), qsTr("奇校验"), qsTr("空格"), qsTr("标记")]
                }
                currentIndex: 0
            }
        }

        RowLayout {
            spacing: 8

            Label {
                text: tr("流控制:")
                Layout.preferredWidth: 60
            }

            ComboBox {
                id: cbFlow
                Layout.fillWidth: true
                model: {
                    var _ = LanguageManager.currentLanguage
                    return [qsTr("无"), qsTr("硬件"), qsTr("软件")]
                }
                currentIndex: 0
            }
        }
    }

    onAccepted: {
        var dataMap = [5, 6, 7, 8]
        var stopMap = [1, 3, 2]
        var parityMap = [0, 2, 3, 4, 5]
        var flowMap = [0, 1, 2]

        root.dataBits = dataMap[cbData.currentIndex]
        root.stopBits = stopMap[cbStop.currentIndex]
        root.parity = parityMap[cbParity.currentIndex]
        root.flowControl = flowMap[cbFlow.currentIndex]

        AppController.settings.serialDataBits = root.dataBits
        AppController.settings.serialStopBits = root.stopBits
        AppController.settings.serialParity = root.parity
        AppController.settings.serialFlowControl = root.flowControl
    }
}
