/*
* Copyright (C) 2013-2014 The Communi Project
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
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
