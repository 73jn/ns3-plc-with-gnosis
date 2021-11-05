#ifndef SIMULATORMAINWINDOW_H
#define SIMULATORMAINWINDOW_H

#include <QMainWindow>
#include "../BodeWidget/bodewidget.h"

class BodeWidgetWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit BodeWidgetWindow(QWidget *parent = 0);
    BodeWidget * getPlot(){ return plot; }

signals:

public slots:
    void saveMagnitudePlotAsPDF();

private:
    BodeWidget * plot;
};

#endif // SIMULATORMAINWINDOW_H
