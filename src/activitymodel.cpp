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

#include "activitymodel.h"
#include <IrcMessage>
#include <IrcBuffer>

ActivityModel::ActivityModel(QObject* parent) : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> ActivityModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "title";
    return roles;
}

int ActivityModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_buffers.count();
}

QVariant ActivityModel::data(const QModelIndex& index, int role) const
{
    const int row = index.row();
    if (row < 0 || row >= m_buffers.count() || role != Qt::DisplayRole)
        return QVariant();
    return m_buffers.at(row);
}

void ActivityModel::clear()
{
    beginResetModel();
    m_buffers.clear();
    endResetModel();
}

void ActivityModel::receive(IrcBuffer* buffer, IrcMessage* message)
{
    Q_UNUSED(message);
    QString title = buffer->title();
    // Check if buffer is already in the top
    for (int i = 0; i < m_buffers.count(); ++i) {
        if (m_buffers.at(i) == title) {
            // If buffer is not the first already, promote it to the top
            if (i > 0) {
                beginMoveRows(QModelIndex(), i, i, QModelIndex(), 0);
                m_buffers.move(i, 0);
                endMoveRows();
            }
            return;
        }
    }

    if (m_buffers.count() < 5) {
        beginInsertRows(QModelIndex(), 0, 0);
        m_buffers.prepend(title);
        endInsertRows();
    } else {
        m_buffers.prepend(title);
        m_buffers.removeAt(4);
        emit dataChanged(index(0), index(4));
    }
}
