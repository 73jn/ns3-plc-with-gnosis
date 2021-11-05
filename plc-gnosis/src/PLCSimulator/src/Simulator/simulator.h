#ifndef PLC_GUIHELPER_H
#define PLC_GUIHELPER_H

#include <QObject>
#include <QMap>

//TODO Rename stuff in this class so it's consistent
#include "../PLCTopologyLoader/plctopologyloader.h"

#include <ns3/plc-defs.h>
#include <ns3/plc-node.h>
#include <ns3/plc.h>

#include "../BodeWidget/bodewidget.h"

#include "../SimulatorMainWindow/bodewidgetwindow.h"
#include "../GraphWidget/graphwidget.h"

class PLCSimulator : public QObject
{
    Q_OBJECT

public:
    PLCSimulator();
    PLCSimulator(QString modelFileName);
    void collectTransferFunctions();
    void showPsdPlotWindow();
    void showSinrPlotWindow();
    void showBodeWindow();
    int numberOfPlots();

    bool validTopology(){ return canSimulate; }

    void psdTest();

    PLCTopologyLoader* getLoader(){return loader;}

    static void exportTransferData(QString fileName, Ptr<PLC_TransferBase> ctf);


    static QVector<QVector<double> > ctfToPlottable(Ptr<PLC_TransferVector> ctf);
    static QVector<QVector<double> > spectrumValueToPlottable(Ptr<SpectrumValue> val);

public slots:
    void collectTransferFunctions(QStringList enTxNodes, QStringList enRxNodes);
    void simulateSINRAtReceiver(bool doPSD, bool doSINR, QString txName, QString rxName);

private:
    void setupWidgets();

    PLCTopologyLoader * loader;
    BodeWidgetWindow* bodeWindow;

    GraphWidget* psdGraphWidget;
    GraphWidget* sinrGraphWidget;

    bool canSimulate;

    QMap<QString, QMap<QString,Ptr<PLC_TransferBase> > > transferFunctions;
};

#endif // PLC_GUIHELPER_H
