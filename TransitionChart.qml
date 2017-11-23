import QtQuick 2.0
import QtCharts 2.2

ChartView
{
	id: root

	property var transition
	property string label
	property bool alignToZero: false

	ValueAxis {
		id: xAxis

		tickCount: 10
//		min: model.minT
		min: 0.0
		max: 1.0
		titleText: "T [K]"
	}

	ValueAxis {
		id: yAxis

		tickCount: 10
		min: 0.0
		max: 1.0
		titleText: "dQ/dT [kJ/mol]"
	}

	LineSeries {
		id: xySeries

		axisX: xAxis
		axisY: yAxis
		name: makeLabel()
//        markerSize: 5.0
	}

	Connections {
		target: transition
		onCpTChanged: {
			transition.updateSeries(xySeries)
			updateAxes()
		}
	}

	Component.onCompleted: {
		transition.updateSeries(xySeries)
		updateAxes()
	}

	function updateAxes()
	{
		var minX = Infinity
		var maxX = -Infinity
		var minY = 0.0
		var maxY = -Infinity
		for (var i = 0; i < xySeries.count; i++) {
			minX = Math.min(minX, xySeries.at(i).x)
			maxX = Math.max(maxX, xySeries.at(i).x)
			minY = Math.min(minY, xySeries.at(i).y)
			maxY = Math.max(maxY, xySeries.at(i).y)
		}
		xAxis.min = minX
		xAxis.max = maxX
		yAxis.min = minY
		yAxis.max = maxY
	}

	function makeLabel()
	{
		var label = Math.round(transition.a * 1000) / 1000
		if (transition.bAlign) {
			if (transition.b < 0)
				label += " (" + root.label + " + " + -transition.b + ")"
			else
				label += " (" + root.label + " - " + transition.b + ")"
		} else
			label += " " + root.label
		return label
	}

}
