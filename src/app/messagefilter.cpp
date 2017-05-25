/*
  Copyright (C) 2013-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "messagefilter.h"
#include "messagerole.h"
#include "messagemodel.h"
#include <IrcMessage>
#include <IrcBuffer>

IRC_USE_NAMESPACE

MessageFilter::MessageFilter(QObject* parent) :
    QSortFilterProxyModel(parent),
    m_events(true),
    m_topicMessages(true)
{
    setDynamicSortFilter(true);
}

QObject* MessageFilter::source() const
{
    return QSortFilterProxyModel::sourceModel();
}

void MessageFilter::setSource(QObject* source)
{
    QSortFilterProxyModel::setSourceModel(qobject_cast<QAbstractItemModel*>(source));
}

bool MessageFilter::showEvents() const
{
    return m_events;
}

void MessageFilter::setShowEvents(bool show)
{
    if (m_events != show) {
        m_events = show;
        emit showEventsChanged();
        invalidateFilter();
    }
}

bool MessageFilter::showTopicMessages() const
{
    return m_topicMessages;
}

void MessageFilter::setShowTopicMessages(bool show)
{
    if (m_topicMessages != show) {
        m_topicMessages = show;
        emit showTopicMessagesChanged();
        invalidateFilter();
    }
}

bool MessageFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (!m_events || !m_topicMessages) {
        const QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        switch (index.data(TypeRole).toInt()) {
        case IrcMessage::Join:
        case IrcMessage::Part:
        case IrcMessage::Quit:
            return m_events;
        case IrcMessage::Topic:
        case IrcMessage::Numeric:
        case IrcMessage::Names:
            return m_topicMessages || !dynamic_cast<const MessageModel*>(sourceModel())->buffer()->isChannel();
        default:
            return true;
        }
    }
    return true;
}
