import QtQuick 6.0
import QtQuick.Controls 6.0

Dialog {
    id: fileChooserDialog
    title: "Select Files to Compress"
    width: 600
    height: 400
    modal: true

    property bool isDecompress: false

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        Text {
            text: isDecompress ? "Select Archive to Decompress" : "Select Files to Compress"
            font.pixelSize: 16
            font.bold: true
            color: "#333333"
        }

        Text {
            text: "File Path:"
            font.pixelSize: 12
            color: "#333333"
        }

        TextField {
            id: inputPathField
            Layout.fillWidth: true
            placeholderText: isDecompress ? "Enter path to .rdx archive..." : "Enter path to file(s) to compress..."
        }

        Button {
            text: "Browse..."
            onClicked: {
                // TODO: Implement native file dialog via C++ backend
                console.log("File browser - to be implemented via C++ backend")
            }
        }


        Text {
            text: "Output Path:"
            font.pixelSize: 12
            color: "#333333"
        }

        TextField {
            id: outputPathField
            Layout.fillWidth: true
            placeholderText: isDecompress ? "Extract to folder..." : "Save archive as..."
        }

        RowLayout {
            Button {
                text: "OK"
                enabled: inputPathField.text !== "" && outputPathField.text !== ""
                onClicked: {
                    if (isDecompress) {
                        if (compressionController) {
                            compressionController.decompressArchive(inputPathField.text, outputPathField.text)
                        }
                    } else {
                        if (compressionController) {
                            var files = [inputPathField.text]
                            compressionController.compressFiles(files, outputPathField.text)
                        }
                    }
                    fileChooserDialog.close()
                }
            }

            Button {
                text: "Cancel"
                onClicked: fileChooserDialog.close()
            }
        }
    }

    // File dialogs will be handled via C++ backend
    // For now, use text input for file paths
    Component.onCompleted: {
        // Initialize
    }
}

