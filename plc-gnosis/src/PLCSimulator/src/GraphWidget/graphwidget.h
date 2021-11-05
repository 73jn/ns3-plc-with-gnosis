#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QMenu>
#include "../../lib/QCustomPlot/qcustomplot.h"


typedef QVector<QVector<double> > GraphData;

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = 0);
    void addPlot(GraphData data, const QString &name);

    QCustomPlot* getPlot(){ return plot; }
    unsigned int getNumberPlots(){ return graphs.count(); }

signals:

public slots:
    void selectionChanged();
    void replot();

protected:

private:
    QCustomPlot* plot;

    int hue;

    QList<QCPGraph*> graphs;
    QCPGraph* currentSelection;
};

#endif // GRAPHWIDGET_H
