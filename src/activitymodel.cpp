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
#include "messagemodel.h"
#include <IrcMessage>
#include <IrcBuffer>

static const int ROWCOUNT = 5;

ActivityModel::ActivityModel(QObject* parent) : QAbstractListModel(parent)
{
}

QHash<int, QByteArray> ActivityModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "buffer";
    return roles;
}

int ActivityModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ROWCOUNT;
}

QVariant ActivityModel::data(const QModelIndex& index, int role) const
{
    const int row = index.row();
    if (row < 0 || row >= ROWCOUNT || role != Qt::UserRole)
        return QVariant();
    MessageModel* model = m_models.value(row);
    if (model)
        return QVariant::fromValue(model->buffer());
    return QVariant();
}

void ActivityModel::add(MessageModel* model)
{
    if (model && !m_models.contains(model)) {
        connect(model, SIGNAL(badgeChanged()), this, SLOT(onBadgeChanged()));
        connect(model, SIGNAL(activeHighlightsChanged()), this, SLOT(onActiveHighlightsChanged()));
        m_models.prepend(model);
        emitDataChanged(0);
    }
}

void ActivityModel::remove(MessageModel* model)
{
    const int row = m_models.indexOf(model);
    if (row != -1) {
        disconnect(model, SIGNAL(badgeChanged()), this, SLOT(onBadgeChanged()));
        disconnect(model, SIGNAL(activeHighlightsChanged()), this, SLOT(onActiveHighlightsChanged()));
        m_models.removeAt(row);
        emitDataChanged(row);
    }
}

void ActivityModel::promote(MessageModel* model)
{
    const int row = m_models.indexOf(model);
    if (row > 0) {
        m_models.move(row, 0);
        emitDataChanged(0);
    }
}

void ActivityModel::demote(MessageModel* model)
{
    const int from = m_models.indexOf(model);
    if (from >= 0 && from < m_models.count() - 1) {
        int to = from + 1;
        while (to < m_models.count()) {
            MessageModel* model = m_models.at(to);
            if (model->badge() == 0 && model->activeHighlights() == 0)
                break;
            ++to;
        }
        if (to - 1 > from && to < m_models.count()) {
            m_models.move(from, to - 1);
            emitDataChanged(from);
        }
    }
}

void ActivityModel::onBadgeChanged()
{
    MessageModel* model = qobject_cast<MessageModel*>(sender());
    if (model) {
        if (model->badge() > 0)
            promote(model);
        else
            demote(model);
    }
}

void ActivityModel::onActiveHighlightsChanged()
{
    MessageModel* model = qobject_cast<MessageModel*>(sender());
    if (model) {
        const int highlights = model->activeHighlights();
        if (highlights > 0)
            promote(model);
    }
}

void ActivityModel::emitDataChanged(int from)
{
    if (from < ROWCOUNT)
        emit dataChanged(index(from), index(qMin(m_models.count(), ROWCOUNT) - 1));
}
