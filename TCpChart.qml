import QtQuick 2.0
import QtCharts 2.2

ChartView
{
    id: root

    property var model
    property string label

    ValueAxis {
        id: xAxis

        tickCount: 10
        min: model.minT
        max: model.maxT
        titleText: "T [K]"
    }

    ValueAxis {
        id: yAxis

        tickCount: 10
        min: model.minCp
        max: model.maxCp
        titleText: "Cp [J/(K mol)]"
    }

    LineSeries {
        id: mainSeries

        axisX: xAxis
        axisY: yAxis
        name: root.label
//        markerSize: 5.0
    }

    Connections {
        target: model
        onCpChanged: model.updateSeries(mainSeries)
//        onCpChanged: {
//            model.updateSeries(mainSeries)
//            xAxis.min = mainSeries.at(0).x
//            xAxis.max = mainSeries.at(mainSeries.count - 1).x
//            var yMin = Infinity;
//            var yMax = 0.0;
//            for (var i = 0; i < mainSeries.count; i++) {
//                yMin = Math.min(yMin, mainSeries.at(i).y)
//                yMax = Math.max(yMax, mainSeries.at(i).y)
//            }
//            if (yMin < yMax) {
//                yAxis.min = yMin
//                yAxis.max = yMax
//            }
//        }
    }
}
