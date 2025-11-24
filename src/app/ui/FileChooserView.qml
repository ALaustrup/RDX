import QtQuick 6.0
import QtQuick.Controls 6.0
import Qt.labs.platform 1.1 as Platform

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
        }

        Button {
            text: isDecompress ? "Choose Archive..." : "Choose Files..."
            onClicked: {
                if (isDecompress) {
                    archiveDialog.open()
                } else {
                    fileDialog.open()
                }
            }
        }

        TextField {
            id: outputPathField
            Layout.fillWidth: true
            placeholderText: "Output path..."
        }

        RowLayout {
            Button {
                text: "OK"
                onClicked: {
                    if (isDecompress) {
                        compressionController.decompressArchive(archiveDialog.fileUrl, outputPathField.text)
                    } else {
                        var files = []
                        for (var i = 0; i < fileDialog.fileUrls.length; i++) {
                            files.push(fileDialog.fileUrls[i])
                        }
                        compressionController.compressFiles(files, outputPathField.text)
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

    Platform.FileDialog {
        id: fileDialog
        title: "Select Files"
        fileMode: Platform.FileDialog.OpenFiles
    }

    Platform.FileDialog {
        id: archiveDialog
        title: "Select Archive"
        nameFilters: ["RDX Archives (*.rdx)"]
        fileMode: Platform.FileDialog.OpenFile
    }
}

