import QtQuick 2.0
import QtCharts 2.2

ChartView
{
    id: root

    property var model
    property string label
	property string temperatureUnit: "K"
	property string enthalpyUnit: "kJ"
	property string cpUnit: temperatureUnit + "⁻¹ " + enthalpyUnit
	property alias minT: xAxis.min
	property alias maxT: xAxis.max

    ValueAxis {
        id: xAxis

        tickCount: 10
        min: model.minT
        max: model.maxT
		titleText: "T [" + temperatureUnit + "]"
    }

    ValueAxis {
        id: yAxis

        tickCount: 10
        min: model.minCp
        max: model.maxCp
		titleText: "Cp [" + cpUnit + "]"
    }

	LineSeries {
		id: cumulativeSeries

		axisX: xAxis
		axisY: yAxis
		name: "Kumulacyjny"
		color: "red"
//        markerSize: 5.0
	}

	LineSeries {
        id: mainSeries

        axisX: xAxis
        axisY: yAxis
        name: root.label
		color: "blue"
	}

	ScatterSeries {
		id: mainSeriesScatter

		axisX: xAxis
		axisY: yAxis
//		name: root.label
		color: "blue"
		markerSize: 2.0
		borderWidth: 0.0
	}


	ScatterSeries {
		id: cumulativeScatter

		axisX: xAxis
		axisY: yAxis
//		name: "Kumulacyjny"
		color: "red"
		markerSize: 2.0
		borderWidth: 0.0
	}

	Connections {
        target: model
        onCpTChanged: updateSeries()
    }

    function updateSeries()
    {
        model.updateSeries(mainSeries)
        model.updateSeries(mainSeriesScatter)
        updateCumulativeSeries()
    }

    function updateCumulativeSeries()
    {
        model.updateCumulativeSeries(cumulativeSeries)
        model.updateCumulativeSeries(cumulativeScatter)
    }
}
