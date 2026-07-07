import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

Rectangle {
    id: root
    height: 40
    color: "#ffffff"
    radius: 4
    border.color: "#e0e0e0"

    property string filePath: ""

    signal expandClicked()

    function tr(text) {
        var _ = LanguageManager.currentLanguage
        return qsTr(text)
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 4

        Button {
            text: tr("清空")
            implicitHeight: 28
            font.pixelSize: 12
            onClicked: AppController.clearDisplay()
        }

        Button {
            text: tr("保存")
            implicitHeight: 28
            font.pixelSize: 12
            onClicked: saveFileDialog.open()
        }

        Button {
            text: tr("打开文件")
            implicitHeight: 28
            font.pixelSize: 12
            onClicked: openFileDialog.open()
        }

        TextField {
            id: leFilePath
            Layout.fillWidth: true
            Layout.minimumWidth: 80
            text: root.filePath
            readOnly: true
            placeholderText: tr("选择文件...")
            implicitHeight: 28
            font.pixelSize: 11
        }

        Button {
            text: tr("发送文件")
            implicitHeight: 28
            font.pixelSize: 12
            enabled: root.filePath !== ""
            onClicked: AppController.loadAndSendFile(root.filePath)
        }

        Rectangle {
            width: 1
            height: parent.height * 0.6
            color: "#e0e0e0"
        }

        Button {
            text: tr("扩展")
            implicitHeight: 28
            font.pixelSize: 12
            background: Rectangle {
                color: "#4CAF50"
                radius: 4
            }
            contentItem: Text {
                text: parent.text
                color: "white"
                font.pixelSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: root.expandClicked()
        }
    }
}
