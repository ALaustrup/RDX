import QtQuick 6.0
import QtQuick.Controls 6.0

Dialog {
    id: settingsDialog
    title: "Settings"
    width: 500
    height: 400
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
        }

        Button {
            text: "Close"
            onClicked: settingsDialog.close()
        }
    }
}

