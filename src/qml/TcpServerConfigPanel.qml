import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

ColumnLayout {
    id: root
    spacing: Style.defaultSpacing

    Label {
        text: "TCP Server 配置"
        font: Style.headerFont
    }

    // Listen port
    RowLayout {
        Layout.fillWidth: true
        spacing: Style.defaultSpacing

        Label {
            text: "监听端口:"
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

    // Start/Stop button
    Button {
        id: btnStart
        Layout.fillWidth: true
        text: AppController.connected ? "停止服务器" : "启动服务器"
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
                AppController.settings.tcpServerPort = lePort.text
                AppController.connectionType = 1
            }
        }
    }
}
