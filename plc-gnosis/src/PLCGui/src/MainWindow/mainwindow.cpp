#include "mainwindow.h"

#include <QTime>

#include <QDebug>
#include <QPushButton>
#include <QElapsedTimer>
#include <QMenuBar>
#include <QSharedMemory>
#include <QLabel>
#include <QFileDialog>

//TODO: All actions should connect to the editor.

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    currentSheet = 0;


    //Need to load settings if possible here... right now uses default
    ns3BuildPath = PLC_DEFAULT_NS3_PATH; //Should have a dialog to set this. Right now LD_LIBRARY_PATH should have the ns3 libraries on it (as well as qwt libraries for the time being)

    //Setup the Layouts and Toolbars
    editTools = new QToolBar(QString("Editing Tools"), this);
    editTools->setMovable(false);
    editTools->setIconSize(QSize(16, 16));

    simulationTools = new QToolBar(QString("Simulation Tools"), this);
    simulationTools->setMovable(false);
    simulationTools->setIconSize(QSize(16, 16));

    lowerToolbar = new QToolBar(QString("Misc. Tools"), this);
    lowerToolbar->setMovable(false);

    zoomSlider = new ZoomSlider(this);
    zoomSlider->setZoom(1.0);

    QWidget * spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    lowerToolbar->addWidget(spacer);
    lowerToolbar->addWidget(new QLabel("Zoom:", this));
    lowerToolbar->addWidget(zoomSlider);


    this->editActionGroup = new QActionGroup(this);

    editor = new DiagramEditor();

    connect(zoomSlider, SIGNAL(zoomChanged(double)), editor, SLOT(setZoom(double)));
    connect(editor, SIGNAL(zoomChanged(double)), zoomSlider, SLOT(setZoom(double)));

    setCentralWidget(editor);
    editor->setFocus();

    addToolBar(Qt::LeftToolBarArea, editTools);
    addToolBar(Qt::LeftToolBarArea, simulationTools);
    addToolBar(Qt::BottomToolBarArea, lowerToolbar);



    createActions();
    createMenus();

    setCurrentSheet(new DiagramSheet());

    simulatorProcess = 0;
}

void MainWindow::createActions(){

    newAction = new QAction("&New", this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setToolTip("Closes the currently open sheet and opens a new one");
    connect(newAction, SIGNAL(triggered()), this, SLOT(newSheet()));

    saveAction = new QAction("&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setToolTip("Save current diagram to disk");
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveCurrentSheet()));

    saveAsAction = new QAction("Save As",  this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setToolTip("Save current diagram as..");
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveCurrentSheetAs()));

    openAction = new QAction("&Open", this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setToolTip("Load an existing diagram from disk");
    connect(openAction, SIGNAL(triggered()), this, SLOT(loadSheet()));

    //ToolBar Actions
    QIcon * selectionIcon = new QIcon(":/icons/arrow.svg");
    enableSelectorTool = new QAction(this);
    enableSelectorTool->setIcon(*selectionIcon);
    enableSelectorTool->setCheckable(true);
    enableSelectorTool->setChecked(true);
    editActionGroup->addAction(enableSelectorTool);
    editTools->addAction(enableSelectorTool);

    QIcon * componentIcon = new QIcon(":/icons/node.svg");
    enableNodePlacementTool = new QAction(this);
    enableNodePlacementTool->setIcon(*componentIcon);
    enableNodePlacementTool->setCheckable(true);
    editActionGroup->addAction(enableNodePlacementTool);
    editTools->addAction(enableNodePlacementTool);

    QIcon * lineIcon = new QIcon(":/icons/edge.svg");
    enableEdgeDrawingTool = new QAction(this);
    enableEdgeDrawingTool->setIcon(*lineIcon);
    enableEdgeDrawingTool->setCheckable(true);
    editActionGroup->addAction(enableEdgeDrawingTool);
    editTools->addAction(enableEdgeDrawingTool);

    QIcon * simSettingsIcon = new QIcon(":/icons/spectrum.svg");
    configureSpectrumAction = new QAction(this);
    configureSpectrumAction->setIcon(*simSettingsIcon);
    simulationTools->addAction(configureSpectrumAction);

    //Simulation 'Play' Action
    QIcon * simulateIcon = new QIcon(":/icons/play.svg");
    runSimulationAction = new QAction(this);
    runSimulationAction->setIcon(*simulateIcon);
    simulationTools->addAction(runSimulationAction);
    connect(runSimulationAction, SIGNAL(triggered(bool)), this, SLOT(runSimulation()));

}

void MainWindow::newSheet(){
    setCurrentSheet(0);
}

void MainWindow::setCurrentSheet(DiagramSheet* sheet){

    if(currentSheet != 0){
        delete(this->currentSheet);
    }

    if(sheet == 0){
        sheet = new DiagramSheet();
    }

    currentSheet = sheet;
    editor->setScene(currentSheet);

    editor->ensureVisible(currentSheet->itemsBoundingRect());

    connect(enableSelectorTool, SIGNAL(triggered(bool)), currentSheet, SLOT(setSelectionMode(bool)));
    connect(enableNodePlacementTool, SIGNAL(triggered(bool)), currentSheet, SLOT(placeNewNode(bool)));
    connect(enableEdgeDrawingTool, SIGNAL(triggered(bool)), currentSheet, SLOT(setLineMode(bool)));
    connect(configureSpectrumAction, SIGNAL(triggered(bool)), currentSheet, SLOT(configureSpectrumModel()));
}

void MainWindow::saveCurrentSheet(){

    if(currentSheet->getSaveToFile().isEmpty()){
        //QFileDialog dialog;
        //dialog.setFileMode(QFileDialog::AnyFile);
        QString newFileName = QFileDialog::getSaveFileName(this, "Save Diagram File...", "", "Diagram Files (*.dgm)", 0);

        if(!newFileName.endsWith(".dgm")){
            newFileName += ".dgm";
        }

        currentSheet->setSaveToFile(newFileName);
    }

    currentSheet->setSelectionMode(true);

    QFile saveFile(currentSheet->getSaveToFile());
    saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QByteArray jsonData= currentSheet->toJSONData();
    saveFile.write(jsonData); //TODO: Possibly add text compression here.
    saveFile.close();
}

void MainWindow::saveCurrentSheetAs(){
    currentSheet->setSaveToFile("");
    saveCurrentSheet();
}

void MainWindow::loadSheet(){
     QString openFileName = QFileDialog::getOpenFileName(this, "Save Diagram File...", "", "Diagram Files (*.dgm)", 0);
     if(openFileName.isEmpty()){
         //setCurrentSheet(new DiagramSheet());
         return;
     }
     else{
         setCurrentSheet(new DiagramSheet(openFileName));
         return;
     }
}

void MainWindow::createMenus(){
    fileMenu = this->menuBar()->addMenu("&File");

    fileMenu->addAction(newAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(openAction);
}

void MainWindow::readFromSimulation(){
    //qDebug() << simulatorProcess->readAll();
}

void MainWindow::cleanupSimulation(){

    sharedMemory->detach();
    delete(sharedMemory);

    disconnect(simulatorProcess, SIGNAL(readyRead()), this, SLOT(readFromSimulation()));
    disconnect(simulatorProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cleanupSimulation()));

    delete simulatorProcess;
    simulatorProcess = 0;
}



void MainWindow::runSimulation(){

    if(simulatorProcess != 0){
        qDebug() << "Simulator already running!";
        return;
    }

    QByteArray diagramJSONData = currentSheet->toJSONData();

    sharedMemory = new QSharedMemory("PLC_TOPOLOGY_JSON_DATA", this);
    bool ok = sharedMemory->create(diagramJSONData.length());

    if(!ok){
        if(sharedMemory->error() == QSharedMemory::AlreadyExists){
            qDebug() << "Issue with shared memory....???";
            sharedMemory->attach();
            sharedMemory->detach();
            delete(sharedMemory);

            qDebug() << "Trying to remove existing shared memory. Make sure simulator processes "
                        "are killed if this fails.";

            sharedMemory = new QSharedMemory("PLC_TOPOLOGY_JSON_DATA", this);
            ok = sharedMemory->create(diagramJSONData.length());

            if(!ok){
                qDebug() << "Make sure no other simulator processes are running and try again";
                return;
            }
        }
    }

    //Copy the json data into shared memory for access by the simulator program
    sharedMemory->lock();
    memcpy(sharedMemory->data(), diagramJSONData.constData(), diagramJSONData.length());
    sharedMemory->unlock();

    simulatorProcess = new QProcess();
    simulatorProcess->setProcessChannelMode(QProcess::ForwardedChannels);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    //Make sure we can get at the ns3 libraries
    env.insert(LD_ENV_VAR, QDir::currentPath() + ':' + this->ns3BuildPath + ':' + env.value(LD_ENV_VAR));
    simulatorProcess->setProcessEnvironment(env);

    connect(simulatorProcess, SIGNAL(readyRead()), this, SLOT(readFromSimulation()));
    connect(simulatorProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cleanupSimulation()));

    qDebug() << "GUI:\n    Running external simulator process";

    simulatorProcess->start("./simulator");
    simulatorProcess->waitForStarted();
}
