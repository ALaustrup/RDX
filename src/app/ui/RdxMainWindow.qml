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
                onClicked: fileChooserView.visible = true
            }

            Button {
                Layout.fillWidth: true
                text: "Decompress"
                onClicked: {
                    // Open file dialog for archive
                    fileChooserView.visible = true
                }
            }

            Button {
                Layout.fillWidth: true
                text: "Corpus Dashboard"
                onClicked: corpusDashboardView.visible = true
            }

            Button {
                Layout.fillWidth: true
                text: "Settings"
                onClicked: settingsView.visible = true
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

            JobListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    // File Chooser Dialog
    FileChooserView {
        id: fileChooserView
        visible: false
    }

    // Corpus Dashboard
    CorpusDashboardView {
        id: corpusDashboardView
        visible: false
    }

    // Settings View
    SettingsView {
        id: settingsView
        visible: false
    }
}

