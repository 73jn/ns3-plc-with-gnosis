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

#include "nodeupdatemanager.h"
#include "debug/xdebug.h"

namespace netanim {

NodeUpdateManager::NodeUpdateManager()
{

}

void
NodeUpdateManager::systemReset()
{
    m_nodeUpdates.clear();
}

bool
NodeUpdateManager::updateNodes(qreal currentTime)
{
    bool updated = false;
    if (m_nodeUpdates.empty())
        return false;
    if (currentTime < m_lastTime)
    {
        m_currentIterator = m_nodeUpdates.begin();
        updated = true;
    }
    m_lastTime = currentTime;
    for(; m_currentIterator != m_nodeUpdates.end(); ++m_currentIterator)
    {
        //qDebug(0, "moved iterator");
        //qDebug(currentTime, "current TIme");
        NodeUpdate_t nu = *m_currentIterator;
        qreal t = nu.updateTime;
        //qDebug(t, "t");

        if (t > currentTime)
        {
            if(m_currentIterator != m_nodeUpdates.begin())
                --m_currentIterator;
            return updated;
        }
        nu.animNode->updateNode(QColor(nu.r, nu.g, nu.b), nu.description, nu.visible);
        updated = true;
        //qDebug(ld.link->toString() + " " + QString(ld.description) + "descr");
    }

    return updated;

}


void
NodeUpdateManager::addNodeUpdate(qreal updateTime, AnimNode * animNode, uint8_t r, uint8_t g, uint8_t b, QString description, bool visible)
{
    NodeUpdate_t nu;
    nu.animNode = animNode;
    nu.updateTime = updateTime;
    nu.r = r;
    nu.g = g;
    nu.b = b;
    nu.description = description;
    nu.visible = visible;
    m_nodeUpdates.push_back(nu);
    m_currentIterator = m_nodeUpdates.begin();
    m_lastTime = 0;
}

} // namespace netanim
