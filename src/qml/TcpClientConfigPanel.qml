import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

ColumnLayout {
    id: root
    spacing: Style.defaultSpacing

    Label {
        text: "TCP Client 配置"
        font: Style.headerFont
    }

    // Server IP
    RowLayout {
        Layout.fillWidth: true
        spacing: Style.defaultSpacing

        Label {
            text: "服务器IP:"
            Layout.preferredWidth: 60
        }

        TextField {
            id: leIP
            Layout.fillWidth: true
            text: AppController.settings.tcpClientIP
            placeholderText: "127.0.0.1"
            onEditingFinished: AppController.settings.tcpClientIP = text
        }
    }

    // Server port
    RowLayout {
        Layout.fillWidth: true
        spacing: Style.defaultSpacing

        Label {
            text: "端口:"
            Layout.preferredWidth: 60
        }

        TextField {
            id: lePort
            Layout.fillWidth: true
            text: AppController.settings.tcpClientPort
            placeholderText: "10000"
            validator: IntValidator { bottom: 1; top: 65535 }
            onEditingFinished: AppController.settings.tcpClientPort = text
        }
    }

    Item { Layout.fillHeight: true }

    // Connect/Disconnect button
    Button {
        id: btnConnect
        Layout.fillWidth: true
        text: AppController.connected ? "断开连接" : "连接服务器"
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
                AppController.settings.tcpClientIP = leIP.text
                AppController.settings.tcpClientPort = lePort.text
                AppController.connectionType = 2
            }
        }
    }
}
