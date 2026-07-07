import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import MySSCOM 1.0

ApplicationWindow {
    id: root
    width: 500
    height: 400
    title: {
        var _ = LanguageManager.currentLanguage
        return qsTr("快速发送面板")
    }
    color: "#f5f5f5"

    function tr(text) {
        var _ = LanguageManager.currentLanguage
        return qsTr(text)
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 8
        color: "#ffffff"
        radius: 4
        border.color: "#e0e0e0"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 4

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 24
                spacing: 8

                Label {
                    text: "#"
                    Layout.preferredWidth: 30
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    text: tr("数据")
                    Layout.fillWidth: true
                    font.pixelSize: 11
                }

                Label {
                    text: "Hex"
                    Layout.preferredWidth: 40
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    text: tr("操作")
                    Layout.preferredWidth: 50
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            ScrollView {
                id: scrollView
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                ListView {
                    id: listView
                    width: scrollView.width - 20
                    model: AppController.quickSendModel
                    spacing: 4
                    clip: true

                    delegate: Item {
                        width: listView.width
                        height: 36

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 4
                            anchors.rightMargin: 4
                            spacing: 8

                            Label {
                                text: String(index + 1).padStart(2, '0')
                                Layout.preferredWidth: 30
                                font.pixelSize: 11
                                horizontalAlignment: Text.AlignHCenter
                            }

                            TextField {
                                id: leData
                                Layout.fillWidth: true
                                text: model.entryData
                                placeholderText: tr("输入数据") + (index + 1)
                                implicitHeight: 30
                                font.pixelSize: 12
                                onEditingFinished: AppController.quickSendModel.setEntryData(index, text)
                            }

                            CheckBox {
                                id: cbHex
                                Layout.preferredWidth: 40
                                checked: model.entryHex
                                onToggled: AppController.quickSendModel.setEntryHex(index, checked)
                            }

                            Button {
                                Layout.preferredWidth: 50
                                text: tr("发送")
                                implicitHeight: 30
                                font.pixelSize: 11
                                onClicked: AppController.sendQuickSendData(index)
                            }
                        }
                    }
                }
            }
        }
    }
}
