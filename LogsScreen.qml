import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: logsScreen
    color: "white"
    anchors.fill: parent

    Text {
        anchors.centerIn: parent
        text: "Logs / Debug Output"
        font.pixelSize: 30
        color: "black"
    }
}
