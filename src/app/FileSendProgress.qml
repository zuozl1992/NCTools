import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

Rectangle {
    id: root
    height: visible ? 30 : 0
    color: "#ffffff"
    radius: 4
    border.color: "#e0e0e0"
    visible: sending || progressBar.value > 0

    property bool sending: false

    function tr(text) {
        var _ = LanguageManager.currentLanguage
        return qsTr(text)
    }

    Timer {
        id: hideTimer
        interval: 5000
        onTriggered: {
            progressBar.value = 0
            root.sending = false
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Label {
            text: tr("文件发送进度:")
            font.pixelSize: 11
        }

        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            from: 0
            to: 100
            value: 0
        }

        Label {
            text: Math.round(progressBar.value) + "%"
            font.pixelSize: 11
        }
    }

    Connections {
        target: AppController
        function onFileProgressChanged(percent) {
            if (percent >= 0 && percent <= 100) {
                root.sending = true
                progressBar.value = percent
                if (percent >= 100) {
                    hideTimer.start()
                } else {
                    hideTimer.stop()
                }
            }
        }
    }
}
