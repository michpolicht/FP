import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1

import FP 1.0

Dialog {
	id: dialog

	modal: true
	title: "Przejście fazowe - Edycja"
	standardButtons: Dialog.Ok | Dialog.Cancel
	closePolicy: Popup.CloseOnEscape
	focus: true

	property var transition
	property var transitionClone
	property string temperatureUnit: "K"
	property string enthalpyUnit: "kJ"

	RowLayout {
		GridLayout {
			id: grid
			columns: 2

			Label {
				text: "Nazwa:"
			}

			TextField {
				id: nameField

				placeholderText: qsTr("nazwa")
				text: transitionClone.name

				onEditingFinished: transitionClone.name = text
			}

			Label {
				text: qsTr("Temperatura początkowa [" + temperatureUnit + "]:")
			}

			TextField {
				id: temperatureBeginField

				placeholderText: qsTr("temperatura [" + temperatureUnit + "]")
				validator: DoubleValidator {}
				text: transitionClone.temperatureBegin

				states: [
					State {
						when: !temperatureBeginField.acceptableInput
						PropertyChanges {
							target: temperatureBeginField
							color: "red"
						}
					}
				]

				onEditingFinished: transitionClone.temperatureBegin = Number.fromLocaleString(Qt.locale(), text)
			}

			Label {
				text: qsTr("Temperatura końcowa [" + temperatureUnit + "]:")
			}

			TextField {
				id: temperatureEndField

				placeholderText: qsTr("temperatura [" + temperatureUnit + "]")
				validator: DoubleValidator {}
				text: transitionClone.temperatureEnd

				states: [
					State {
						when: !temperatureEndField.acceptableInput
						PropertyChanges {
							target: temperatureEndField
							color: "red"
						}
					}
				]

				onEditingFinished: transitionClone.temperatureEnd = Number.fromLocaleString(Qt.locale(), text)
			}

			Label {
				text: qsTr("Entalpia [" + enthalpyUnit + "]:")
			}

			TextField {
				id: enthalpyField

				placeholderText: qsTr("entalpia [" + enthalpyUnit + "]")
				validator: DoubleValidator {}
				text: transitionClone.enthalpy

				states: [
					State {
						when: !enthalpyField.acceptableInput
						PropertyChanges {
							target: enthalpyField
							color: "red"
						}
					}
				]

				onEditingFinished: transitionClone.enthalpy = Number.fromLocaleString(Qt.locale(), text)
			}

			Label {
				Layout.alignment: Qt.AlignTop

				text: qsTr("Funkcja:")
			}

			Column {
				spacing: 5

				ComboBox {
					id: functionCombo

					width: parent.width
					editable: false
					textRole: "text"
					model: ListModel {
						id: model
						ListElement { text: "Liniowa malejąca"; expr: "function (x) { return -x; }" }
						ListElement { text: "Kwadratowa"; expr: "function (x) { return x * x; }" }
						ListElement { text: "Własna" }
					}
					onActivated: {
						functionExprField.text = model.get(currentIndex).expr
						transitionClone.functionExpr = model.get(currentIndex).expr
					}
				}

				TextField {
					id: functionExprField

					text: transitionClone.functionExpr

					onEditingFinished: {
						if (transitionClone.isExprValid(text)) {
							transitionClone.functionExpr = text
							functionCombo.currentIndex = functionCombo.model.count - 1
						}
					}
				}
			}

			Label {
				text: qsTr("Próbkowanie:")
			}

			TextField {
				id: samplesField

				placeholderText: qsTr("liczba próbek")
				validator: IntValidator { bottom: 1 }
				text: transitionClone.samples

				states: [
					State {
						when: !samplesField.acceptableInput
						PropertyChanges {
							target: samplesField
							color: "red"
						}
					}
				]

				onEditingFinished: transitionClone.samples = Number.fromLocaleString(Qt.locale(), text)
			}

			Label {
				text: qsTr("Wyrównaj do zera:")
			}

			CheckBox {
				id: bAlignBox

				checked: transitionClone.bAlign

				onToggled: transitionClone.bAlign = checked
			}

			Label {
				text: qsTr("Odejmij temperaturę początkową:")
			}

			CheckBox {
				id: subtractBeginTemperatureBox

				checked: transitionClone.subtractBeginTemperature

				onToggled: transitionClone.subtractBeginTemperature = checked
			}

			Label {
				text: qsTr("Odejmij temperaturę końcową:")
			}

			CheckBox {
				id: subtractEndTemperatureBox

				checked: transitionClone.subtractEndTemperature

				onToggled: transitionClone.subtractEndTemperature = checked
			}
		}

		TransitionChart {
			Layout.preferredWidth: 600
			Layout.preferredHeight: 600
			//			Layout.fillWidth: true
			Layout.fillHeight: true

			label: makeTransitionChartLabel()
			transition: dialog.transitionClone
		}
	}

	Connections {
		target: parent
		onHeightChanged: ensureVisible()
		onWidthChanged: ensureVisible()
	}

	onAccepted: transition.copy(transitionClone)

	onOpened: ensureVisible()

	onClosed: {
		destroy()
		transitionClone.deleteLater()
	}

	function ensureVisible() {
		y = Math.min(y, parent.height - implicitHeight)
		x = Math.min(x, parent.width - implicitWidth)
	}

	function updateXY() {
		transitionClone.updateXY(functionExprField.text, temperatureEnd - temperatureBegin, samples)
	}

	function makeTransitionChartLabel() {
		var xLabel = "x"
		if (transitionClone.subtractBeginTemperature)
			xLabel += " - " + transitionClone.temperatureBegin
		if (transitionClone.subtractEndTemperature)
			xLabel += " - " + transitionClone.temperatureEnd + ")"
		return "function(" + xLabel + ")"
	}
}
