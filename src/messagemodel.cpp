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

MessageModel::MessageModel(IrcBuffer* buffer) : QStringListModel(buffer),
    m_badge(0), m_active(false), m_highlight(false),
    m_buffer(buffer), m_formatter(new MessageFormatter(this))
{
    m_formatter->setBuffer(buffer);
    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receive(IrcMessage*)));
}

IrcBuffer* MessageModel::buffer() const
{
    return m_buffer;
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
            m_seen.fill(true);
            setActiveHighlight(false);
        } else {
            setBadge(0);
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
    roles[Qt::DisplayRole] = "display";
    roles[HighlightRole] = "highlight";
    roles[SeenRole] = "seen";
    return roles;
}

QVariant MessageModel::data(const QModelIndex& index, int role) const
{
    if (role == HighlightRole) {
        return m_highlights.value(index.row(), false);
    }
    if (role == SeenRole)
        return m_seen.value(index.row(), false);
    return QStringListModel::data(index, role);
}

bool MessageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == HighlightRole)
        m_highlights[index.row()] = value.toBool();
    if (role == SeenRole)
        m_seen[index.row()] = value.toBool();
    return QStringListModel::setData(index, value, role);
}

void MessageModel::receive(IrcMessage* message)
{
    QString formatted = m_formatter->formatMessage(message);
    if (!formatted.isEmpty()) {
        bool hilite = false;
        if (!(message->flags() & IrcMessage::Own))
            hilite = message->property("content").toString().contains(message->connection()->nickName(), Qt::CaseInsensitive);
        append(formatted, hilite);
        if (!m_active) {
            if (hilite || message->property("private").toBool()) {
                setActiveHighlight(true);
                emit highlighted(message);
            }
            setBadge(m_badge + 1);
        }
    }
}

void MessageModel::append(const QString& message, bool hilite)
{
    int row = rowCount();
    insertRow(row);
    m_seen.resize(row + 1);
    m_highlights.resize(row + 1);
    m_highlights[row] = hilite;
    setData(index(row), message);
}
