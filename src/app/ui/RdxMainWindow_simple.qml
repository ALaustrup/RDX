import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

ApplicationWindow {
    id: window
    width: 1200
    height: 800
    visible: true
    title: "RDX - Schema-Aware Compression"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Sidebar
        ColumnLayout {
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            Layout.margins: 10

            Button {
                Layout.fillWidth: true
                text: "Compress Files"
            }

            Button {
                Layout.fillWidth: true
                text: "Decompress"
            }

            Button {
                Layout.fillWidth: true
                text: "Corpus Dashboard"
            }

            Button {
                Layout.fillWidth: true
                text: "Settings"
            }
        }

        // Main content
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 10

            Text {
                text: "Compression Jobs"
                font.pixelSize: 20
                font.bold: true
            }

            Text {
                text: "No jobs yet. Use the buttons to compress or decompress files."
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}

