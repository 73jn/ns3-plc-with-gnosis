#include "plcgraphicsnodeitem.h"
#include <QPainter>
#include <QPainterPath>
#include <math.h>
#include <QStyleOptionGraphicsItem>

#include "../NodeConfiguration/nodeconfiguration.h"
#include "../PLCGraphicsEdgeItem/plcgraphicsedgeitem.h"
#include <QDebug>
#include "edgemodel.h"



#define NODEITEM_RADIUS 20.0
#define NODE_RECT 0.707*NODEITEM_RADIUS

void PLCGraphicsNodeItem::setupLabelsAndFlags(){
    setZValue(0);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);

    this->nodeLabel = new QGraphicsTextItem(this);
    this->impedanceLabel = new QGraphicsTextItem(this);

    //this->nodeLabel->setCacheMode(QGraphicsItem::NoCache);
    //this->impedanceLabel->setCacheMode(QGraphicsItem::NoCache);


    nodeLabel->setPlainText("");
    impedanceLabel->setPlainText("");

    nodeLabel->setPos(NODEITEM_RADIUS, -NODEITEM_RADIUS);
    impedanceLabel->setPos(NODEITEM_RADIUS, 0);
}

void PLCGraphicsNodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event);

    NodeConfiguration * nodeConfig = new NodeConfiguration(getNodeModel());
    nodeConfig->exec();
    delete nodeConfig;
    updateLabels();
    update();
}


PLCGraphicsNodeItem::PLCGraphicsNodeItem(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    this->node = new NodeModel();
    setupLabelsAndFlags();
}

void PLCGraphicsNodeItem::updateLabels(){
    if(node->getNetDevice() != 0){
        nodeLabel->setPlainText(node->getNetDevice()->getName());
    }

    nodeLabel->setPlainText("");
}

PLCGraphicsNodeItem::PLCGraphicsNodeItem(NodeModel* node, QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    this->node = node;
    setupLabelsAndFlags();
}


PLCGraphicsNodeItem::~PLCGraphicsNodeItem(){
    delete(this->node);

}

void PLCGraphicsNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    QPainterPath path;

    Q_UNUSED(widget);
    Q_UNUSED(option);

    if(this->isSelected()){
        painter->setBrush(Qt::red);
        painter->setPen(Qt::red);
    }
    else {
        painter->setBrush(Qt::black);
        painter->setPen(Qt::black);
    }


    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter->setRenderHint(QPainter::Antialiasing, true);

    //Draw the net device symbol if node has any associated net devices
    if(node->getNetDevice() != 0){
        painter->setBrush(Qt::white);
        path.moveTo(0, 0.8*NODEITEM_RADIUS);
        path.lineTo(0.8*NODEITEM_RADIUS*cos(M_PI/6.0), -0.8*NODEITEM_RADIUS*sin(M_PI/6.0));
        path.lineTo(0.8*NODEITEM_RADIUS*cos(5.0*M_PI/6.0), -0.8*NODEITEM_RADIUS*sin(5.0*M_PI/6.0));
        path.closeSubpath();

        painter->fillPath(path, painter->brush());
        painter->strokePath(path, painter->pen());

        //Draw circle if has transmitter
        if(node->getNetDevice()->transmitterEnabled()){
            painter->setBrush(Qt::transparent);
            painter->drawEllipse(QPointF(0, 0), 0.6*NODEITEM_RADIUS, 0.6*NODEITEM_RADIUS);
        }

        //Fill in triangle if has receiver
        if(node->getNetDevice()->receiverEnabled()){
            painter->fillPath(path, QBrush(Qt::black));
         }
    }
    else {
        //Draw basic node
        painter->drawEllipse(QPointF(0, 0), 0.20*NODEITEM_RADIUS, 0.20*NODEITEM_RADIUS);
    }


    if(node->getHasOutlet()){
        painter->setBrush(Qt::white);
        painter->drawEllipse(QPointF(0, 0), 0.32*NODEITEM_RADIUS, 0.32*NODEITEM_RADIUS);
    }

    if(node->getNoiseSource() != 0){

        painter->drawArc(-NODEITEM_RADIUS, -NODEITEM_RADIUS, 2*NODEITEM_RADIUS, 2*NODEITEM_RADIUS, 720, 1440);
        painter->drawArc(-0.8*NODEITEM_RADIUS, -0.8*NODEITEM_RADIUS, 1.6*NODEITEM_RADIUS, 1.6*NODEITEM_RADIUS, 720, 1440);

        painter->setBrush(Qt::transparent);
    }
}

QRectF PLCGraphicsNodeItem::boundingRect() const{
    return QRectF(-NODEITEM_RADIUS, -NODEITEM_RADIUS, NODEITEM_RADIUS*2.0, NODEITEM_RADIUS*2.0);
}

int PLCGraphicsNodeItem::type() const{
    return Type;
}



void PLCGraphicsNodeItem::installEdge(PLCGraphicsEdgeItem * newEdge){

    if(newEdge == 0){
        return;
    }

    foreach(PLCGraphicsEdgeItem* edge, associatedEdges){
        if(newEdge == edge){
            return;
        }
    }

    associatedEdges.append(newEdge);
}

void PLCGraphicsNodeItem::uninstallEdge(PLCGraphicsEdgeItem * edge){
    associatedEdges.removeAll(edge);
}


QVariant PLCGraphicsNodeItem::itemChange(GraphicsItemChange change, const QVariant &value){
    if(change == QGraphicsItem::ItemPositionChange){
        QPointF pos = value.toPointF();
        pos /= 8;
        QPointF newPos = pos.toPoint();
        newPos *= 8;

        return newPos;
    }


    if(change == QGraphicsItem::ItemPositionHasChanged){

        foreach(PLCGraphicsEdgeItem* edge, associatedEdges){
            edge->updateGeometry();
        }

        this->getNodeModel()->setPosition(value.toPointF());
    }

    return value;
}
