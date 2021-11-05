#ifndef BODEWIDGET_H
#define BODEWIDGET_H

#include <QWidget>
#include <QMenu>
#include "../../lib/QCustomPlot/qcustomplot.h"


typedef QVector<QVector<double> > BodeData;

class BodeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BodeWidget(QWidget *parent = 0);
    void addBodePlot(BodeData* data, const QString &name);

    QCustomPlot* getDBPlot(){ return dbPlot; }
    QCustomPlot* getPAPlot(){ return paPlot; }

    unsigned int getNumberPlots(){ return paGraphMap.count(); }

signals:
    
public slots:
    void selectionChanged();
    void replot();

protected:

private:
    QCustomPlot* dbPlot; //Decibel power plot
    QCustomPlot* paPlot; //phase angle plot

    int hue;

    QMap<QCPGraph*, QCPGraph*> paGraphMap;

    QCPGraph* currentSelection;
};

#endif // BODEWIDGET_H
