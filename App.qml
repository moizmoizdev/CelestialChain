import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: window
    width: 1280
    height: 720
    visible: true
    title: "CelestialChain Node"

    // Load your main screen .ui file from the resource system
    Loader {
        anchors.fill: parent
        source: "qrc:/qml/Screen01.ui.qml"
    }
}
