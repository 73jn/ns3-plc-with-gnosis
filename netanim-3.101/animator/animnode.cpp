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


#include "animnode.h"
#include "debug/xdebug.h"
#include <QtGui/QGraphicsEllipseItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QBrush>
#include <QtGui/QPen>

namespace netanim {

AnimNode::AnimNode(uint32_t nodeId,
                   AnimNodeShape shape,
                   qreal width,
                   qreal height,
                   QString description,
                   QColor * color) :
    m_nodeId(nodeId),
    m_shape(shape),
    m_width(width),
    m_height(height),
    m_description(description),
    m_color(color),
    m_graphicsNodeIdTextItem(0)
{
    switch (shape)
    {
        case CIRCLE:
        m_graphicsItem = new QGraphicsEllipseItem;
            break;
        case RECTANGLE:
        m_graphicsItem = new QGraphicsRectItem;
            break;
        case IMAGE:
            m_graphicsItem = new QGraphicsPixmapItem;
        break;
        default:
            m_graphicsItem = 0;
            // TODO
        break;
    }
    QString nodeDescription;
    if(m_description != "")
        nodeDescription = m_description;
    else
        nodeDescription = QString::number(m_nodeId);
    m_graphicsNodeIdTextItem = new QGraphicsTextItem(nodeDescription);
    m_graphicsNodeIdTextItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_visible = true;

}


AnimNode::~AnimNode()
{
    if(m_graphicsItem)
        delete m_graphicsItem;
    if(m_graphicsNodeIdTextItem)
        delete m_graphicsNodeIdTextItem;
    if(m_color)
        delete m_color;
}

QGraphicsItem *
AnimNode::getGraphicsItem()
{
    return m_graphicsItem;
}



void
AnimNode::setRect(QPointF pos)
{

    //qDebug(pos,"SetRect");
    QBrush brush;
    if (!m_color)
    {
        brush.setColor(Qt::red);
    }
    else
    {
        brush.setColor(*m_color);
    }
    brush.setStyle(Qt::SolidPattern);
    QPen pen;
    pen.setColor(Qt::red);
    switch(m_shape)
    {
        case CIRCLE:
        {
            QGraphicsEllipseItem * g = qgraphicsitem_cast<QGraphicsEllipseItem*> (m_graphicsItem);
            g->setRect(pos.x()-m_width/2, pos.y()-m_height/2, m_width, m_height);
            g->setBrush(brush);
            g->setPen(pen);

            break;
        }
        case RECTANGLE:
        {
            QGraphicsRectItem * g = qgraphicsitem_cast<QGraphicsRectItem*> (m_graphicsItem);
            g->setRect(pos.x(), pos.y(), m_width, m_height);
            g->setBrush(brush);
            break;
        }
        case IMAGE:
        break;
    }



}


uint32_t
AnimNode::getNodeId()
{
    return m_nodeId;
}

void
AnimNode::setSize(qreal size)
{
    m_width = m_height = size;
    //qDebug("SetSize");
    setRect(m_graphicsItem->sceneBoundingRect().center());
}



AnimNodeShape
AnimNode::getNodeShape()
{
    return m_shape;
}


void
AnimNode::setPos(QPointF pos)
{
    setRect(pos);
}

QGraphicsTextItem *
AnimNode::getNodeIdTextItem()
{
    return m_graphicsNodeIdTextItem;
}


void
AnimNode::updateNode(QColor c, QString description, bool visible)
{
    setColor(c.red(), c.green(), c.blue());
    m_description = description;
    if(m_description == "")
        m_description = QString::number(m_nodeId);
    m_graphicsNodeIdTextItem->setPlainText(m_description);

    m_visible = visible;
    setVisible(m_visible);
}


void
AnimNode::showNodeIdText(bool show)
{

    m_graphicsNodeIdTextItem->setPos(m_graphicsItem->sceneBoundingRect().bottomRight());
    m_graphicsNodeIdTextItem->setVisible(show);
    if(show == true && m_visible == false)
    {
        m_graphicsNodeIdTextItem->setVisible(m_visible);
    }


}

void
AnimNode::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    QColor * c = new QColor(r, g, b);
    if (m_color)
        delete m_color;
    m_color = c;
}

void
AnimNode::setDescription(QString description)
{
    m_description = description;
    m_graphicsNodeIdTextItem->setPlainText(description);
}

void
AnimNode::setVisible(bool visible)
{
    m_graphicsItem->setVisible(visible);
    m_graphicsNodeIdTextItem->setVisible(visible);

    if(visible == true && m_visible == false)
    {
        m_graphicsItem->setVisible(m_visible);
        m_graphicsNodeIdTextItem->setVisible(m_visible);
    }

}

} // namespace netanim
