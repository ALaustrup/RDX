import QtQuick 2.15
import QtQuick.Controls 2.15
import RDX 1.0

ScrollView {
    ListView {
        id: jobList
        model: jobViewModel
        anchors.fill: parent

        delegate: Rectangle {
            width: jobList.width
            height: 80
            color: index % 2 === 0 ? "#f0f0f0" : "#ffffff"
            border.color: "#cccccc"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10

                ColumnLayout {
                    Layout.fillWidth: true

                    Text {
                        text: model.item
                        font.pixelSize: 14
                        font.bold: true
                    }

                    Text {
                        text: {
                            var op = model.operation === 0 ? "Compress" : "Decompress"
                            var status = ["Queued", "Running", "Done", "Failed"][model.status]
                            return op + " - " + status
                        }
                        font.pixelSize: 12
                        color: "#666666"
                    }

                    Text {
                        text: {
                            if (model.originalSize > 0) {
                                var orig = (model.originalSize / 1024.0).toFixed(2) + " KB"
                                var comp = (model.compressedSize / 1024.0).toFixed(2) + " KB"
                                var ratio = (model.ratio * 100).toFixed(1) + "%"
                                return orig + " → " + comp + " (" + ratio + ")"
                            }
                            return ""
                        }
                        font.pixelSize: 11
                        color: "#888888"
                    }
                }

                Text {
                    text: model.errorMessage || ""
                    color: "red"
                    visible: model.status === 3  // Failed
                }
            }
        }
    }
}

