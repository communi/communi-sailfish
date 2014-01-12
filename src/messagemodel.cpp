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
    HighlightRole
};

MessageModel::MessageModel(IrcBuffer* buffer) : QAbstractListModel(buffer),
    m_badge(0), m_active(false), m_highlight(false),
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

bool MessageModel::isActive() const
{
    return m_active;
}

void MessageModel::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        if (!active) {
            for (int i = m_messages.count() - 1; i >= 0; --i) {
                bool& seen = m_messages[i].seen;
                if (seen)
                    break;
                seen = true;
            }
        } else {
            setBadge(0);
            setActiveHighlight(false);
        }
        emit activeChanged();
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
    case SeenRole:
        return m_messages.at(row).seen;
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
    QString plaintext = m_formatter->formatMessage(message, Qt::PlainText);
    if (!plaintext.isEmpty()) {
        bool hilite = false;
        if (!(message->flags() & IrcMessage::Own))
            hilite = message->property("content").toString().contains(message->connection()->nickName(), Qt::CaseInsensitive);
        QString richtext = m_formatter->formatMessage(message, Qt::RichText);
        append(richtext, plaintext, hilite);
        if (!m_active) {
            if (hilite || message->property("private").toBool()) {
                setActiveHighlight(true);
                emit highlighted(message);
            }
            setBadge(m_badge + 1);
        }
    }
}

void MessageModel::append(const QString& richtext, const QString& plaintext, bool hilite)
{
    int row = m_messages.count();
    beginInsertRows(QModelIndex(), row, row);
    MessageData data;
    data.seen = false;
    data.hilite = hilite;
    data.richtext = richtext;
    data.plaintext = plaintext;
    m_messages.append(data);
    endInsertRows();
}

void MessageModel::clear()
{
    beginResetModel();
    m_messages.clear();
    endResetModel();
    setActiveHighlight(false);
    setBadge(0);
}
