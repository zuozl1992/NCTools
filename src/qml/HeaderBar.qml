import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

Rectangle {
    id: root
    height: 50
    color: Style.panelColor
    radius: Style.defaultRadius
    border.color: Style.borderColor

    property int connectionType: cbName.currentIndex < cbName.count - 3 ? 0 :
                                 cbName.currentIndex === cbName.count - 3 ? 1 :
                                 cbName.currentIndex === cbName.count - 2 ? 2 : 3

    RowLayout {
        anchors.fill: parent
        anchors.margins: Style.defaultSpacing
        spacing: Style.defaultSpacing

        Label {
            text: "端口/模式:"
            font: Style.defaultFont
        }

        ComboBox {
            id: cbName
            Layout.fillWidth: true
            model: {
                var ports = AppController.transport.availablePorts
                var list = ports.slice()
                list.push("TCP Server")
                list.push("TCP Client")
                list.push("UDP")
                return list
            }
            onCurrentIndexChanged: {
                if (currentIndex >= 0) {
                    AppController.connectionType = root.connectionType
                }
            }
        }

        Button {
            text: "刷新"
            implicitHeight: Style.buttonHeight
            onClicked: AppController.refreshPorts()
        }

        // Manual port input
        TextField {
            id: leManualPort
            Layout.preferredWidth: 120
            placeholderText: "手动输入端口"
            implicitHeight: Style.inputHeight
        }

        Button {
            text: "添加"
            implicitHeight: Style.buttonHeight
            onClicked: {
                AppController.addManualPort(leManualPort.text)
                leManualPort.clear()
            }
        }
    }
}
