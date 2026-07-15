import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

Rectangle {
    id: root
    height: 120
    color: Style.panelColor
    radius: Style.defaultRadius
    border.color: Style.borderColor

    property alias sendText: teSendData.text

    RowLayout {
        anchors.fill: parent
        anchors.margins: Style.defaultSpacing
        spacing: Style.defaultSpacing

        // Send text area
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextArea {
                id: teSendData
                wrapMode: TextArea.Wrap
                placeholderText: "输入要发送的数据..."
                font: Style.defaultFont
                color: Style.textColor

                onTextChanged: {
                    if (AppController.settings.hexSend) {
                        // Filter non-hex characters
                        var pos = cursorPosition - 1
                        if (pos >= 0) {
                            var ch = text.charAt(pos)
                            if (!/[0-9A-Fa-f ]/.test(ch)) {
                                remove(pos, pos + 1)
                            }
                        }
                    }
                }
            }
        }

        // Send button
        Button {
            Layout.preferredWidth: 80
            Layout.fillHeight: true
            text: "发送"
            background: Rectangle {
                color: parent.pressed ? Style.accentDarkColor : Style.accentColor
                radius: Style.defaultRadius
            }
            contentItem: Text {
                text: parent.text
                color: "white"
                font: Style.headerFont
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                AppController.sendData(teSendData.text, AppController.settings.hexSend)
                if (AppController.settings.autoClearSend) {
                    teSendData.clear()
                }
            }
        }
    }

    // Auto-send connection
    Connections {
        target: AppController
        function onSendStatusChanged(ok) {
            if (AppController.settings.autoSend && ok) {
                AppController.sendData(teSendData.text, AppController.settings.hexSend)
                if (AppController.settings.autoClearSend) {
                    teSendData.clear()
                }
            }
        }
    }
}
