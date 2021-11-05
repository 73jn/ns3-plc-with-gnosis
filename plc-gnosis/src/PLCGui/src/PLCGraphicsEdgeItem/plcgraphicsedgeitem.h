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

#ifndef PLCGRAPHICSEDGEITEM_H
#define PLCGRAPHICSEDGEITEM_H

#include <QGraphicsItem>
#include "edgemodel.h"
#include "../PLCGraphicsNodeItem/plcgraphicsnodeitem.h"
#include <QGraphicsTextItem>
#include <QFontMetricsF>
#include <QPen>

class PLCGraphicsEdgeItem : public QGraphicsItem
{
public:

    enum {Type = UserType + 2};

    explicit PLCGraphicsEdgeItem(EdgeModel* edge, QGraphicsItem* parent = 0);
    explicit PLCGraphicsEdgeItem(PLCGraphicsNodeItem * fromNode, QPointF toPoint, QGraphicsItem* parent = 0);
    explicit PLCGraphicsEdgeItem(EdgeModel *edge, PLCGraphicsNodeItem* fromNode, PLCGraphicsNodeItem* toNode);

    ~PLCGraphicsEdgeItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget);

    virtual int type() const { return Type; }

    QRectF boundingRect() const;

    void setToPoint(QPointF point);
    void setToNode(PLCGraphicsNodeItem * node);

    PLCGraphicsNodeItem* getFromNode() { return fromNode; }

    EdgeModel* getEdgeModel(){ return edge; }

    void updateGeometry();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    EdgeModel* edge;

    PLCGraphicsNodeItem * fromNode;
    PLCGraphicsNodeItem * toNode;

    QString lengthText;

    QPointF toPoint;

    static QFont labelFont;
    QFontMetricsF fontMetrics;

    QPen linePen;

    double length, height;
    double lengthTextWidth;
    double nameTextWidth;
    QRectF cachedBoundingRect;

    void setup();
};

#endif // PLCGRAPHICSEDGEITEM_H
