import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: root
    width: 1200
    height: 800
    visible: true
    title: "LINA - QML"
    color: "#1A1A1A"

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        // Top bar
        Rectangle {
            height: 56
            color: "#2C3E50"
            Layout.fillWidth: true
            Row {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 12
                Text { text: "LINA"; color: "#ECF0F1"; font.bold: true; font.pointSize: 18 }
                Item { Layout.fillWidth: true }
                Button { text: "Start"; onClicked: bridge.startCollection(); }
                Button { text: "Stop"; onClicked: bridge.stopCollection(); }
            }
        }

        // Main area
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Left panel
            Rectangle {
                width: 320
                color: "#1A1A1A"
                border.color: "#3498DB"
                Layout.fillHeight: true
                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8
                    Text { text: "Status: Idle"; color: "#ECF0F1" }
                    TextArea { id: logs; readOnly: true; wrapMode: TextArea.Wrap; font.family: "monospace"; color: "#ECF0F1"; background: Rectangle { color: "#0D0D0D" } }
                }
            }

            // Right panel
            Rectangle {
                color: "#1A1A1A"
                Layout.fillWidth: true
                Layout.fillHeight: true
                Column {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8
                    TextArea { id: chat; readOnly: true; color: "#ECF0F1"; background: Rectangle { color: "#0D0D0D" } ; Layout.fillHeight: true }
                    Row {
                        TextField { id: input; placeholderText: "Type your question..."; Layout.fillWidth: true }
                        Button { text: "Send"; onClicked: {
                            var resp = bridge.sendQuery(input.text);
                            chat.append("You: " + input.text + "\n");
                            chat.append("LINA: " + resp + "\n\n");
                            input.text = "";
                        } }
                    }
                }
            }
        }
    }
}
