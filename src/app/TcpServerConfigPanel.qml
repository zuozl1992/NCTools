import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

ColumnLayout {
    id: root
    spacing: 8

    function tr(text) {
        var _ = LanguageManager.currentLanguage
        return qsTr(text)
    }

    Label {
        text: tr("TCP Server 配置")
        font.pixelSize: 14
        font.bold: true
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        Label {
            text: tr("监听端口:")
            Layout.preferredWidth: 60
        }

        TextField {
            id: lePort
            Layout.fillWidth: true
            text: AppController.settings.tcpServerPort
            placeholderText: "10000"
            validator: IntValidator { bottom: 1; top: 65535 }
            onEditingFinished: AppController.settings.tcpServerPort = text
        }
    }

    Item { Layout.fillHeight: true }

    Button {
        id: btnStart
        Layout.fillWidth: true
        text: AppController.connected ? tr("停止服务器") : tr("启动服务器")
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
                var port = parseInt(lePort.text)
                if (port > 0) {
                    AppController.startTcpServer(port)
                } else {
                    AppController.errorMessage(tr("请输入监听端口"))
                }
            }
        }
    }
}
