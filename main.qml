import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtCharts 2.2

import FP 1.0

ApplicationWindow {
    id: appWindow

    visible: true
    width: 1024
    height: 768
    title: qsTr("FP - Proj1")

    property var t1List: model.t1List

    Model {
        id: model

        source: "../FP1/data/entalpia.csv"
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 5

        Component {
            id: t1Delegate

            Row {
                padding: 5
                spacing: 5

                TextField
                {
                    implicitWidth: 100
                    placeholderText: qsTr("Temperatura")
                    validator: DoubleValidator {}
                    text: temperature

                    onAccepted: t1List.setTemperature(index, Number.fromLocaleString(Qt.locale(), text))
                }

                TextField
                {
                    implicitWidth: 100
                    placeholderText: qsTr("Entalpia")
                    validator: DoubleValidator {}
                    text: enthalpy

                    onAccepted: t1List.setEnthalpy(index, Number.fromLocaleString(Qt.locale(), text))
                }

//                Label {
//                    text: temperature
//                }

//                Label {
//                    text: temperature
//                }

//                Button {
//                    text: "Edytuj..."
//                }

                Button {
                    text: qsTr("Usuń")
                    onClicked: t1List.remove(index)
                }
            }
        }

        ColumnLayout
        {
            Layout.fillHeight: true

            ListView {
                id: t1View

                Layout.fillHeight: true
                implicitWidth: 350
                clip: true

                model: t1List
                delegate: t1Delegate

                header: Row {
                    padding: 5
                    spacing: 5

                    Label { width: 100; text: qsTr("Temperatura [K]") }
                    Label { width: 100; text: qsTr("Entalpia [J]") }
                }
            }

            Button
            {
                id: addTransitionButton

                text: "Dodaj przemianę..."
                onClicked: t1List.append()
            }
        }

        ColumnLayout {
            spacing: 5

            Label {
                Layout.alignment: Qt.AlignHCenter

                text: "ΔH [J/mol]: " + model.integral
                font.pixelSize: 18
            }

            TCpChart {
                Layout.fillHeight: true
                Layout.fillWidth: true

                implicitWidth: 400
                implicitHeight: 400

                model: model
                label: model.source
            }

            RowLayout
            {
                spacing: 5

                Button {
                    text: "Wybierz plik..."
                    onClicked: fileDialog.open()
                }

                Button {
                    text: "Parsuj"
                    onClicked: model.parseCSV()
                }

                Label
                {
                    Layout.alignment: Qt.AlignVCenter
                    text: "Przedział całkowania:"
                }

                TextField
                {
                    implicitWidth: 60
                    text: Math.round(tSlider.first.value * 10) / 10
                    focus: true
                    validator: DoubleValidator { bottom: tSlider.from; top: tSlider.second.value }

                    onAccepted: tSlider.first.value = Number.fromLocaleString(Qt.locale(), text)
                }

                RangeSlider
                {
                    id: tSlider

                    from: model.minT
                    to: model.maxT

                    onFromChanged: first.value = from
                    onToChanged: second.value = to
                }

                TextField
                {
                    implicitWidth: 60
                    text: Math.round(tSlider.second.value * 10) / 10
                    focus: true
                    validator: DoubleValidator { bottom: tSlider.first.value; top: tSlider.to }

                    onAccepted: tSlider.second.value = Number.fromLocaleString(Qt.locale(), text)
                }

                Button {
                    text: "Całkuj"
                    onClicked: model.integrate(tSlider.first.value, tSlider.second.value)
                }
            }
        }
    }

    FileDialog {
        id: fileDialog

        title: "Please choose a file"
        folder: shortcuts.home

        onAccepted: model.setSourceAsUrl(fileDialog.fileUrl)

        onRejected: console.log("Canceled")
    }

    function createT1Dialog()
    {
        var component = Qt.createComponent("T1Dialog.qml");
        var dialog = component.createObject(appWindow, {"parent": appWindow});
        dialog.open()
    }
}
