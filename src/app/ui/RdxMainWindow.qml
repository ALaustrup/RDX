import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import RDX 1.0

ApplicationWindow {
    id: window
    width: 1400
    height: 900
    visible: true
    title: "RDX - Schema-Aware Compression"

    // Dark theme colors
    readonly property color bgDark: "#1a1a1a"
    readonly property color bgMedium: "#252525"
    readonly property color bgLight: "#2d2d2d"
    readonly property color accentBlue: "#00a8ff"
    readonly property color accentPurple: "#9c27b0"
    readonly property color accentGreen: "#4caf50"
    readonly property color accentOrange: "#ff9800"
    readonly property color textPrimary: "#ffffff"
    readonly property color textSecondary: "#b0b0b0"
    readonly property color textTertiary: "#808080"

    color: bgDark

    menuBar: MenuBar {
        Menu {
            title: "File"
            Action {
                text: "Compress Files..."
                shortcut: "Ctrl+C"
                onTriggered: fileChooserView.visible = true
            }
            Action {
                text: "Decompress Archive..."
                shortcut: "Ctrl+D"
                onTriggered: {
                    fileChooserView.isDecompress = true
                    fileChooserView.visible = true
                }
            }
            MenuSeparator {}
            Action {
                text: "Exit"
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: "View"
            Action {
                text: "Corpus Dashboard"
                shortcut: "Ctrl+Shift+C"
                onTriggered: corpusDashboardView.visible = true
            }
            Action {
                text: "Settings"
                shortcut: "Ctrl+,"
                onTriggered: settingsView.visible = true
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left Sidebar
        Rectangle {
            Layout.preferredWidth: 240
            Layout.fillHeight: true
            color: bgMedium
            border.color: accentBlue
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                // Logo
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                    color: "transparent"
                    border.color: accentPurple
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: "RDX"
                        font.pixelSize: 28
                        font.bold: true
                        color: accentBlue
                    }
                }

                // Compress button
                Rectangle {
                    id: compressBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: compressBtnMouseArea.containsMouse ? bgLight : bgMedium
                    border.color: compressBtnMouseArea.containsMouse ? accentGreen : accentBlue
                    border.width: 1
                    radius: 4

                    Text {
                        anchors.centerIn: parent
                        text: "📦 Compress"
                        font.pixelSize: 14
                        font.bold: true
                        color: textPrimary
                    }

                    MouseArea {
                        id: compressBtnMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            fileChooserView.isDecompress = false
                            fileChooserView.inputPathField.text = ""
                            fileChooserView.outputPathField.text = ""
                            fileChooserView.visible = true
                        }
                    }
                }

                // Decompress button
                Rectangle {
                    id: decompressBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: decompressBtnMouseArea.containsMouse ? bgLight : bgMedium
                    border.color: decompressBtnMouseArea.containsMouse ? accentOrange : accentPurple
                    border.width: 1
                    radius: 4

                    Text {
                        anchors.centerIn: parent
                        text: "📂 Decompress"
                        font.pixelSize: 14
                        font.bold: true
                        color: textPrimary
                    }

                    MouseArea {
                        id: decompressBtnMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            fileChooserView.isDecompress = true
                            fileChooserView.inputPathField.text = ""
                            fileChooserView.outputPathField.text = ""
                            fileChooserView.visible = true
                        }
                    }
                }

                Item { Layout.fillHeight: true }

                // Dashboard button
                Rectangle {
                    id: dashboardBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    color: dashboardBtnMouseArea.containsMouse ? bgLight : bgMedium
                    border.color: "#00bcd4"
                    border.width: 1
                    radius: 4

                    Text {
                        anchors.centerIn: parent
                        text: "📊 Dashboard"
                        font.pixelSize: 13
                        color: textSecondary
                    }

                    MouseArea {
                        id: dashboardBtnMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: corpusDashboardView.visible = true
                    }
                }

                // Settings button
                Rectangle {
                    id: settingsBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    color: settingsBtnMouseArea.containsMouse ? bgLight : bgMedium
                    border.color: textTertiary
                    border.width: 1
                    radius: 4

                    Text {
                        anchors.centerIn: parent
                        text: "⚙️ Settings"
                        font.pixelSize: 13
                        color: textSecondary
                    }

                    MouseArea {
                        id: settingsBtnMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: settingsView.visible = true
                    }
                }
            }
        }

        // Main content
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: bgDark
            border.color: accentGreen
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                // Header
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: bgMedium
                    border.color: accentBlue
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12

                        Text {
                            text: "Compression Jobs"
                            font.pixelSize: 20
                            font.bold: true
                            color: textPrimary
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            text: jobViewModel ? jobViewModel.rowCount + " job(s)" : "0 job(s)"
                            font.pixelSize: 12
                            color: textSecondary
                        }
                    }
                }

                // Job list
                JobListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }

    // File Chooser Dialog
    Dialog {
        id: fileChooserView
        title: "Select Files to Compress"
        width: 700
        height: 500
        visible: false
        modal: true

        property bool isDecompress: false

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            Text {
                text: fileChooserView.isDecompress ? "📂 Decompress Archive" : "📦 Compress Files"
                font.pixelSize: 20
                font.bold: true
                color: accentBlue
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: accentBlue
            }

            Text {
                text: "Input:"
                font.pixelSize: 13
                font.bold: true
                color: textSecondary
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                TextField {
                    id: inputPathField
                    Layout.fillWidth: true
                    placeholderText: fileChooserView.isDecompress ? "Path to .rdx archive..." : "Path to file(s) to compress..."
                    color: textPrimary
                }

                Button {
                    text: "Browse..."
                    Layout.preferredWidth: 100
                    onClicked: {
                        console.log("File browser - to be implemented via C++ backend")
                    }
                }
            }

            Text {
                text: "Output:"
                font.pixelSize: 13
                font.bold: true
                color: textSecondary
            }

            TextField {
                id: outputPathField
                Layout.fillWidth: true
                placeholderText: fileChooserView.isDecompress ? "Extract to folder..." : "Save archive as..."
                color: textPrimary
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Item { Layout.fillWidth: true }

                Button {
                    text: "Cancel"
                    Layout.preferredWidth: 100
                    onClicked: fileChooserView.close()
                }

                Button {
                    text: "OK"
                    Layout.preferredWidth: 100
                    enabled: inputPathField.text !== "" && outputPathField.text !== ""
                    onClicked: {
                        if (fileChooserView.isDecompress) {
                            if (compressionController) {
                                compressionController.decompressArchive(inputPathField.text, outputPathField.text)
                            }
                        } else {
                            if (compressionController) {
                                var files = [inputPathField.text]
                                compressionController.compressFiles(files, outputPathField.text)
                            }
                        }
                        fileChooserView.close()
                    }
                }
            }
        }
    }

    // Corpus Dashboard
    Dialog {
        id: corpusDashboardView
        title: "Lifetime Corpus Model Dashboard"
        width: 800
        height: 600
        visible: false
        modal: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            Text {
                text: "📊 Corpus Statistics"
                font.pixelSize: 22
                font.bold: true
                color: "#00bcd4"
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#00bcd4"
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                columnSpacing: 16
                rowSpacing: 12

                Text {
                    text: "Total Files:"
                    font.pixelSize: 14
                    color: textSecondary
                }
                Text {
                    text: (lcmStatsViewModel ? lcmStatsViewModel.totalFiles : 0).toLocaleString()
                    font.pixelSize: 14
                    font.bold: true
                    color: textPrimary
                }

                Text {
                    text: "Corpus Size:"
                    font.pixelSize: 14
                    color: textSecondary
                }
                Text {
                    text: (lcmStatsViewModel ? (lcmStatsViewModel.totalCorpusSize / 1024.0 / 1024.0).toFixed(2) : "0.00") + " MB"
                    font.pixelSize: 14
                    font.bold: true
                    color: textPrimary
                }
            }

            Text {
                text: "Top Schemas:"
                font.pixelSize: 14
                font.bold: true
                color: accentPurple
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                color: bgDark
                border.color: accentPurple
                border.width: 1
                radius: 4

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 8
                    contentWidth: width
                    clip: true

                    Text {
                        text: lcmStatsViewModel ? lcmStatsViewModel.topSchemas : "N/A"
                        font.pixelSize: 12
                        color: textSecondary
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Text {
                text: "Top File Types:"
                font.pixelSize: 14
                font.bold: true
                color: accentOrange
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                color: bgDark
                border.color: accentOrange
                border.width: 1
                radius: 4

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 8
                    contentWidth: width
                    clip: true

                    Text {
                        text: lcmStatsViewModel ? lcmStatsViewModel.topFileTypes : "N/A"
                        font.pixelSize: 12
                        color: textSecondary
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Item { Layout.fillWidth: true }

                Button {
                    text: "Refresh"
                    Layout.preferredWidth: 100
                    onClicked: {
                        if (lcmStatsViewModel) {
                            lcmStatsViewModel.refresh()
                        }
                    }
                }

                Button {
                    text: "Close"
                    Layout.preferredWidth: 100
                    onClicked: corpusDashboardView.close()
                }
            }
        }
    }

    // Settings Dialog
    Dialog {
        id: settingsView
        title: "Settings"
        width: 550
        height: 450
        visible: false
        modal: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            Text {
                text: "⚙️ RDX Settings"
                font.pixelSize: 20
                font.bold: true
                color: textPrimary
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: textTertiary
            }

            Text {
                text: "LCM Database Path:"
                font.pixelSize: 13
                color: textSecondary
            }

            TextField {
                id: lcmPathField
                Layout.fillWidth: true
                readOnly: true
                text: "Default location"
                color: textTertiary
            }

            Text {
                text: "Compression Level:"
                font.pixelSize: 13
                color: textSecondary
            }

            SpinBox {
                id: compressionLevel
                from: 1
                to: 9
                value: 3
                editable: true
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true

                Item { Layout.fillWidth: true }

                Button {
                    text: "Close"
                    Layout.preferredWidth: 100
                    onClicked: settingsView.close()
                }
            }
        }
    }
}
