#include "plcgraphicsedgeitem.h"
#include <QPainter>
#include <QDebug>
#include <algorithm>

#include "../EdgeConfiguration/edgeconfiguration.h"

QFont PLCGraphicsEdgeItem::labelFont("Ariel", 12);


PLCGraphicsEdgeItem::PLCGraphicsEdgeItem(PLCGraphicsNodeItem *fromNode, QPointF toPoint, QGraphicsItem *parent) :
    QGraphicsItem(parent), fontMetrics(labelFont)
{
    Q_ASSERT(fromNode != 0);

    this->edge = new EdgeModel(fromNode->getNodeModel()->getName(), "Disconnected");
    this->fromNode = fromNode;
    this->toNode = 0;

    setToPoint(toPoint);

    setup();
}

PLCGraphicsEdgeItem::PLCGraphicsEdgeItem(EdgeModel* edge, PLCGraphicsNodeItem* fromNode, PLCGraphicsNodeItem* toNode) :
    fontMetrics(labelFont)
{
    this->edge = edge;
    this->fromNode = fromNode;
    setToNode(toNode);

    setup();
}

void PLCGraphicsEdgeItem::setup(){
    setZValue(-1);
    setFlags(QGraphicsItem::ItemIsSelectable);

    linePen = QPen(Qt::black);

    labelFont = QFont("Consolas", 12);
    fontMetrics = QFontMetricsF(labelFont);

    fromNode->installEdge(this);

    updateGeometry();
}

PLCGraphicsEdgeItem::~PLCGraphicsEdgeItem(){
    delete(this->edge);

    fromNode->uninstallEdge(this);
    if(toNode != 0){
        toNode->uninstallEdge(this);
    }
}

void PLCGraphicsEdgeItem::updateGeometry(){
    prepareGeometryChange();

    lengthText = QString::number(edge->getLength()) + "m";

    lengthTextWidth = fontMetrics.boundingRect(lengthText).width();
    nameTextWidth = fontMetrics.boundingRect(edge->getName()).width();

    QPointF startPoint = fromNode->pos();
    QPointF endPoint = (toNode == 0)?toPoint:toNode->pos();
    QPointF midPoint = (startPoint + endPoint)/2.0;

    double dx = (endPoint.rx() - startPoint.rx());
    double dy = (endPoint.ry() - startPoint.ry());
    length = sqrt((dx*dx) + (dy*dy));

    double brLength = std::max(lengthTextWidth, std::max(nameTextWidth, length)) + linePen.widthF();
    double theta = atan2(-dy, dx)*180.0/acos(-1);

    //setTransform(QTransform::fromScale(1, -1), false);
    setPos(midPoint);
    setRotation(-theta);

    height = 2.0*fontMetrics.leading() + 2.0*fontMetrics.height();
    height += linePen.widthF();

    cachedBoundingRect = QRectF(-brLength/2.0, -height/2.0, brLength, height).normalized();
}

QRectF PLCGraphicsEdgeItem::boundingRect() const {
    return cachedBoundingRect;
}

void PLCGraphicsEdgeItem::setToPoint(QPointF point){
    this->toPoint = point;
    updateGeometry();
}

void PLCGraphicsEdgeItem::setToNode(PLCGraphicsNodeItem *node){
    this->toNode = node;
    this->edge->setToNode(node->getNodeModel()->getName());
    toNode->installEdge(this);
    updateGeometry();
}

void PLCGraphicsEdgeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event);
    EdgeConfiguration * edgeConfig = new EdgeConfiguration(getEdgeModel());
    edgeConfig->exec();
    updateGeometry();
    delete edgeConfig;
}

void PLCGraphicsEdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    //TODO: Draw an indicator of the directionality of this edge (especially for twoports)
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QPen drawPen = linePen;
    QBrush drawBrush(Qt::black);

    if(this->isSelected()){
        drawPen.setColor(Qt::magenta);
        drawBrush.setColor(Qt::magenta);
    }

    if(!this->edge->isCableModel()){
        linePen.setStyle(Qt::DashDotLine);
    }
    else{
        linePen.setStyle(Qt::SolidLine);
    }

    painter->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing, true);

    painter->setPen(drawPen);
    painter->setBrush(drawBrush);
    painter->drawLine(-length/2.0, 0, length/2.0, 0.0);

    painter->drawText(-lengthTextWidth/2.0, -fontMetrics.descent() - linePen.widthF()/2.0, lengthText);
    painter->drawText(-nameTextWidth/2.0, fontMetrics.ascent() + linePen.widthF()/2.0, edge->getName());
}
