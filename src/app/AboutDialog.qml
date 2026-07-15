import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

Dialog {
    id: root
    title: {
        var _ = LanguageManager.currentLanguage
        return qsTr("关于 NCTools")
    }
    modal: true
    standardButtons: Dialog.Ok
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    function tr(text) {
        var _ = LanguageManager.currentLanguage
        return qsTr(text)
    }

    contentItem: ColumnLayout {
        spacing: 16

        Image {
            Layout.alignment: Qt.AlignHCenter
            source: "qrc:/NCTools/control.png"
            sourceSize: Qt.size(64, 64)
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "NCTools"
            font.pixelSize: 18
            font.bold: true
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: tr("版本 2.0")
            font.pixelSize: 13
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: tr("串口网络调试工具")
            font.pixelSize: 13
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: tr("支持串口、TCP Server、TCP Client、UDP 通信")
            font.pixelSize: 13
            color: "#666666"
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "Copyright © 2024"
            font.pixelSize: 11
            color: "#666666"
        }
    }
}
