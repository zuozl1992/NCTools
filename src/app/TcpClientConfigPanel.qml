import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

ColumnLayout {
    id: root
    spacing: 8

    function tr(text) {
        var _ = LanguageManager.currentLanguage
        return qsTr(text)
    }

    Label {
        text: tr("TCP Client 配置")
        font.pixelSize: 14
        font.bold: true
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        Label {
            text: tr("服务器IP:")
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

    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        Label {
            text: tr("端口:")
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

    Button {
        id: btnConnect
        Layout.fillWidth: true
        text: AppController.connected ? tr("断开连接") : tr("连接服务器")
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
                var ip = leIP.text
                var port = parseInt(lePort.text)
                if (ip && port > 0) {
                    AppController.connectTcpClient(ip, port)
                } else {
                    AppController.errorMessage(tr("请输入服务器IP和端口"))
                }
            }
        }
    }
}
