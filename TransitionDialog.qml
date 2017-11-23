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
				text: "Temperatura początkowa [" + temperatureUnit + "]:"
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
				text: "Temperatura końcowa [" + temperatureUnit + "]:"
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
				text: "Entalpia [" + enthalpyUnit + "]:"
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
				text: "Funkcja:"
			}

			TextField {
				id: functionExprField

//				placeholderText: qsTr("function(x) { return -x } ")
//				validator: DoubleValidator {}
				text: transitionClone.functionExpr

				onEditingFinished: if (transitionClone.isExprValid(text)) transitionClone.functionExpr = text
			}

			Label {
				text: "Próbkowanie:"
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
				text: "Wyrównaj do zera:"
			}

			CheckBox {
				id: bAlignBox

				checked: transitionClone.bAlign

				onToggled: transitionClone.bAlign = checked
			}

			Button {
				text: "TEMP integrate"
				onClicked: console.log(transitionClone.calculateA(Number.fromLocaleString(Qt.locale(), enthalpyField.text), alignToZeroBox.checked))
			}
		}

		TransitionChart {
			Layout.preferredWidth: 600
			Layout.preferredHeight: 600
//			Layout.fillWidth: true
			Layout.fillHeight: true

			label: "function(x)"
			transition: dialog.transitionClone
		}
	}

	Connections {
		target: parent
		onHeightChanged: ensureVisible()
		onWidthChanged: ensureVisible()
	}

	onAccepted: {
		transition.copy(transitionClone)
//		if (temperatureBeginField.acceptableInput)
//			transitionClone.temperatureBegin = Number.fromLocaleString(Qt.locale(), temperatureBeginField.text)
//		if (temperatureEndField.acceptableInput)
//			transitionClone.temperatureEnd = Number.fromLocaleString(Qt.locale(), temperatureEndField.text)
//		if (enthalpyField.acceptableInput)
//			transitionClone.enthalpy = Number.fromLocaleString(Qt.locale(), enthalpyField.text)
////		if (samplesField.acceptableInput)
////			transitionClone.samples = Number.fromLocaleString(Qt.locale(), samplesField.text)
//		if (transitionClone.isExprValid(functionExprField.text))
//			transitionClone.functionExpr = functionExprField.text
	}

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
}
