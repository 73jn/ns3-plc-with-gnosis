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

#include "animator/animlink.h"
#include "debug/xdebug.h"


#include <QPainter>

namespace netanim {


AnimLink::AnimLink(uint32_t fromId, uint32_t toId,
                   QLineF line, QString pointADescription,
                   QString pointBDescription, QString linkDescription):QGraphicsLineItem(line),
                   m_fromId(fromId), m_toId (toId)
{
    m_pointADescription = 0;
    m_pointBDescription = 0;
    m_currentLinkDescription = 0;
    if (pointADescription != "")
    {
        m_pointADescription = new QString(pointADescription);
    }
    if (pointBDescription != "")
    {
        m_pointBDescription = new QString(pointBDescription);
    }
    m_originalLinkDescription = new QString("");
    if (linkDescription != "")
    {
        m_currentLinkDescription = new QString(linkDescription);
        *m_originalLinkDescription = linkDescription;
    }
}

AnimLink::~AnimLink()
{
    if (m_pointADescription)
        delete m_pointADescription;
    if (m_pointBDescription)
        delete m_pointBDescription;
    if (m_currentLinkDescription)
        delete m_currentLinkDescription;
    if (m_originalLinkDescription)
        delete m_originalLinkDescription;

}


QPointF
AnimLink::getLinkDescriptionCenter(QPainter * painter , QPointF * offset)
{
    QFontMetrics fm = painter->fontMetrics();
    qreal x = (line().length() - fm.width(m_currentLinkDescription->toAscii().data()))/2;
    QPointF pOffset = line().p1().x() < line().p2().x()? line().p1():line().p2();
    *offset = pOffset;
    QPointF p = QPointF(x, -1);
    return p;

}

void
AnimLink::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;
    painter->drawLine(line());
    QFont font;
    font.setPointSize(3);
    QPen pen;
    pen.setColor(Qt::darkRed);
    painter->setFont(font);
    painter->setPen(pen);
    /*if (m_pointADescription)
        painter->drawText(line().p1(), *m_pointADescription);
    if (m_pointBDescription)
        painter->drawText(line().p2(), *m_pointBDescription);*/

    if (m_currentLinkDescription)
    {
        QPointF offset;
        QPointF center = getLinkDescriptionCenter(painter, &offset);
        painter->save();
        painter->translate(offset);

        if (offset != line().p1())
        {
            painter->rotate(180-line().angle());
        }
        else
        {
            painter->rotate(-line().angle());
        }
        painter->drawText(center, *m_currentLinkDescription);
        painter->restore();
    }
}


void
AnimLink::updateCurrentLinkDescription(QString linkDescription)
{
    if(!m_currentLinkDescription)
    {
        m_currentLinkDescription = new QString(linkDescription);
        return;
    }
    *m_currentLinkDescription = linkDescription;
}

void
AnimLink::resetCurrentLinkDescription()
{
    if (m_originalLinkDescription)
        m_currentLinkDescription = m_originalLinkDescription;
}

QString
AnimLink::toString()
{
    QString s = QString("From:") + QString::number(m_fromId) + " To:" + QString::number(m_toId);
    return s;
}

} // namespace netanim
