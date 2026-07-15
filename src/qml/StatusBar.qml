import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

Rectangle {
    id: root
    height: 25
    color: Style.panelColor
    radius: Style.defaultRadius
    border.color: Style.borderColor

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Style.defaultSpacing
        anchors.rightMargin: Style.defaultSpacing
        spacing: Style.defaultSpacing

        Label {
            text: AppController.connected ? "已连接" : "未连接"
            font: Style.smallFont
            color: AppController.connected ? Style.successColor : Style.textSecondaryColor
        }

        Label {
            text: {
                var types = ["串口", "TCP Server", "TCP Client", "UDP"]
                return types[AppController.connectionType] || ""
            }
            font: Style.smallFont
            color: Style.textSecondaryColor
        }

        Item { Layout.fillWidth: true }

        Label {
            text: "NCTools v2.0"
            font: Style.smallFont
            color: Style.textSecondaryColor
        }
    }
}
