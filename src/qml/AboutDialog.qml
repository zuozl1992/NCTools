import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

Dialog {
    id: root
    title: "关于 MySSCOM"
    modal: true
    standardButtons: Dialog.Ok
    anchors.centerIn: parent

    contentItem: ColumnLayout {
        spacing: 16

        Image {
            Layout.alignment: Qt.AlignHCenter
            source: "qrc:/control.png"
            sourceSize: Qt.size(64, 64)
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "MySSCOM"
            font.pixelSize: 18
            font.bold: true
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "版本 2.0"
            font: Style.defaultFont
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "串口网络调试工具"
            font: Style.defaultFont
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "支持串口、TCP Server、TCP Client、UDP 通信"
            font: Style.defaultFont
            color: Style.textSecondaryColor
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "Copyright © 2024"
            font: Style.smallFont
            color: Style.textSecondaryColor
        }
    }
}
