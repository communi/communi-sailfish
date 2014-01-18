/*
* Copyright (C) 2013-2014 J-P Nurmi <jpnurmi@gmail.com>
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

#include "messagemodel.h"
#include "messageformatter.h"
#include <IrcConnection>
#include <IrcMessage>
#include <IrcBuffer>

enum DataRole {
    SeenRole = Qt::UserRole,
    HighlightRole,
    EventRole
};

MessageModel::MessageModel(IrcBuffer* buffer) : QAbstractListModel(buffer),
    m_badge(0), m_current(false), m_visible(false), m_highlight(false),
    m_buffer(buffer), m_formatter(new MessageFormatter(this))
{
    m_formatter->setBuffer(buffer);
    m_formatter->setTimeStampFormat("hh:mm");
    m_formatter->setPrivateMessageNickFormat("%1:");

    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receive(IrcMessage*)));

    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(modelReset()), this, SIGNAL(countChanged()));
}

IrcBuffer* MessageModel::buffer() const
{
    return m_buffer;
}

int MessageModel::count() const
{
    return m_messages.count();
}

int MessageModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_messages.count();
}

bool MessageModel::isCurrent() const
{
    return m_current;
}

void MessageModel::setCurrent(bool current)
{
    if (m_current != current) {
        m_current = current;
        if (!current) {
            m_seen.fill(true);
        } else {
            setBadge(0);
            setActiveHighlight(false);
        }
        emit currentChanged();
    }
}

bool MessageModel::isVisible() const
{
    return m_visible;
}

void MessageModel::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        emit visibleChanged();
    }
}

bool MessageModel::activeHighlight() const
{
    return m_highlight;
}

void MessageModel::setActiveHighlight(bool highlight)
{
    if (m_highlight != highlight) {
        m_highlight = highlight;
        emit activeHighlightChanged();
    }
}

int MessageModel::badge() const
{
    return m_badge;
}

void MessageModel::setBadge(int badge)
{
    if (m_badge != badge) {
        m_badge = badge;
        emit badgeChanged();
    }
}

QHash<int, QByteArray> MessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "richtext";
    roles[Qt::EditRole] = "plaintext";
    roles[HighlightRole] = "highlight";
    roles[EventRole] = "event";
    roles[SeenRole] = "seen";
    return roles;
}

QVariant MessageModel::data(const QModelIndex& index, int role) const
{
    const int row = index.row();
    if (row < 0 || row >= m_messages.count())
        return QVariant();

    switch (role) {
    case HighlightRole:
        return m_messages.at(row).hilite;
    case EventRole:
        return m_messages.at(row).event;
    case SeenRole:
        return m_seen.at(row);
    case Qt::DisplayRole:
        return m_messages.at(row).richtext;
    case Qt::EditRole:
        return m_messages.at(row).plaintext;
    default:
        return QVariant();
    }
}

void MessageModel::receive(IrcMessage* message)
{
    MessageData data;
    data.plaintext = m_formatter->formatMessage(message, Qt::PlainText);
    if (!data.plaintext.isEmpty()) {
        data.event = (message->type() != IrcMessage::Private && message->type() != IrcMessage::Notice);
        if (!(message->flags() & IrcMessage::Own))
            data.hilite = message->property("content").toString().contains(message->connection()->nickName(), Qt::CaseInsensitive);
        data.richtext = m_formatter->formatMessage(message, Qt::RichText);
        bool seen = m_current && m_visible;
        append(data, seen);
        if (!m_current || !m_visible) {
            if (data.hilite || message->property("private").toBool()) {
                if (!m_current)
                    setActiveHighlight(true);
                if (!m_current || !m_visible)
                    emit highlighted(message, this->m_buffer);
            }
            if (!m_current && !data.event) {
                // TODO: create a setting for this?
                setBadge(m_badge + 1);
            }
        }
    }
}

void MessageModel::append(const MessageData& data, bool seen)
{
    int row = m_messages.count();
    beginInsertRows(QModelIndex(), row, row);
    m_messages.append(data);
    m_seen.resize(row + 1);
    m_seen[row] = seen;
    Q_ASSERT(m_messages.size() == m_seen.size());
    endInsertRows();
}

void MessageModel::clear()
{
    beginResetModel();
    m_messages.clear();
    m_seen.clear();
    endResetModel();
    setActiveHighlight(false);
    setBadge(0);
}
