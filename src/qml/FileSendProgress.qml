import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

Rectangle {
    id: root
    height: visible ? 30 : 0
    color: Style.panelColor
    radius: Style.defaultRadius
    border.color: Style.borderColor
    visible: progressBar.value > 0

    RowLayout {
        anchors.fill: parent
        anchors.margins: Style.defaultSpacing
        spacing: Style.defaultSpacing

        Label {
            text: "文件发送进度:"
            font: Style.smallFont
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
            font: Style.smallFont
        }
    }

    Connections {
        target: AppController
        function onFileProgressChanged(percent) {
            progressBar.value = percent
        }
    }
}
