import QtQuick 2.0
import QtQuick.Controls 2.1

Dialog {
    id: dialog

    implicitWidth: 200
    implicitHeight: 300

    title: "Title"
    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: console.log("Ok clicked")
    onRejected: console.log("Cancel clicked")
}
