import QtQuick 6.0
import QtQuick.Controls 6.0
import RDX 1.0

ScrollView {
    clip: true
    background: Rectangle {
        color: "#1a1a1a"
    }

    ListView {
        id: jobList
        model: jobViewModel
        anchors.fill: parent
        spacing: 8

        delegate: Rectangle {
            width: jobList.width
            height: 100
            color: index % 2 === 0 ? "#252525" : "#2d2d2d"
            
            // Colorful edge based on status
            border.color: {
                if (model.status === 3) return "#f44336"  // Failed - red
                if (model.status === 2) return "#4caf50"  // Done - green
                if (model.status === 1) return "#00a8ff"  // Running - blue
                return "#9c27b0"  // Queued - purple
            }
            border.width: 2
            radius: 4

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                // Status indicator icon
                Rectangle {
                    Layout.preferredWidth: 40
                    Layout.preferredHeight: 40
                    radius: 20
                    color: {
                        if (model.status === 3) return "#f44336"  // Failed
                        if (model.status === 2) return "#4caf50"  // Done
                        if (model.status === 1) return "#00a8ff"  // Running
                        return "#9c27b0"  // Queued
                    }
                    border.color: "#ffffff"
                    border.width: 2

                    Text {
                        anchors.centerIn: parent
                        text: {
                            if (model.status === 3) return "✕"
                            if (model.status === 2) return "✓"
                            if (model.status === 1) return "⟳"
                            return "○"
                        }
                        font.pixelSize: 18
                        color: "#ffffff"
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    // File name
                    Text {
                        text: model.item
                        font.pixelSize: 15
                        font.bold: true
                        color: "#ffffff"
                        elide: Text.ElideMiddle
                    }

                    // Operation and status
                    RowLayout {
                        spacing: 8

                        Text {
                            text: {
                                var op = model.operation === 0 ? "📦 Compress" : "📂 Decompress"
                                return op
                            }
                            font.pixelSize: 12
                            color: "#b0b0b0"
                        }

                        Text {
                            text: "•"
                            font.pixelSize: 12
                            color: "#808080"
                        }

                        Text {
                            text: {
                                var status = ["Queued", "Running", "Done", "Failed"][model.status]
                                return status
                            }
                            font.pixelSize: 12
                            font.bold: true
                            color: {
                                if (model.status === 3) return "#f44336"  // Failed
                                if (model.status === 2) return "#4caf50"  // Done
                                if (model.status === 1) return "#00a8ff"  // Running
                                return "#9c27b0"  // Queued
                            }
                        }
                    }

                    // Size and ratio info
                    Text {
                        text: {
                            if (model.originalSize > 0) {
                                var orig = (model.originalSize / 1024.0).toFixed(2) + " KB"
                                var comp = (model.compressedSize / 1024.0).toFixed(2) + " KB"
                                var ratio = (model.ratio * 100).toFixed(1) + "%"
                                var savings = ((1 - model.ratio) * 100).toFixed(1) + "%"
                                return orig + " → " + comp + " (" + ratio + ", saved " + savings + ")"
                            }
                            return ""
                        }
                        font.pixelSize: 11
                        color: "#808080"
                    }

                    // Progress bar for running jobs
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 4
                        visible: model.status === 1  // Running
                        color: "#1a1a1a"
                        border.color: "#00a8ff"
                        border.width: 1
                        radius: 2

                        Rectangle {
                            width: parent.width * 0.5  // TODO: Connect to actual progress
                            height: parent.height
                            color: "#00a8ff"
                            radius: 2
                        }
                    }

                    // Error message
                    Text {
                        text: model.errorMessage || ""
                        color: "#f44336"
                        font.pixelSize: 11
                        visible: model.status === 3  // Failed
                        wrapMode: Text.WordWrap
                    }
                }

                // Ratio badge
                Rectangle {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 30
                    visible: model.status === 2 && model.originalSize > 0  // Done with data
                    color: {
                        var ratio = model.ratio
                        if (ratio < 0.3) return "#4caf50"  // Excellent
                        if (ratio < 0.5) return "#8bc34a"  // Good
                        if (ratio < 0.7) return "#ff9800"  // Fair
                        return "#f44336"  // Poor
                    }
                    border.color: "#ffffff"
                    border.width: 1
                    radius: 4

                    Text {
                        anchors.centerIn: parent
                        text: (model.ratio * 100).toFixed(0) + "%"
                        font.pixelSize: 12
                        font.bold: true
                        color: "#ffffff"
                    }
                }
            }
        }

        // Empty state
        Rectangle {
            anchors.centerIn: parent
            width: 400
            height: 200
            visible: jobList.count === 0
            color: "transparent"

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 16

                Text {
                    text: "📦"
                    font.pixelSize: 64
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }

                Text {
                    text: "No compression jobs yet"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#b0b0b0"
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }

                Text {
                    text: "Use the buttons on the left to compress or decompress files"
                    font.pixelSize: 12
                    color: "#808080"
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }
    }
}
