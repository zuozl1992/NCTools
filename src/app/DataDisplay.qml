import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

Rectangle {
    id: root
    color: "#ffffff"
    radius: 4
    border.color: "#e0e0e0"

    ScrollView {
        id: scrollView
        anchors.fill: parent
        anchors.margins: 1

        TextArea {
            id: textArea
            readOnly: true
            wrapMode: TextArea.Wrap
            text: AppController.displayText
            font: Qt.font({ pixelSize: 13 })
            color: "#333333"
            selectByMouse: true
        }
    }

    // Scroll to bottom when text changes
    Connections {
        target: AppController
        function onDisplayTextChanged() {
            if (AppController.displayText.length > 0) {
                scrollView.ScrollBar.vertical.position = 1.0 - scrollView.ScrollBar.vertical.size
            }
        }
    }
}
