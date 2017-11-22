import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1

Dialog {
    id: dialog

    modal: true
    title: "Przejście fazowe - Edycja"
    standardButtons: Dialog.Ok | Dialog.Cancel

    property int index
    property var transitionList

    GridLayout {
        id: grid
        columns: 2

        Text { text: "Three"; font.bold: true; }
        Text { text: "words"; color: "red" }
        Text { text: "in"; font.underline: true }
        Text { text: "a"; font.pixelSize: 20 }
        Text { text: "row"; font.strikeout: true }
    }
    Connections {
        target: parent
        onHeightChanged: ensureVisible()
    }

    onAccepted: console.log("Ok clicked")

    onRejected: console.log("Cancel clicked")

    onOpened: ensureVisible()

    function ensureVisible() {
        y = Math.min(y, parent.height - implicitHeight)
    }
}
