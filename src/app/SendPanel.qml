import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

Rectangle {
    id: root
    height: 120
    color: "#ffffff"
    radius: 4
    border.color: "#e0e0e0"

    property alias sendText: teSendData.text

    function tr(text) {
        var _ = LanguageManager.currentLanguage
        return qsTr(text)
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextArea {
                id: teSendData
                wrapMode: TextArea.Wrap
                placeholderText: AppController.settings.hexSend ? tr("输入HEX数据，如: 48 65 6C 6C 6F") : tr("输入要发送的数据...")
                font.pixelSize: 13
                color: "#333333"

                onTextChanged: {
                    if (AppController.settings.hexSend) {
                        var cursorPos = cursorPosition;
                        var filtered = text.replace(/[^0-9A-Fa-f\s]/g, '');
                        filtered = filtered.toUpperCase();
                        var formatted = '';
                        var count = 0;
                        for (var i = 0; i < filtered.length; i++) {
                            if (filtered[i] === ' ' || filtered[i] === '\n') {
                                if (count > 0) {
                                    formatted += ' ';
                                    count = 0;
                                }
                            } else {
                                formatted += filtered[i];
                                count++;
                                if (count === 2 && i < filtered.length - 1) {
                                    formatted += ' ';
                                    count = 0;
                                }
                            }
                        }

                        if (text !== formatted) {
                            text = formatted;
                            cursorPosition = Math.min(cursorPos, text.length);
                        }
                    }
                }
            }
        }

        Button {
            Layout.preferredWidth: 80
            Layout.fillHeight: true
            text: tr("发送")
            background: Rectangle {
                color: parent.pressed ? "#1976D2" : "#2196F3"
                radius: 4
            }
            contentItem: Text {
                text: parent.text
                color: "white"
                font.pixelSize: 14
                font.bold: true
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
}
