import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import QtCharts 2.2

import FP 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("FP - Proj1")

    Model
    {
        id: model

        source: "../Proj1/data/entalpia.csv"
    }


    Flow
    {
        //        x: 0
        //        y: 0
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10

        Column
        {
            spacing: 5

            Button
            {
                text: "Select file..."
                onClicked: fileDialog.open()
            }

            Button
            {
                text: "Parse"
                onClicked: model.parseCSV()
            }

            Button
            {
                text: "Integrate"
                onClicked: model.integrate()
            }
        }

        Column
        {
            spacing: 5

            Label
            {
                text: "File: " + model.source
            }

            Label
            {
                text: "integral: " + model.integral
            }
        }

        ChartView
        {
            width: 400
            height: 400

            ValueAxis
            {
                id: xAxis
            }

            ValueAxis
            {
                id: yAxis
            }

            LineSeries
            {
                id: mainSeries
                axisX: xAxis
                axisY: yAxis
            }

            Connections
            {
                target: model
                onCpChanged: {
                    model.updateSeries(mainSeries)
                    xAxis.min = mainSeries.at(0).x
                    xAxis.max = mainSeries.at(mainSeries.count - 1).x
                    var yMin = Infinity;
                    var yMax = 0.0;
                    for (var i = 0; i < mainSeries.count; i++) {
                        yMin = Math.min(yMin, mainSeries.at(i).y)
                        yMax = Math.max(yMax, mainSeries.at(i).y)
                    }
                    if (yMin < yMax) {
                        yAxis.min = yMin
                        yAxis.max = yMax
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog

        title: "Please choose a file"
        folder: shortcuts.home

        onAccepted: model.source = fileDialog.fileUrl

        onRejected: console.log("Canceled")
    }

    //    SwipeView {
    //        id: swipeView
    //        anchors.fill: parent
    //        currentIndex: tabBar.currentIndex

    //        Page1 {
    //        }

    //        Page {
    //            Label {
    //                text: qsTr("Second page")
    //                anchors.centerIn: parent
    //            }
    //        }
    //    }

    //    footer: TabBar {
    //        id: tabBar
    //        currentIndex: swipeView.currentIndex
    //        TabButton {
    //            text: qsTr("First")
    //        }
    //        TabButton {
    //            text: qsTr("Second")
    //        }
    //    }
}
