#include "simulator.h"
#include <QApplication>
#include "../PLCSimulatorSettingsWidget/plcsimulatorsettingswidget.h"
#include <QObject>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char* argv[]){

    QApplication app(argc, argv);

    PLCSimulator simulator;
    //PLCSimulator simulator("./diagrams/ScenarioB.dgm");

    if(!simulator.validTopology()){
        qDebug() << "Invalid topology passed to the simulator. Check your terminal for specifics.";
        QMessageBox msg;
        msg.setText("Invalid topology. Check your terminal for specifics (Usually an issue with the dimensions of one of the vdf files you've specified).");
        msg.exec();
        return -1;
    }

    PLCSimulatorSettingsWidget* simSettings = new PLCSimulatorSettingsWidget(QString(""), simulator.getLoader()->getReceiverNames(), simulator.getLoader()->getTransmitterNames());


    QObject::connect(simSettings, SIGNAL(collectGivenTransferFunctions(QStringList,QStringList)), &simulator, SLOT(collectTransferFunctions(QStringList,QStringList)));
    QObject::connect(simSettings, SIGNAL(doNoiseSim(bool,bool,QString,QString)), &simulator, SLOT(simulateSINRAtReceiver(bool,bool,QString,QString)));

    simSettings->show();

    //simulator.collectTransferFunctions();
    ///simulator.simulateSINRAtReceiver("N1", "N2", 100000);

    //simulator.psdTest();

    //simulator.showBodeWindow();
    //simulator.showPlotWindow();

    //qDebug() << "Simulator process finished. Showing Plots.";
    app.exec();
    return 0;
}
