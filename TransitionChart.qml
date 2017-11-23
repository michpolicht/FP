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
		titleText: "x"
	}

	ValueAxis {
		id: yAxis

		tickCount: 10
		min: 0.0
		max: 1.0
		titleText: "y"
	}

	LineSeries {
		id: xySeries

		axisX: xAxis
		axisY: yAxis
		name: if (transition.bAlign) {
				  if (transition.b < 0)
					root.label + " + " + -transition.b
				  else
					root.label + " - " + transition.b
			  } else
				  root.label
//        markerSize: 5.0
	}

	Connections {
		target: transition
		onXyChanged: {
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
}
