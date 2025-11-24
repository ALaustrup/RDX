import QtQuick 6.0
import QtQuick.Controls 6.0

Dialog {
    id: dashboardDialog
    title: "Lifetime Corpus Model Dashboard"
    width: 700
    height: 500
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
            text: "Total Files Tracked: " + lcmStatsViewModel.totalFiles
            font.pixelSize: 14
        }

        Text {
            text: "Total Corpus Size: " + (lcmStatsViewModel.totalCorpusSize / 1024.0 / 1024.0).toFixed(2) + " MB"
            font.pixelSize: 14
        }

        Text {
            text: "Top Schemas:"
            font.pixelSize: 14
            font.bold: true
        }

        Text {
            text: lcmStatsViewModel.topSchemas
            font.pixelSize: 12
            Layout.fillWidth: true
        }

        Text {
            text: "Top File Types:"
            font.pixelSize: 14
            font.bold: true
        }

        Text {
            text: lcmStatsViewModel.topFileTypes
            font.pixelSize: 12
            Layout.fillWidth: true
        }

        Button {
            text: "Refresh"
            onClicked: lcmStatsViewModel.refresh()
        }

        Button {
            text: "Close"
            onClicked: dashboardDialog.close()
        }
    }
}

