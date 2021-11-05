#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QVariantMap>


#include "MainWindow/mainwindow.h"

#include "nodemodel.h"
#include "NodeConfiguration/nodeconfiguration.h"

#include "PLCGraphicsNodeItem/plcgraphicsnodeitem.h"
#include "nodemodel.h"

int main(int argc, char* argv[]){

    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();


    //cfg.show();

    app.exec();
    return 0;

}


