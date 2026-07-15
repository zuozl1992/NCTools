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
        text: tr("UDP 配置")
        font.pixelSize: 14
        font.bold: true
    }

    GroupBox {
        Layout.fillWidth: true
        title: tr("本地（接收）")
        label: Label {
            text: parent.title
            font.pixelSize: 11
            color: "#666666"
        }

        RowLayout {
            width: parent.width
            spacing: 8

            Label {
                text: tr("端口:")
                Layout.preferredWidth: 40
            }

            TextField {
                id: leLocalPort
                Layout.fillWidth: true
                text: AppController.settings.udpLocalPort
                placeholderText: "10001"
                validator: IntValidator { bottom: 1; top: 65535 }
                onEditingFinished: AppController.settings.udpLocalPort = text
            }
        }
    }

    GroupBox {
        Layout.fillWidth: true
        title: tr("远程（发送）")
        label: Label {
            text: parent.title
            font.pixelSize: 11
            color: "#666666"
        }

        ColumnLayout {
            width: parent.width
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: tr("IP:")
                    Layout.preferredWidth: 40
                }

                TextField {
                    id: lePeerIP
                    Layout.fillWidth: true
                    text: AppController.settings.udpPeerIP
                    placeholderText: "127.0.0.1"
                    onEditingFinished: AppController.settings.udpPeerIP = text
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: tr("端口:")
                    Layout.preferredWidth: 40
                }

                TextField {
                    id: lePeerPort
                    Layout.fillWidth: true
                    text: AppController.settings.udpPeerPort
                    placeholderText: "10002"
                    validator: IntValidator { bottom: 1; top: 65535 }
                    onEditingFinished: AppController.settings.udpPeerPort = text
                }
            }
        }
    }

    Item { Layout.fillHeight: true }

    Button {
        id: btnBind
        Layout.fillWidth: true
        text: AppController.connected ? tr("解绑") : tr("绑定（接收）")
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
                var localPort = parseInt(leLocalPort.text)
                if (localPort > 0) {
                    AppController.settings.udpPeerIP = lePeerIP.text
                    AppController.settings.udpPeerPort = lePeerPort.text
                    AppController.bindUdp(localPort)
                } else {
                    AppController.errorMessage(tr("请输入本地端口"))
                }
            }
        }
    }

    Label {
        Layout.fillWidth: true
        text: tr("提示：绑定本地端口用于接收数据；发送数据只需填写远程IP和端口，无需绑定。")
        font.pixelSize: 10
        color: "#999999"
        wrapMode: Text.Wrap
    }
}
