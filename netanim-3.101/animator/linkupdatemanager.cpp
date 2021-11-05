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

#include "linkupdatemanager.h"
#include "debug/xdebug.h"

namespace netanim {

LinkUpdateManager::LinkUpdateManager()
{

}

void
LinkUpdateManager::addLinkUpdate(qreal updateTime, AnimLink * link, QString linkDescription)
{
    LinkDescription_t ld;
    ld.updateTime = updateTime;
    ld.link = link;
    ld.description = linkDescription;
    m_timeLinkDescriptions.push_back(ld);
    m_currentIterator = m_timeLinkDescriptions.begin();
    m_lastTime = 0;
    //qDebug(m_timeLinkDescriptions.size() , "map count");

}

void
LinkUpdateManager::systemReset()
{
    m_timeLinkDescriptions.clear();
}

bool
LinkUpdateManager::updateLinks(qreal currentTime)
{
    bool updated = false;
    if (m_timeLinkDescriptions.empty())
        return false;
    if (currentTime < m_lastTime)
    {
        m_currentIterator = m_timeLinkDescriptions.begin();
        updated = true;
    }
    m_lastTime = currentTime;
    for(; m_currentIterator != m_timeLinkDescriptions.end(); ++m_currentIterator)
    {
        //qDebug(0, "moved iterator");
        //qDebug(currentTime, "current TIme");
        LinkDescription_t ld = *m_currentIterator;
        qreal t = ld.updateTime;
        //qDebug(t, "t");

        if (t > currentTime)
        {
            if(m_currentIterator != m_timeLinkDescriptions.begin())
                --m_currentIterator;
            return updated;
        }
        ld.link->updateCurrentLinkDescription(ld.description);
        updated = true;
        //qDebug(ld.link->toString() + " " + QString(ld.description) + "descr");
    }
    return updated;
}



} // namespace netanim
