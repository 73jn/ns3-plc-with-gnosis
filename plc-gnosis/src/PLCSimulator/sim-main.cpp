#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QVariantMap>

//#include <qjson/parser.h>

#include "DiagramSymbol/diagramsymbol.h"
#include "MainWindow/mainwindow.h"
#include "DiagramEditor/diagrameditor.h"
#include "Simulator/simulator.h"

int main(int argc, char* argv[]){

    QApplication app(argc, argv);

    DiagramEditor editor;
    editor.loadTemp();

    SimulatorHelper * simulator = new SimulatorHelper(editor.sheet());

    app.exec();
    return 0;

}
