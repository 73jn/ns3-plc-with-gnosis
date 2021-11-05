#include "graphwidget.h"
#include <QBoxLayout>
#include <QDebug>

GraphWidget::GraphWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* plotLayout = new QVBoxLayout(this);

    hue = 250;

    plot = new QCustomPlot(this);

    plot->setInteractions(QCustomPlot::iSelectPlottables | QCustomPlot::iSelectLegend);
    plot->legend->setSelectable(QCPLegend::spItems);
    plot->xAxis->setVisible(true);
    plot->xAxis->setTickLabels(true);
    plot->yAxis->setLabel("Magnitude (dB)");
    plot->xAxis->setLabel("Frequency (MHz)");

    QFont legendFont;
    legendFont.setPointSize(8);

    plot->legend->setVisible(true);
    plot->legend->setIconSize(10, 10);
    plot->legend->setFont(legendFont);
    plot->legend->setItemSpacing(0);
    plot->legend->setPositionStyle(QCPLegend::psRight);

    plotLayout->addWidget(plot);

    currentSelection = 0;

    this->setLayout(plotLayout);

    connect(plot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
}


void GraphWidget::addPlot(GraphData data, const QString& name){
    QCPGraph* graph = plot->addGraph();

    graph->setData(data.at(0), data.at(1));
    graph->setName(name);
    graph->setPen(QPen(QColor::fromHsv(hue, 255, 255, 192)));

    graph->rescaleAxes(false);

    hue += 17;
    hue %= 360;

    this->graphs.append(graph);
    plot->replot();
}

void GraphWidget::selectionChanged(){

    QList<QCPAbstractLegendItem*> selectedLegendItems = plot->legend->selectedItems();
    if(selectedLegendItems.length() > 0){
        QCPPlottableLegendItem* plottableLegendItem = dynamic_cast<QCPPlottableLegendItem*>(selectedLegendItems.at(0));
        if(plottableLegendItem){
            plottableLegendItem->plottable()->setSelected(true);
        }
    }

    QList<QCPGraph*> selectedGraphs = plot->selectedGraphs();

    if(selectedGraphs.length() > 0){
        currentSelection = selectedGraphs.at(0);
        setWindowTitle(currentSelection->name());

        QCPPlottableLegendItem *lip = plot->legend->itemWithPlottable(currentSelection);
        lip->setSelected(true);
    }

    else {
        setWindowTitle(" ");
    }

    this->replot();
}

void GraphWidget::replot(){
    plot->replot();
}
