import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

Rectangle {
    id: root
    height: 25
    color: "#ffffff"
    radius: 4
    border.color: "#e0e0e0"

    // Format bytes to human readable string
    function formatBytes(bytes) {
        if (bytes < 1024) return bytes + " B"
        if (bytes < 1048576) return (bytes / 1024).toFixed(1) + " KB"
        return (bytes / 1048576).toFixed(2) + " MB"
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 8

        Label {
            text: {
                var _ = LanguageManager.currentLanguage
                return AppController.connected ? qsTr("已连接") : qsTr("未连接")
            }
            font.pixelSize: 11
            color: AppController.connected ? "#4CAF50" : "#666666"
        }

        Label {
            text: {
                var _ = LanguageManager.currentLanguage
                switch(AppController.connectionType) {
                case 0: return qsTr("串口")
                case 1: return "TCP Server"
                case 2: return "TCP Client"
                case 3: return "UDP"
                default: return ""
                }
            }
            font.pixelSize: 11
            color: "#666666"
        }

        Rectangle {
            width: 1
            height: parent.height * 0.6
            color: "#e0e0e0"
        }

        Label {
            text: "TX: " + formatBytes(AppController.sentBytes)
            font.pixelSize: 11
            color: "#2196F3"
        }

        Label {
            text: "RX: " + formatBytes(AppController.receivedBytes)
            font.pixelSize: 11
            color: "#4CAF50"
        }

        Item { Layout.fillWidth: true }

        Label {
            text: "MySSCOM v2.0"
            font.pixelSize: 11
            color: "#666666"
        }
    }
}
