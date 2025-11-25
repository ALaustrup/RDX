import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import RDX 1.0

ApplicationWindow {
    id: window
    width: 1200
    height: 800
    visible: true
    title: "RDX - Schema-Aware Compression"

    menuBar: MenuBar {
        Menu {
            title: "File"
            Action {
                text: "Compress Files..."
                onTriggered: fileChooserView.visible = true
            }
            Action {
                text: "Decompress Archive..."
                onTriggered: fileChooserView.visible = true
            }
            MenuSeparator {}
            Action {
                text: "Exit"
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: "View"
            Action {
                text: "Corpus Dashboard"
                onTriggered: corpusDashboardView.visible = true
            }
            Action {
                text: "Settings"
                onTriggered: settingsView.visible = true
            }
        }
    }

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
                onClicked: {
                    console.log("Compress Files - FileChooserView disabled for now");
                }
            }

            Button {
                Layout.fillWidth: true
                text: "Decompress"
                onClicked: {
                    console.log("Decompress - FileChooserView disabled for now");
                }
            }

            Button {
                Layout.fillWidth: true
                text: "Corpus Dashboard"
                onClicked: {
                    corpusDashboardView.visible = true;
                }
            }

            Button {
                Layout.fillWidth: true
                text: "Settings"
                onClicked: {
                    settingsView.visible = true;
                }
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
                color: "#333333"
            }

            Text {
                text: "No compression jobs yet.\n\nUse the buttons on the left to:\n• Compress files into .rdx archives\n• Decompress .rdx archives\n• View corpus statistics\n• Access settings"
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#666666"
                font.pixelSize: 14
                wrapMode: Text.WordWrap
            }
        }
    }

    // File Chooser Dialog - disabled for now
    // FileChooserView {
    //     id: fileChooserView
    //     visible: false
    // }

    // Corpus Dashboard - simplified version
    Dialog {
        id: corpusDashboardView
        title: "Lifetime Corpus Model Dashboard"
        width: 700
        height: 500
        visible: false
        modal: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20

            Text {
                text: "Corpus Statistics"
                font.pixelSize: 18
                font.bold: true
            }

            Text {
                text: "Total Files Tracked: " + (lcmStatsViewModel ? lcmStatsViewModel.totalFiles : 0)
                font.pixelSize: 14
            }

            Text {
                text: "Total Corpus Size: " + (lcmStatsViewModel ? (lcmStatsViewModel.totalCorpusSize / 1024.0 / 1024.0).toFixed(2) : "0.00") + " MB"
                font.pixelSize: 14
            }

            Text {
                text: "Top Schemas:"
                font.pixelSize: 14
                font.bold: true
            }

            Text {
                text: lcmStatsViewModel ? lcmStatsViewModel.topSchemas : "N/A"
                font.pixelSize: 12
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Text {
                text: "Top File Types:"
                font.pixelSize: 14
                font.bold: true
            }

            Text {
                text: lcmStatsViewModel ? lcmStatsViewModel.topFileTypes : "N/A"
                font.pixelSize: 12
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Button {
                text: "Refresh"
                onClicked: {
                    if (lcmStatsViewModel) {
                        lcmStatsViewModel.refresh();
                    }
                }
            }

            Button {
                text: "Close"
                onClicked: corpusDashboardView.close()
            }
        }
    }

    // Settings View - using a simple version for now
    Dialog {
        id: settingsView
        title: "Settings"
        width: 500
        height: 400
        visible: false
        modal: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20

            Text {
                text: "RDX Settings"
                font.pixelSize: 18
                font.bold: true
            }

            Text {
                text: "LCM Database Path:"
                font.pixelSize: 12
            }

            TextField {
                id: lcmPathField
                Layout.fillWidth: true
                readOnly: true
                text: "Default location"
            }

            Text {
                text: "Compression Level:"
                font.pixelSize: 12
            }

            SpinBox {
                id: compressionLevel
                from: 1
                to: 9
                value: 3
                editable: true
            }

            Button {
                text: "Close"
                onClicked: settingsView.close()
            }
        }
    }
}

