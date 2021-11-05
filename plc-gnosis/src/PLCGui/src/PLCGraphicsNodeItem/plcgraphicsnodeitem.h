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

#ifndef PLCGRAPHICSNODEITEM_H
#define PLCGRAPHICSNODEITEM_H

#include <QGraphicsItem>
#include "nodemodel.h"


class EdgeModel;
class PLCGraphicsEdgeItem;

class PLCGraphicsNodeItem : public QGraphicsItem
{

public:

    enum {Type = UserType + 1};
    explicit PLCGraphicsNodeItem(QGraphicsItem *parent = 0);
    explicit PLCGraphicsNodeItem(NodeModel* node, QGraphicsItem* parent = 0);
    ~PLCGraphicsNodeItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget);
    virtual int type() const;
    QRectF boundingRect() const;

    NodeModel* getNodeModel(){ return node; }
    void installEdge(PLCGraphicsEdgeItem* edge);
    void uninstallEdge(PLCGraphicsEdgeItem* edge);
    

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);


private:
    NodeModel* node;
    QGraphicsTextItem* nodeLabel;
    QGraphicsTextItem* impedanceLabel;

    void setupLabelsAndFlags();
    void updateLabels();

    QList<PLCGraphicsEdgeItem *> associatedEdges;
};

#endif // PLCGRAPHICSNODEITEM_H
