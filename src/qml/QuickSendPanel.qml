import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

Rectangle {
    id: root
    height: 300
    color: Style.panelColor
    radius: Style.defaultRadius
    border.color: Style.borderColor

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.defaultSpacing
        spacing: 2

        // Header
        RowLayout {
            Layout.fillWidth: true
            spacing: Style.defaultSpacing

            Label {
                text: "#"
                Layout.preferredWidth: 30
                font: Style.smallFont
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                text: "数据"
                Layout.fillWidth: true
                font: Style.smallFont
            }

            Label {
                text: "Hex"
                Layout.preferredWidth: 40
                font: Style.smallFont
                horizontalAlignment: Text.AlignHCenter
            }

            Label {
                text: "操作"
                Layout.preferredWidth: 50
                font: Style.smallFont
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // Quick send entries
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: listView
                model: AppController.quickSendModel
                spacing: 2

                delegate: RowLayout {
                    width: listView.width
                    spacing: Style.defaultSpacing

                    Label {
                        text: String(index + 1).padStart(2, '0')
                        Layout.preferredWidth: 30
                        font: Style.smallFont
                        horizontalAlignment: Text.AlignHCenter
                    }

                    TextField {
                        id: leData
                        Layout.fillWidth: true
                        text: model.entryData
                        placeholderText: "输入数据" + (index + 1)
                        implicitHeight: Style.inputHeight
                        onEditingFinished: AppController.quickSendModel.setEntryData(index, text)
                    }

                    CheckBox {
                        id: cbHex
                        Layout.preferredWidth: 40
                        checked: model.entryHex
                        onCheckedChanged: AppController.quickSendModel.setEntryHex(index, checked)
                    }

                    Button {
                        Layout.preferredWidth: 50
                        text: "发送"
                        implicitHeight: Style.buttonHeight
                        onClicked: AppController.sendQuickSendData(index)
                    }
                }
            }
        }
    }
}
