#include "bodewidgetwindow.h"
#include "../../lib/QCustomPlot/qcustomplot.h"
#include <QFileDialog>
#include <QMenuBar>

BodeWidgetWindow::BodeWidgetWindow(QWidget *parent) :
    QMainWindow(parent)
{
    plot = new BodeWidget(this);
    setCentralWidget(plot);
    //Setup save menu
    QAction * saveAsPDF = new QAction("Save As PDF", this);

    QMenu * saveMenu= new QMenu("File", this);
    connect(saveAsPDF, SIGNAL(triggered()), this, SLOT(saveMagnitudePlotAsPDF()));
    saveMenu->addAction(saveAsPDF);

    this->menuBar()->addMenu(saveMenu);
}


void BodeWidgetWindow::saveMagnitudePlotAsPDF(){
    QString newFileName = QFileDialog::getSaveFileName(this, "Export Plot As PDF...", "", "PDF Files (*.pdf)", 0, QFileDialog::DontUseNativeDialog);

    if(!newFileName.endsWith(".pdf")){
        newFileName += ".pdf";
    }

    plot->getDBPlot()->savePdf(newFileName, true);

    qDebug() << "Plot saved as " << newFileName;
}
