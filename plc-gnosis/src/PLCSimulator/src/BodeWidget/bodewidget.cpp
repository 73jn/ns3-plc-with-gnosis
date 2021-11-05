#include "bodewidget.h"
#include <QBoxLayout>
#include <QDebug>

BodeWidget::BodeWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* plotLayout = new QVBoxLayout(this);

    hue = 250;

    dbPlot = new QCustomPlot(this);
    paPlot = new QCustomPlot(this);

    dbPlot->setInteractions(QCustomPlot::iSelectPlottables | QCustomPlot::iSelectLegend);
    dbPlot->legend->setSelectable(QCPLegend::spItems);
    dbPlot->xAxis->setVisible(true);
    dbPlot->xAxis->setTickLabels(true);
    dbPlot->yAxis->setLabel("Magnitude (dB)");
    dbPlot->xAxis->setLabel("Frequency (MHz)");
    paPlot->xAxis->setLabel("Frequency (MHz)");
    paPlot->yAxis->setLabel("Phase (Radians)");

    QFont legendFont;
    legendFont.setPointSize(8);

    dbPlot->legend->setVisible(true);
    dbPlot->legend->setIconSize(10, 10);
    dbPlot->legend->setFont(legendFont);
    dbPlot->legend->setItemSpacing(0);
    dbPlot->legend->setPositionStyle(QCPLegend::psRight);

    plotLayout->addWidget(dbPlot);
    plotLayout->addWidget(paPlot);

    currentSelection = 0;

    this->setLayout(plotLayout);


    connect(dbPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
}

void BodeWidget::addBodePlot(BodeData *data, const QString& name){
    QCPGraph* dbGraph = dbPlot->addGraph();
    QCPGraph* paGraph = paPlot->addGraph();

    dbGraph->setData(data->at(0), data->at(1));
    paGraph->setData(data->at(0), data->at(2));

    dbGraph->setName(name);
    paGraph->setName(name);


    dbGraph->setPen(QPen(QColor::fromHsv(hue, 255, 255, 192)));
    paGraph->setPen(QPen(QColor::fromHsv(hue, 255, 255, 192)));

    paGraph->setVisible(false);

    dbGraph->rescaleAxes();
    paGraph->rescaleAxes();

    hue += 17;
    hue %= 360;

    this->paGraphMap[dbGraph] = paGraph;
}

void BodeWidget::selectionChanged(){

    QList<QCPAbstractLegendItem*> selectedLegendItems = dbPlot->legend->selectedItems();
    if(selectedLegendItems.length() > 0){
        QCPPlottableLegendItem* plottableLegendItem = dynamic_cast<QCPPlottableLegendItem*>(selectedLegendItems.at(0));
        if(plottableLegendItem){
            plottableLegendItem->plottable()->setSelected(true);
        }
    }

    QList<QCPGraph*> selectedGraphs = dbPlot->selectedGraphs();

    if(selectedGraphs.length() > 0){

        if(currentSelection != 0){
            paGraphMap[currentSelection]->setVisible(false);
        }

        currentSelection = selectedGraphs.at(0);
        paGraphMap[currentSelection]->setVisible(true);

        setWindowTitle(currentSelection->name());

        QCPPlottableLegendItem *lip = dbPlot->legend->itemWithPlottable(currentSelection);
        lip->setSelected(true);
    }

    else {
        setWindowTitle(" ");
    }

    this->replot();
}

void BodeWidget::replot(){
    dbPlot->replot();
    paPlot->replot();

    dbPlot->rescaleAxes();
    paPlot->rescaleAxes();
}
