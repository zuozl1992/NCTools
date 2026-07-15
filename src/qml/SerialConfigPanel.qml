import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

ColumnLayout {
    id: root
    spacing: Style.defaultSpacing

    Label {
        text: "串口配置"
        font: Style.headerFont
    }

    // Baud rate
    RowLayout {
        Layout.fillWidth: true
        spacing: Style.defaultSpacing

        Label {
            text: "波特率:"
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
        text: "更多设置"
        implicitHeight: Style.buttonHeight
        onClicked: serialSettingsDialog.open()
    }

    Item { Layout.fillHeight: true }

    // Open/Close serial button
    Button {
        id: btnOpenSerial
        Layout.fillWidth: true
        text: AppController.connected ? "关闭串口" : "打开串口"
        implicitHeight: Style.buttonHeight * 1.2
        background: Rectangle {
            color: AppController.connected ? Style.errorColor : Style.successColor
            radius: Style.defaultRadius
        }
        contentItem: Text {
            text: parent.text
            color: "white"
            font: Style.defaultFont
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        onClicked: {
            if (AppController.connected) {
                AppController.transport.closeCurrent()
            } else {
                var config = {
                    portName: cbName.currentText,
                    baudRate: parseInt(cbBaudRate.currentText)
                }
                AppController.transport.switchTransport(0, config)
                if (!AppController.transport.currentTransport || !AppController.transport.currentTransport.open()) {
                    // Error handled by signal
                }
            }
        }
    }
}
