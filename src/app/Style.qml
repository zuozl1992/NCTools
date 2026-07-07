import QtQuick 2.15

QtObject {
    // Colors
    readonly property color backgroundColor: "#f5f5f5"
    readonly property color panelColor: "#ffffff"
    readonly property color accentColor: "#2196F3"
    readonly property color accentDarkColor: "#1976D2"
    readonly property color textColor: "#333333"
    readonly property color textSecondaryColor: "#666666"
    readonly property color borderColor: "#e0e0e0"
    readonly property color successColor: "#4CAF50"
    readonly property color errorColor: "#f44336"
    readonly property color warningColor: "#ff9800"

    // Fonts
    readonly property font defaultFont: Qt.font({ pixelSize: 13 })
    readonly property font headerFont: Qt.font({ pixelSize: 14, weight: Font.Bold })
    readonly property font smallFont: Qt.font({ pixelSize: 11 })

    // Sizes
    readonly property int defaultRadius: 4
    readonly property int defaultSpacing: 8
    readonly property int panelPadding: 12
    readonly property int buttonHeight: 32
    readonly property int inputHeight: 30
}
