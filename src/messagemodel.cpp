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

#include "messagemodel.h"
#include "messageformatter.h"
#include <QTextBoundaryFinder>
#include <IrcConnection>
#include <IrcMessage>
#include <IrcBuffer>
#include <QDateTime>

enum DataRole {
    SeenRole = Qt::UserRole,
    HighlightRole,
    TimestampRole,
    EventRole
};

MessageModel::MessageModel(IrcBuffer* buffer) : QAbstractListModel(buffer),
    m_badge(0), m_current(false), m_visible(false), m_separator(-1),
    m_highlights(0), m_buffer(buffer), m_formatter(new MessageFormatter(this))
{
    m_formatter->setStripNicks(true);
    m_formatter->setDetailed(false);
    m_formatter->setBuffer(buffer);
    m_formatter->setTimeStampFormat("");

    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receive(IrcMessage*)));
    if (buffer->isSticky() && buffer->connection())
        connect(buffer->connection(), SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(displaySocketError()));

    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(modelReset()), this, SIGNAL(countChanged()));
}

MessageModel::~MessageModel()
{
    setActiveHighlights(0);
}

IrcBuffer* MessageModel::buffer() const
{
    return m_buffer;
}

QObject* MessageModel::formatter() const
{
    return m_formatter;
}

int MessageModel::count() const
{
    return m_messages.count();
}

int MessageModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_messages.count();
}

int MessageModel::separator() const
{
    return m_separator;
}

void MessageModel::setSeparator(int separator)
{
    if (m_separator != separator) {
        m_separator = separator;
        emit separatorChanged();
    }
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
            setSeparator(m_messages.count() - 1);
        } else {
            setBadge(0);
            setActiveHighlights(0);
            if (m_separator == -1)
                setSeparator(m_messages.count() - 1);
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
        if (visible) {
            setBadge(0);
            setActiveHighlights(0);
        }
        emit visibleChanged();
    }
}

int MessageModel::activeHighlights() const
{
    return m_highlights;
}

void MessageModel::setActiveHighlights(int highlights)
{
    if (m_highlights != highlights) {
        m_highlights = highlights;
        emit activeHighlightsChanged();
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
    roles[TimestampRole] = "timestamp";
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
    case TimestampRole:
        return m_messages.at(row).timestamp;
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
        data.timestamp = message->timeStamp().toString("hh:mm");
        data.event = (message->type() != IrcMessage::Private && message->type() != IrcMessage::Notice);
        if (!data.event && !(message->flags() & IrcMessage::Own)) {
            int pos = 0;
            QString nick = message->connection()->nickName();
            QString content = message->property("content").toString();
            while (!data.hilite && (pos = content.indexOf(nick, pos, Qt::CaseInsensitive)) != -1) {
                // #60: more precise nick alerts
                QTextBoundaryFinder finder(QTextBoundaryFinder::Word, content);
                finder.setPosition(pos);
                if (finder.isAtBoundary()) {
                    finder.setPosition(pos + nick.length());
                    data.hilite = finder.isAtBoundary();
                }
                pos += nick.length();
            }
        }
        data.richtext = m_formatter->formatMessage(message, Qt::RichText);
        bool seen = (m_current && m_visible) || !message->connection()->isConnected();
        append(data, seen);
        if (!m_current || !m_visible) {
            if (data.hilite || message->property("private").toBool()) {
                setActiveHighlights(m_highlights + 1);
                emit highlighted(message);
            }
            if (!m_current && !data.event) {
                // TODO: create a setting for this?
                setBadge(m_badge + 1);
            }
        }
        if (!data.event)
            emit received(message);
    }
}

void MessageModel::displaySocketError()
{
    IrcConnection* connection = m_buffer->connection();
    QString error = connection->socket()->errorString();
    if (!error.isEmpty()) {
        IrcMessage* msg = IrcMessage::fromParameters(connection->host(), QString::number(Irc::ERR_UNKNOWNERROR), QStringList() << connection->nickName() << error, connection);
        receive(msg);
        msg->deleteLater();
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
    setActiveHighlights(0);
    setBadge(0);
}
