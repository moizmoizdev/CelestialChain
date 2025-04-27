import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: dashboard
    color: "white"
    anchors.fill: parent

    GridLayout {
        columns: 2
        anchors.fill: parent
        anchors.margins: 10
        rowSpacing: 10
        columnSpacing: 10

        // Blockchain View
        GroupBox {
            title: "Blockchain"
            Layout.preferredWidth: parent.width / 2
            Layout.preferredHeight: parent.height / 2

            ListView {
                anchors.fill: parent
                model: ListModel {
                    ListElement { blockInfo: "Block #1: Hash XYZ123" }
                    ListElement { blockInfo: "Block #2: Hash ABC456" }
                    ListElement { blockInfo: "Block #3: Hash LMN789" }
                }
                delegate: Text {
                    text: blockInfo
                    font.pixelSize: 14
                    padding: 5
                }
            }
        }

        // Mempool View
        GroupBox {
            title: "Mempool"
            Layout.preferredWidth: parent.width / 2
            Layout.preferredHeight: parent.height / 2

            ListView {
                anchors.fill: parent
                model: ListModel {
                    ListElement { txInfo: "Tx1: Alice → Bob (5 coins)" }
                    ListElement { txInfo: "Tx2: Dave → Charlie (2 coins)" }
                }
                delegate: Text {
                    text: txInfo
                    font.pixelSize: 14
                    padding: 5
                }
            }
        }

        // Wallet Info
        GroupBox {
            title: "Wallet"
            Layout.preferredWidth: parent.width / 2
            Layout.preferredHeight: parent.height / 2

            Column {
                anchors.fill: parent
                spacing: 8
                padding: 8

                Text { text: "Address: 0x1234abcd..."; font.pixelSize: 14 }
                Text { text: "Balance: 100 CelestialCoins"; font.pixelSize: 14 }
            }
        }

        // Logs View
        GroupBox {
            title: "Logs"
            Layout.preferredWidth: parent.width / 2
            Layout.preferredHeight: parent.height / 2

            ScrollView {
                anchors.fill: parent

                TextArea {
                    id: logsArea
                    text: "System ready...\nConnected to 3 peers...\nBlock mined successfully!"
                    readOnly: true
                    wrapMode: TextEdit.Wrap
                }
            }
        }
    }

    Rectangle {
        id: dashboard1
        color: "#ffffff"
        anchors.fill: parent
        GridLayout {
            anchors.fill: parent
            anchors.margins: 10
            rowSpacing: 10
            GroupBox {
                title: "Blockchain"
                ListView {
                    anchors.fill: parent
                    model: ListModel {
                        ListElement {
                            blockInfo: "Block #1: Hash XYZ123"
                        }

                        ListElement {
                            blockInfo: "Block #2: Hash ABC456"
                        }

                        ListElement {
                            blockInfo: "Block #3: Hash LMN789"
                        }
                    }
                    delegate: Text {
                        text: blockInfo
                        font.pixelSize: 14
                        padding: 5
                    }
                }
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height / 2
            }

            GroupBox {
                title: "Mempool"
                ListView {
                    anchors.fill: parent
                    model: ListModel {
                        ListElement {
                            txInfo: "Tx1: Alice → Bob (5 coins)"
                        }

                        ListElement {
                            txInfo: "Tx2: Dave → Charlie (2 coins)"
                        }
                    }
                    delegate: Text {
                        text: txInfo
                        font.pixelSize: 14
                        padding: 5
                    }
                }
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height / 2
            }

            GroupBox {
                title: "Wallet"
                Column {
                    anchors.fill: parent
                    spacing: 8
                    padding: 8
                    Text {
                        text: "Address: 0x1234abcd..."
                        font.pixelSize: 14
                    }

                    Text {
                        text: "Balance: 100 CelestialCoins"
                        font.pixelSize: 14
                    }
                }
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height / 2
            }

            GroupBox {
                title: "Logs"
                ScrollView {
                    anchors.fill: parent
                    TextArea {
                        id: logsArea1
                        text: "System ready...\nConnected to 3 peers...\nBlock mined successfully!"
                        wrapMode: TextEdit.Wrap
                        readOnly: true
                    }
                }
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height / 2
            }
            columns: 2
            columnSpacing: 10
        }
    }
}
