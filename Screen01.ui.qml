import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: mainScreen
    width: 1280; height: 720

    // ─── NAVBAR ──────────────────────────────────────────────────────────
    Rectangle {
        id: navbar
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 60
        color: "#222831"
        Text {
            anchors.centerIn: parent
            text: "CelestialChain Node"
            font.pixelSize: 24
            color: "white"
        }
    }

    // ─── SIDEBAR ─────────────────────────────────────────────────────────
    Rectangle {
        id: sidebar
        anchors { top: navbar.bottom; left: parent.left; bottom: parent.bottom }
        width: 220
        color: "#393E46"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10

            Button { text: "Dashboard"; onClicked: stackView.push("Dashboard.qml") }
            Button { text: "Blockchain"; onClicked: stackView.push("BlockchainScreen.qml") }
            Button { text: "Mempool"; onClicked: stackView.push("MempoolScreen.qml") }
            Button { text: "Peers"; onClicked: stackView.push("PeersScreen.qml") }
            Button { text: "Wallet"; onClicked: stackView.push("WalletScreen.qml") }
            Button { text: "Explorer"; onClicked: stackView.push("ExplorerScreen.qml") }
            Button { text: "Logs"; onClicked: stackView.push("LogsScreen.qml") }
            Button { text: "Console"; onClicked: stackView.push("ConsoleScreen.qml") }
        }
    }

    // ─── MAIN CONTENT AREA ────────────────────────────────────────────────
    StackView {
        id: stackView
        anchors {
            top:    navbar.bottom
            left:   sidebar.right
            right:  parent.right
            bottom: parent.bottom
            margins: 0
        }
        // Show Dashboard first
        initialItem: "Dashboard.qml"
    }
}
