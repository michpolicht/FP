import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtCharts 2.2

import FP 1.0

/**
  @todo H0.
  */
ApplicationWindow {
    id: appWindow

    visible: true
	width: 1280
    height: 768
	title: qsTr("FP Projekt")

	property var transitionList: model.transitionList
	property string enthalpyUnit: "kJ/mol"
	property string temperatureUnit: "K"
	property int listViewItemWidth: 100
	property int listViewItemSpacing: 5

    Model {
        id: model

        source: "../FP1/data/entalpia.csv"
    }

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
	}

	Component {
		id: transitionListDelegate

		RowLayout {
			id: transitionListDelegateLayout

			spacing: listViewItemSpacing

			Text {
				Layout.minimumWidth: listViewItemWidth
				text: temperatureBegin
			}

			Text {
				Layout.minimumWidth: listViewItemWidth
				text: temperatureEnd
			}

			Text {
				Layout.minimumWidth: listViewItemWidth
				text: enthalpy
			}

			Button {
				id: editButton

				text: qsTr("Edytuj...")
				onClicked: createTransitionDialog(index)
			}

			Button {
				text: qsTr("Usuń")
				onClicked: transitionList.remove(index)
			}
		}
	}

	RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 5

        ColumnLayout
        {
            Layout.fillHeight: true

            ListView {
				id: transitionListView

                Layout.fillHeight: true
				implicitWidth: listViewItemWidth * 3 + addTransitionButton.width * 2 + listViewItemSpacing * 5
                clip: true

				model: transitionList
				delegate: transitionListDelegate
				spacing: listViewItemSpacing

//				headerPositioning: ListView.OverlayHeader
                header: Row {
					spacing: listViewItemSpacing
					bottomPadding: 20

					Label { width: listViewItemWidth; text: qsTr("T. początkowa [" + temperatureUnit + "]") }
					Label { width: listViewItemWidth; text: qsTr("T. końcowa [" + temperatureUnit + "]") }
					Label { width: listViewItemWidth; text: qsTr("Entalpia [" + enthalpyUnit + "]") }
					Label { width: listViewItemWidth; text: qsTr("Operacje") }
                }
            }

            Button
            {
                id: addTransitionButton

                text: "Dodaj przemianę..."
				onClicked: transitionList.append()
            }
        }

        ColumnLayout {
            spacing: 5

            Label {
                Layout.alignment: Qt.AlignHCenter

				text: "ΔH [" + enthalpyUnit + "]: " + model.integral
                font.pixelSize: 18
            }

            TCpChart {
				id: tCpChart

                Layout.fillHeight: true
                Layout.fillWidth: true

                implicitWidth: 400
                implicitHeight: 400

                model: model
                label: model.source
				temperatureUnit: appWindow.temperatureUnit
				enthalpyUnit: appWindow.enthalpyUnit
				minT: tSlider.first.value
				maxT: tSlider.second.value
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

	function createTransitionDialog(index)
    {
        var component = Qt.createComponent("TransitionDialog.qml");
		var dialog = component.createObject(appWindow, {"parent": appWindow,
												"x": mouseArea.mouseX,
												"y": mouseArea.mouseY,
												"transition": transitionList.at(index),
												"transitionClone": transitionList.at(index).clone(),
												"transitionList": transitionList,
												"temperatureUnit": temperatureUnit,
												"enthalpyUnit": enthalpyUnit
											});
		dialog.accepted.connect(tCpChart.updateCumulativeSeries)
		dialog.open()
    }
}
