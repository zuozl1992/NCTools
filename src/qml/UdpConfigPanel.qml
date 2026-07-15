import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

ColumnLayout {
    id: root
    spacing: Style.defaultSpacing

    Label {
        text: "UDP 配置"
        font: Style.headerFont
    }

    // Local port
    GroupBox {
        Layout.fillWidth: true
        title: "本地"
        label: Label {
            text: parent.title
            font: Style.smallFont
            color: Style.textSecondaryColor
        }

        RowLayout {
            width: parent.width
            spacing: Style.defaultSpacing

            Label {
                text: "端口:"
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

    // Remote settings
    GroupBox {
        Layout.fillWidth: true
        title: "远程"
        label: Label {
            text: parent.title
            font: Style.smallFont
            color: Style.textSecondaryColor
        }

        ColumnLayout {
            width: parent.width
            spacing: Style.defaultSpacing

            RowLayout {
                Layout.fillWidth: true
                spacing: Style.defaultSpacing

                Label {
                    text: "IP:"
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
                spacing: Style.defaultSpacing

                Label {
                    text: "端口:"
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

    // Bind/Unbind button
    Button {
        id: btnBind
        Layout.fillWidth: true
        text: AppController.connected ? "解绑" : "绑定"
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
                AppController.settings.udpLocalPort = leLocalPort.text
                AppController.settings.udpPeerIP = lePeerIP.text
                AppController.settings.udpPeerPort = lePeerPort.text
                AppController.connectionType = 3
            }
        }
    }
}
