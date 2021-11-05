/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: John Abraham <john.abraham@gatech.edu>
 */

#ifndef ANIMNODE_H
#define ANIMNODE_H

#include "main/common.h"
#include <QtGui/QGraphicsItem>
#include <QColor>

namespace netanim {

enum AnimNodeShape
{
    CIRCLE,
    RECTANGLE,
    IMAGE
};

class AnimNode
{
public:
    explicit AnimNode(uint32_t nodeId,
                      AnimNodeShape shape,
                      qreal width = 1,
                      qreal height = 1,
                      QString description ="",
                      QColor * color = 0);
    ~AnimNode();

    QGraphicsItem * getGraphicsItem();
    AnimNodeShape getNodeShape();
    void setPos(QPointF pos);
    void setSize(qreal size);
    uint32_t getNodeId();
    QGraphicsTextItem * getNodeIdTextItem();
    void showNodeIdText(bool show);
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setDescription(QString description);
    void setVisible(bool visible=true);
    void updateNode(QColor c, QString description, bool visible);

private:
    uint32_t m_nodeId;
    AnimNodeShape m_shape;
    qreal m_width;
    qreal m_height;
    QString m_description;
    QGraphicsItem * m_graphicsItem;
    QColor * m_color;
    QGraphicsTextItem * m_graphicsNodeIdTextItem;
    bool m_visible;

    void setRect(QPointF pos);

    
signals:
    
public slots:
    
};

} // namespace netanim
#endif // ANIMNODE_H
