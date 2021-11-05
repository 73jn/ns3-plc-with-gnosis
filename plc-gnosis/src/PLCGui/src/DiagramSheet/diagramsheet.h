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

#ifndef DIAGRAMSHEET_H
#define DIAGRAMSHEET_H

#include <QGraphicsView>
#include <QMap>

#include "../PLCGraphicsNodeItem/plcgraphicsnodeitem.h"
#include "../PLCGraphicsEdgeItem/plcgraphicsedgeitem.h"
#include "plctopologymodel.h"

/*! \class DiagramSheet
 *  \brief Manages the graphical representation of a PLC schematic
 *
 *  The DiagramSheet class derives from QGraphicsScene and manages the items in a scene. It keeps track of
 *  the names of nodes, their corresponding data structures, and provides functionality which
 *  allows schematics to be converted to JSON representations and stored as files.
 *
 *  DiagramSheets also have editing modes which allow the scene contents to be changed during user initiated
 *  editing commands. It also keeps track of nodes and edges that are being placed and ensures that their
 *  state is consistent during this process.
 */
class  DiagramSheet : public QGraphicsScene
{
    Q_OBJECT

public:

    /*! Defines the edit modes that this scene can be in */
    enum EditMode {
        SelectionMode,  /*!< The default normal/selecting mode.*/
        LineMode,       /*!< Editing mode in which lines/edges are drawn, connecting nodes */
        PlacementMode   /*!< The creation and placement of new nodes into the schematic */
    };

    explicit DiagramSheet(QObject *parent = 0);
    explicit DiagramSheet(const QString& fileName, QObject* parent = 0);

    ~DiagramSheet();

    void addNode(PLCGraphicsNodeItem * node);

    void removeNode(PLCGraphicsNodeItem* node);
    void removeNode(const QString& nodeName);

    void addEdge(PLCGraphicsEdgeItem * edge);
    void removeEdge(PLCGraphicsEdgeItem* edge);

    QString generateNodeName();
    bool nameExists(QString name);


    const QMap<QString, PLCGraphicsNodeItem*> nodes() const;

    void selectNode(const QString& nodeName);

    PLCTopologyModel toTopologyModel();
    QByteArray       toJSONData();

    void setSaveToFile(const QString& file){fileName = file;}
    QString getSaveToFile(){ return fileName; }

    bool shouldShowAnnotations(){ return showAnnotations; }

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    virtual bool event(QEvent *event);

protected slots:
    bool focusNextPrevChild(bool next);

public slots:
    void setSelectionMode(bool checked);
    void setLineMode(bool checked);
    void placeNewNode(bool checked);
    void configureSpectrumModel();

private:
    PLCGraphicsEdgeItem* edge;  /*!< Holds the current line being drawn */
    PLCGraphicsNodeItem* node;  /*!< Hold the current node being placed */
    EditMode sceneEditMode;     /*!< The current editing mode that the scene is in */

\
    /*! Keeps track of all the nodes in the scene by their name */
    QMap<QString, PLCGraphicsNodeItem*> nodesByName;


    QList<PLCGraphicsEdgeItem*> edges;      /*!< Holds a list of all the edges currently in the scene */
    PLCSpectrumModel spectrumModel;         /*!< Holds the settings over which the schematic will be analyzed */
    QString fileName;                       /*!< File name associated with this sheet. Null if the file has yet to be saved*/

    bool showAnnotations;                   /*!< True if labels should be shown around components */

    void fromTopologyModel(PLCTopologyModel &topologyModel);
    void setup();
};

#endif // DIAGRAMSHEET_H
