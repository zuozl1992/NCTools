import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import NCTools 1.0

Rectangle {
    id: root
    color: Style.panelColor
    radius: Style.defaultRadius
    border.color: Style.borderColor

    ScrollView {
        id: scrollView
        anchors.fill: parent
        anchors.margins: 1

        TextArea {
            id: textArea
            readOnly: true
            wrapMode: TextArea.Wrap
            text: AppController.displayText
            font: Style.defaultFont
            color: Style.textColor
            selectByMouse: true

            onTextChanged: {
                // Auto-scroll to bottom
                scrollView.ScrollBar.vertical.position = 1.0 - scrollView.ScrollBar.vertical.size
            }
        }
    }
}
