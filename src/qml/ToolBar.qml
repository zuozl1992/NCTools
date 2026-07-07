import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

Rectangle {
    id: root
    height: 40
    color: Style.panelColor
    radius: Style.defaultRadius
    border.color: Style.borderColor

    property string filePath: ""

    RowLayout {
        anchors.fill: parent
        anchors.margins: Style.defaultSpacing
        spacing: Style.defaultSpacing

        Button {
            text: "清空"
            implicitHeight: Style.buttonHeight
            onClicked: AppController.clearDisplay()
        }

        Button {
            text: "刷新端口"
            implicitHeight: Style.buttonHeight
            onClicked: AppController.refreshPorts()
        }

        Button {
            text: "保存数据"
            implicitHeight: Style.buttonHeight
            onClicked: saveFileDialog.open()
        }

        Rectangle {
            width: 1
            height: parent.height * 0.6
            color: Style.borderColor
        }

        Button {
            text: "打开文件"
            implicitHeight: Style.buttonHeight
            onClicked: openFileDialog.open()
        }

        TextField {
            id: leFilePath
            Layout.fillWidth: true
            text: root.filePath
            readOnly: true
            placeholderText: "选择要发送的文件..."
            implicitHeight: Style.inputHeight
        }

        Button {
            text: "发送文件"
            implicitHeight: Style.buttonHeight
            enabled: root.filePath !== ""
            onClicked: AppController.loadAndSendFile(root.filePath)
        }

        Rectangle {
            width: 1
            height: parent.height * 0.6
            color: Style.borderColor
        }

        Button {
            text: quickSendPanel.visible ? "收起" : "扩展"
            implicitHeight: Style.buttonHeight
            onClicked: quickSendPanel.visible = !quickSendPanel.visible
        }
    }
}
