import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: consoleScreen
    anchors.fill: parent
    color: "white"

    Text {
        text: "Console Screen Loaded"
        color: "green"
        font.bold: true
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible: true // Keep visible for debugging
    }

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // Title
        Text {
            text: "Node Console"
            font.pixelSize: 24
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Menu Listing - Fixed Height
        ScrollView {
            width: parent.width
            height: parent.height * 0.4 // Take 40% of available height
            
            TextArea {
                id: menuArea
                text: 
                    "1. View blockchain\n" +
                    "2. View mempool\n" +
                    "3. Mine block\n" +
                    "4. Create transaction\n" +
                    "5. View wallet\n" +
                    "6. Connect to peer\n" +
                    "7. Request blockchain\n" +
                    "8. View peers\n" +
                    "9. View stats\n" +
                    "10. Explorer\n" +
                    "0. Exit\n\n" +
                    "Enter choice:"
                readOnly: true
                wrapMode: Text.Wrap
            }
        }

        // Input Row - Fixed Height
        Row {
            width: parent.width
            height: 40
            spacing: 10
            
            TextField { // Changed from TextInput to TextField for better visuals
                id: choiceInput
                placeholderText: "Enter choice (0-10)"
                width: 200
            }
            
            Button {
                text: "OK"
                width: 80
                onClicked: {
                    consoleOutput.text += "\n> You entered: " + choiceInput.text
                    choiceInput.text = ""
                }
            }
        }

        // Output Area - Takes remaining space
        ScrollView {
            width: parent.width
            height: parent.height * 0.5 // Take 50% of remaining height
            
            TextArea {
                id: consoleOutput
                text: ">> Ready.\n"
                readOnly: true
                wrapMode: Text.Wrap
            }
        }
    }
}