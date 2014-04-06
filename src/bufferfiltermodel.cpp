/*
  Copyright (C) 2013-2014 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Communi Project nor the names of its
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

#include "bufferfiltermodel.h"
#include "messagestorage.h"
#include "messagemodel.h"
#include <IrcBuffer>

BufferFilterModel::BufferFilterModel(MessageStorage* storage) :
    QSortFilterProxyModel(storage), m_status(0), m_storage(storage)
{
    setDynamicSortFilter(true);
}

int BufferFilterModel::filterStatus() const
{
    return m_status;
}

void BufferFilterModel::setFilterStatus(int status)
{
    if (m_status != status) {
        m_status = status;
        emit filterStatusChanged();
        invalidate();
    }
}

QString BufferFilterModel::filterString() const
{
    return m_filter;
}

void BufferFilterModel::setFilterString(const QString& filter)
{
    if (m_filter != filter) {
        m_filter = filter;
        emit filterStringChanged();
        invalidate();
    }
}

bool BufferFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (m_status == 0 && m_filter.isEmpty())
        return true;

    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    IrcBuffer* buffer = index.data(Irc::BufferRole).value<IrcBuffer*>();
    if (buffer && !buffer->isSticky()) {
        if (!m_filter.isEmpty() && !buffer->title().contains(m_filter, Qt::CaseInsensitive))
            return false;
        if (m_status > 0) {
            MessageModel* model = m_storage->model(buffer);
            if (model) {
                if (m_status == 1 && model->badge() <= 0)
                    return false;
                if (m_status == 2 && model->activeHighlights() <= 0)
                    return false;
            }
        }
    }
    return true;
}
