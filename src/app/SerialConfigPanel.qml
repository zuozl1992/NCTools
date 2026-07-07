import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

ColumnLayout {
    id: root
    spacing: 8

    // Helper to force translation re-evaluation
    function tr(text) {
        var _ = LanguageManager.currentLanguage
        return qsTr(text)
    }

    Label {
        text: tr("串口配置")
        font.pixelSize: 14
        font.bold: true
    }

    // Port selection
    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        Label {
            text: tr("端口:")
            Layout.preferredWidth: 60
        }

        ComboBox {
            id: cbPort
            Layout.fillWidth: true
            model: AppController.transport.availablePorts
        }
    }

    // Baud rate
    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        Label {
            text: tr("波特率:")
            Layout.preferredWidth: 60
        }

        ComboBox {
            id: cbBaudRate
            Layout.fillWidth: true
            model: ["9600", "57600", "115200"]
            currentIndex: AppController.settings.serialBaudRateIndex
            onCurrentIndexChanged: AppController.settings.serialBaudRateIndex = currentIndex
        }
    }

    // More settings button
    Button {
        Layout.fillWidth: true
        text: tr("更多设置")
        implicitHeight: 32
        onClicked: serialSettingsDialog.open()
    }

    Item { Layout.fillHeight: true }

    // Open/Close serial button
    Button {
        id: btnOpenSerial
        Layout.fillWidth: true
        text: AppController.connected ? tr("关闭串口") : tr("打开串口")
        implicitHeight: 48
        background: Rectangle {
            color: AppController.connected ? "#f44336" : "#4CAF50"
            radius: 4
        }
        contentItem: Text {
            text: parent.text
            color: "white"
            font.pixelSize: 14
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        onClicked: {
            if (AppController.connected) {
                AppController.closeConnection()
            } else {
                var portName = cbPort.currentText
                var baudRate = parseInt(cbBaudRate.currentText)
                if (portName) {
                    AppController.openSerialPort(portName, baudRate)
                } else {
                    AppController.errorMessage(tr("请选择串口"))
                }
            }
        }
    }
}
