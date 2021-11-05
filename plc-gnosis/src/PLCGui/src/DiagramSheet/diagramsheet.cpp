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

#include "diagramsheet.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include <QGraphicsItem>

#include "../NodeConfiguration/nodeconfiguration.h"
#include "../EdgeConfiguration/edgeconfiguration.h"
#include "../PLCSpectrumConfiguration/plcspectrumconfiguration.h"

#include <QDebug>
#include <QFileDialog>

#include "../../lib/qjson/src/parser.h"
#include "../../lib/qjson/src/serializer.h"


/*! Creates a default diagram sheet with the given QObject parent */
DiagramSheet::DiagramSheet(QObject *parent) :
    QGraphicsScene(parent)
{
    fileName = QString("");
    showAnnotations = false;
    setup();
}

/*! Takes a QString fileName and attempts to create a new diagram sheet from the corresponding file on disk. */
DiagramSheet::DiagramSheet(const QString &fileName, QObject* parent) : QGraphicsScene(parent){

    QFile diagramFile(fileName);
    diagramFile.open(QIODevice::ReadOnly);

    showAnnotations = false;

    QByteArray jsonData = diagramFile.readAll();

    QJson::Parser jsonParser;

    bool ok;
    QVariantMap diagramMap = jsonParser.parse(jsonData, &ok).toMap();

    if(!ok){
        qDebug() << "Problem reading the diagram map from file" << fileName;
        setup();
        return;
    }
    else{
        PLCTopologyModel topologyModel(diagramMap);
        fromTopologyModel(topologyModel);
        this->fileName = fileName;
    }

    setup();
}

/*! Sets up default values for a few member variables, including the default scene rectangle and
 *  initializing pointers to zero. */
void DiagramSheet::setup(){
    sceneEditMode = SelectionMode;
    setSceneRect(-250, -250, 500, 500);
    edge = 0;
    node = 0;
}

/*! Takes a topology model, which is the underlying data structure representing a schematic, and
 *  loads it into this sheet by populating it with the appropriate graphics items */
void DiagramSheet::fromTopologyModel(PLCTopologyModel &topologyModel){
    QList<NodeModel*>* nodeList = topologyModel.getNodes();
    QList<EdgeModel*>* edgeList = topologyModel.getEdges();

    //Loads up all of the nodes
    for(int i = 0; i < nodeList->length(); i++){
        this->addNode(new PLCGraphicsNodeItem(nodeList->at(i)));
    }

    //Load up all of the edges
    for(int i = 0; i < edgeList->length(); i++){
        EdgeModel* currentEdge = edgeList->at(i);
        this->addEdge(new PLCGraphicsEdgeItem(currentEdge, nodesByName[currentEdge->getFromNode()],
                      nodesByName[currentEdge->getToNode()]));
    }

    //Load the spectrum model
    this->spectrumModel = topologyModel.getSpectrumModel();
}

/*! DiagramSheet destructor. Cleanup requires that edges be removed first before nodes, so that edges aren't
 *  connected to non-existent nodes which is a weird state and can cause crashes */
DiagramSheet::~DiagramSheet(){
    QMap<QString, PLCGraphicsNodeItem *>::iterator it;

    //Edges need to be deleted/removed first
    for(int i = 0; i < edges.length(); i++){
        QGraphicsScene::removeItem(edges.at(i));
        delete(edges.at(i));
    }

    for(it = nodesByName.begin(); it != nodesByName.end(); it++){
        QGraphicsScene::removeItem(*it);
        delete(*it);
    }
}


/*! Adds a node to the sheet. It also makes sure that the node has a unique name before it
 *  adds it to the scene, so that it is uniquely specified in the nodesByName map. */
void DiagramSheet::addNode(PLCGraphicsNodeItem *node){
    if(!node){
        return;
    }

    QString nodeName = node->getNodeModel()->getName();

    //Set the new name of the symbol if necessary
    if(nodeName.isNull() || nodesByName.contains(nodeName)){
        nodeName = generateNodeName();
        node->getNodeModel()->setName(nodeName);
    }

    node->setPos(node->getNodeModel()->getPosition());
    nodesByName.insert(nodeName, node);

    QGraphicsScene::addItem(node);
}

/*! Attempts to add the given edge to the sheet. If an edge already connects the two specified nodes
 *  together, then the process is aborted and the edge removed from the scene. This prevents hidden lines
 *  and unexpected behaviour when simulating the schematic with the ns3 plc module.*/
void DiagramSheet::addEdge(PLCGraphicsEdgeItem * edge){
    if(!edge){
        return;
    }

    //Make sure no other line already connects the two nodes.
    for(int i = 0; i < this->edges.length(); i++){
        if(edge->getEdgeModel()->connectsSameNodes(*edges.at(i)->getEdgeModel())){

            //This edge could be one just added through an editing operation, so make sure our
            //there's nothing hanging around in our editing state.
            if(this->edge != 0){
                this->removeItem(this->edge);
                delete(this->edge);
            }
            return;
        }
    }

    this->edges.append(edge);

    //Double check that the edge isn't already in the scene before trying to add it in
    //(Edges which have been drawn by hand will already be in the scene during editing/placement)
    if(edge->scene() != this){
        QGraphicsScene::addItem(edge);
    }
}

/*! Removes a node from the scene with the given pointer to a PLCGraphicsNodeItem.
 *  If the node doesn't exist in the scene, nothing will be done.

 *  Removing a node from the sheet does not behave in the same way as removing a QGraphicsItem from a QGraphics View.
 *  Instead, the node is completely deleted, as it is owned by the sheet, and shouldn't be carried around
 *  by anything else. Deleted nodes also delete any edges that were connected to them, preventing the
 *  existence of any edges with 'dangling' ends. */
void DiagramSheet::removeNode(PLCGraphicsNodeItem *node){

    if(!node){
        return;
    }

    for(int i = 0; i < edges.length(); i++){
        if((edges.at(i)->getEdgeModel()->getFromNode() == node->getNodeModel()->getName()) ||
                (edges.at(i)->getEdgeModel()->getToNode() == node->getNodeModel()->getName())){
            removeEdge(edges.at(i));
            i--;
        }

    }

    this->nodesByName.remove(node->getNodeModel()->getName());

    QGraphicsScene::removeItem(node);


    delete(node);
}

/*! \overload
 *  Removes a given node from the sheet by Name. Again, if the node doesn't exist in the
 *  scene this function does nothing. */
void DiagramSheet::removeNode(const QString& nodeName){
    PLCGraphicsNodeItem * node = nodesByName.value(nodeName);

    if(!node){
        return;
    }

    removeNode(node);
}



/*! Removes an edge from the scene, just like DiagramSheet::removeNode, edges are deleted
 *  completely, and not just removed from the scene. */
void DiagramSheet::removeEdge(PLCGraphicsEdgeItem *edge){
    if(!edge){
        return;
    }

    QGraphicsScene::removeItem(edge);

    int index = edges.indexOf(edge);
    if(index >= 0){
        edges.removeAt(index);
    }

    delete(edge);
}

/*! Returns true if a node already exists in the sheet with the specified name. */
bool DiagramSheet::nameExists(QString name){
    return nodesByName.contains(name);
}

/*! Generates a unique name that can be used as a default name for a new node being added to the sheet */
QString DiagramSheet::generateNodeName(){
    QString name;
    int i = nodesByName.count();
    do {
        name = QString("Node") + QString::number(i++);
    }while(this->nodesByName.contains(name));

    return name;
}


/*! Handles mouse clicks on the scene if necessary. This function manages placing edges and
 *  new nodes in the sheet, as well as selection and movement of existing items within the sheet */
void DiagramSheet::mousePressEvent(QGraphicsSceneMouseEvent *event){

    event->accept();

    if(sceneEditMode == LineMode){
        event->accept();

        QList<QGraphicsItem*> hoveredItems = items(event->scenePos());
        PLCGraphicsNodeItem* hoveredItem = 0;

        for(int i = 0; i < hoveredItems.length(); i++){
            hoveredItem = qgraphicsitem_cast<PLCGraphicsNodeItem*>(hoveredItems.at(i));
            if(hoveredItem != 0){
                break;
            }
        }

        if(hoveredItem != 0){

            if(!edge){
                edge = new PLCGraphicsEdgeItem(hoveredItem, event->scenePos());
                this->addItem(edge);
                return;
            }
            else if(hoveredItem != edge->getFromNode()){
                edge->setToNode(hoveredItem);
                addEdge(edge);
                edge = 0;
                return;
            }
        }

        return;
    }

    if(sceneEditMode == PlacementMode){
        node = 0;
        placeNewNode(true);
        node->setPos(event->scenePos());
    }

    else{
        QGraphicsScene::mousePressEvent(event);
    }
}

/*! Handles click-and-release and click-and-drag for the addition of edges to the scene */
void DiagramSheet::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){

    if((sceneEditMode == LineMode) && edge){

        QList<QGraphicsItem*> hoveredItems = items(event->scenePos());
        PLCGraphicsNodeItem* hoveredItem = 0;

        for(int i = 0; i < hoveredItems.length(); i++){
            hoveredItem = qgraphicsitem_cast<PLCGraphicsNodeItem*>(hoveredItems.at(i));
            if(hoveredItem != 0){
                break;
            }
        }

        if(hoveredItem != 0 && hoveredItem != edge->getFromNode()){
            edge->setToNode(hoveredItem);
            addEdge(edge);
            edge = 0;
            return;
        }
    }

    QGraphicsScene::mouseReleaseEvent(event);
    return;
}

/*! This function makes up for the fact that the tab-handling functions are not virtual, which means
 *  custom functionality implemented in DiagramSheet::focusNextPrevChild wasn't accessible.
 *  This is an inelegant solution which will hopefully be addressed in the future. */
bool DiagramSheet::event(QEvent *event){
    if(event->type() == QEvent::KeyPress){
        QKeyEvent * k = static_cast<QKeyEvent *>(event);

        /*This is cribbed directly from the QGraphicsScene source because focusNextPrevChild is NOT VIRTUAL*/
        //TODO Check source more and file a bug report about it if it doesn't make any sense how it is.
        if (k->key() == Qt::Key_Tab || k->key() == Qt::Key_Backtab) {
            if (!(k->modifiers() & (Qt::ControlModifier | Qt::AltModifier))) {  //### Add MetaModifier?
                bool res = false;
                if (k->key() == Qt::Key_Backtab
                    || (k->key() == Qt::Key_Tab && (k->modifiers() & Qt::ShiftModifier))) {
                    res = focusNextPrevChild(false);
                } else if (k->key() == Qt::Key_Tab) {
                    res = focusNextPrevChild(true);
                }
                if (!res)
                    event->ignore();
                return true;
            }
        }
    }
    return QGraphicsScene::event(event);
}

/*! Makes sure nodes/edges in the process of being placed are updated with the
 *  position of the cursor
 */
void DiagramSheet::mouseMoveEvent(QGraphicsSceneMouseEvent *event){

    //Move the line around with the cursor
    if(sceneEditMode == LineMode && edge != 0){
        edge->setToPoint(event->scenePos());
    }

    if(sceneEditMode == PlacementMode && node != 0){
        node->setPos(event->scenePos());
    }

    else{
        QGraphicsScene::mouseMoveEvent(event);
    }

}

/*! Custom tab-handling function which behaves slightly better than the default implementation.
 *  Items which are already selected will not be "tabbable to" directly, this function instead
 *  makes sure that, if possible, a new untabbed graphics item will be selected.
*/
bool DiagramSheet::focusNextPrevChild(bool next){

    if(items().length() == 0){
        return false;
    }

    if(selectedItems().length() == 0){
        this->items().first()->setSelected(true);
        return true;
    }

    else {
        QGraphicsItem* selected = (next)?selectedItems().last():selectedItems().first();
        int selectedIndex = items().indexOf(selected, 0);
        clearSelection();

        //Index CANNOT be -1 just fyi
        if(next && (selectedIndex == items().length() - 1)){
            items().first()->setSelected(true);
            return true;
        }

        if((!next) && (selectedIndex == 0)){
            items().last()->setSelected(true);
            return true;
        }

        items().at(selectedIndex + (next?1:-1))->setSelected(true);
        return true;
    }
}

/*! This mess handles the various types of keypresses that should be handled directly by the scene. Things like
 *  pushing the delete key to remove selected items, and changing the editing mode through the keyboard.
 *
 *  This function should be sanitized in the future to make sure that it adhere's to qt paradigms and is not
 *  functioning like a workaround when there is a more elegant solution available - Keys should also be
 *  remappable which is not possible with this solution
*/
void DiagramSheet::keyPressEvent(QKeyEvent *event){

    switch(event->key()){
    case Qt::Key_Escape:
        //////////////////TODO: Move this to a separate function (cancelEditingOperation)//////////

        if(edge){
            removeItem(edge);
            delete(edge);
            edge = 0;
        }


        if(sceneEditMode == PlacementMode){
            if(node){
                removeNode(node);
                node = 0;
            }

            sceneEditMode = SelectionMode;
        }

        if(sceneEditMode == LineMode){
            sceneEditMode = SelectionMode;
        }

        ////////////////////////////////////////////////////////////////////

        event->accept();
        break;


    case Qt::Key_Delete:


        //////////////////TODO: Move this to a separate function(remove selected)//////////

        //Be very careful to delete one item at a time.
        //Symbols will delete connected lines even if they are not selected!
        while(selectedItems().length() != 0){
            QGraphicsItem * item = selectedItems().first();
            PLCGraphicsNodeItem* node = qgraphicsitem_cast<PLCGraphicsNodeItem*>(item);
            PLCGraphicsEdgeItem* edge = qgraphicsitem_cast<PLCGraphicsEdgeItem*>(item);

            if(node){
                removeNode(node);
            }

            else if(edge){
                removeEdge(edge);
                edge = 0;
            }

            else{
                qDebug("Unidentified item type.... BUG!");
            }
        }
        ////////////////////////////////////////////////////////////////////
        event->accept();
        break;

    case Qt::Key_Alt:
        showAnnotations = !showAnnotations;
        event->accept();
        break;

    default:
        QGraphicsScene::keyPressEvent(event);
        break;
    }

    return;
}

/*! This function initializes placing a new node into the scene. Can be connected
 *  to other UI elements or actions which should initiate the editing operation
*/
void DiagramSheet::placeNewNode(bool checked){
    if(!checked){
        return;
    }

    setSelectionMode(true);

    if(this->node == 0){
        this->node = new PLCGraphicsNodeItem();
        this->addNode(node);
    }

    this->sceneEditMode = PlacementMode;
}

/*! Changes the editing mode to LineMode, which facilitates the addition of edges
 *  to the diagram */
void DiagramSheet::setLineMode(bool checked){
    if(!checked){
        return;
    }
    setSelectionMode(true);

    this->sceneEditMode = LineMode;
}

/*! Changes the editing mode to SelectionMode, which is the default setting
 *  and allows the user to select and move items */
void DiagramSheet::setSelectionMode(bool checked){
    if(!checked){
        return;
    }

    if(node){
        removeNode(node);
        node = 0;
    }

    if(edge){
        removeItem(edge);
        delete(edge);
        edge = 0;
    }
    this->sceneEditMode = SelectionMode;
}

/*! Brings up a dialog for editing the sheets spectrum model settings. */
void DiagramSheet::configureSpectrumModel(){
    PLCSpectrumConfiguration config(&spectrumModel);
    config.exec();
}

/*! Converts this sheet's data into a representation that can be converted to
 *  JSON for storage, or loaded by other code for simulation/emulation purposes. */
PLCTopologyModel DiagramSheet::toTopologyModel(){
    PLCTopologyModel model;

    QList<NodeModel*> modelNodes;
    QList<EdgeModel*> modelEdges;

    for(int i = 0; i < this->edges.length(); i++){
        modelEdges << edges.at(i)->getEdgeModel();
    }

    QMap<QString, PLCGraphicsNodeItem*>::iterator it;
    for(it = nodesByName.begin(); it != nodesByName.end(); it++){
        modelNodes << (*it)->getNodeModel();
    }

    model.setSpectrumModel(this->spectrumModel);
    model.setEdgeList(modelEdges);
    model.setNodeList(modelNodes);

    return model;
}

/*! Converts this sheet directly into a JSON representation */
QByteArray DiagramSheet::toJSONData(){
    QVariantMap sceneMap = toTopologyModel().toVariantMap();

    QJson::Serializer serializer;
    QByteArray jsonData = serializer.serialize(sceneMap);

    return jsonData;
}


