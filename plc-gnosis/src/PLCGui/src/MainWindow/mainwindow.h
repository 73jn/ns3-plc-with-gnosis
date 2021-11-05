/*

Copyright (C) 2013 Donald Harris (dharris.ubc@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>

*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QGroupBox>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QSharedMemory>
#include <QSlider>

#include <QProcess>

#include "../ZoomSlider/zoomslider.h"
#include "../DiagramEditor/diagrameditor.h"
#include "../DiagramSheet/diagramsheet.h"
#include "../PLCSpectrumConfiguration/plcspectrumconfiguration.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    DiagramSheet * currentSheet;
    DiagramEditor* editor;

    QToolBar * editTools;
    QToolBar * simulationTools;
    QToolBar * lowerToolbar;

    ZoomSlider * zoomSlider;

    QActionGroup * editActionGroup;

    QAction * newAction;
    QAction * saveAction;
    QAction * saveAsAction;
    QAction * openAction;

    QAction* enableSelectorTool;
    QAction* enableNodePlacementTool;
    QAction* enableEdgeDrawingTool;
    QAction* configureSpectrumAction;
    QAction* runSimulationAction;

    QSharedMemory* sharedMemory;

public:
    explicit MainWindow(QWidget *parent = 0);
    
signals:
    
public slots:

    void runSimulation();
    void readFromSimulation();
    void cleanupSimulation();

    void setCurrentSheet(DiagramSheet* sheet);

    void saveCurrentSheet();
    void saveCurrentSheetAs();
    void loadSheet();

    void newSheet();

private:
    void createMenus();
    void createActions();

    //Menus

    QMenu * fileMenu;
    QProcess * simulatorProcess;
    QString ns3BuildPath;
};

#endif // MAINWINDOW_H

//TODO: Adding clickables to add shapes
