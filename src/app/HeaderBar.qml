import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

Rectangle {
    id: root
    height: 50
    color: "#ffffff"
    radius: 4
    border.color: "#e0e0e0"

    property int connectionType: cbMode.currentIndex

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Label {
            text: {
                var _ = LanguageManager.currentLanguage
                return qsTr("连接模式:")
            }
            font.pixelSize: 13
        }

        ComboBox {
            id: cbMode
            Layout.fillWidth: true
            model: {
                var _ = LanguageManager.currentLanguage
                return [qsTr("串口"), "TCP Server", "TCP Client", "UDP"]
            }
            currentIndex: AppController.connectionType
            onCurrentIndexChanged: {
                if (currentIndex >= 0) {
                    AppController.connectionType = currentIndex
                }
            }
        }

        // Manual port input for serial
        RowLayout {
            spacing: 8
            visible: root.connectionType === 0

            TextField {
                id: leManualPort
                Layout.preferredWidth: 120
                placeholderText: {
                    var _ = LanguageManager.currentLanguage
                    return qsTr("手动输入端口")
                }
                implicitHeight: 30
            }

            Button {
                text: {
                    var _ = LanguageManager.currentLanguage
                    return qsTr("添加")
                }
                implicitHeight: 32
                onClicked: {
                    AppController.addManualPort(leManualPort.text)
                    leManualPort.clear()
                }
            }
        }

        Button {
            text: {
                var _ = LanguageManager.currentLanguage
                return qsTr("刷新")
            }
            implicitHeight: 32
            visible: root.connectionType === 0
            onClicked: AppController.refreshPorts()
        }
    }
}
