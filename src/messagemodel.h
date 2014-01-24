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

#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <QAbstractListModel>
#include <QBitArray>
#include <QVector>

class IrcBuffer;
class IrcMessage;
class MessageFormatter;

class MessageModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* buffer READ buffer CONSTANT)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int separator READ separator NOTIFY separatorChanged)
    Q_PROPERTY(bool current READ isCurrent WRITE setCurrent NOTIFY currentChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(int activeHighlights READ activeHighlights WRITE setActiveHighlights NOTIFY activeHighlightsChanged)
    Q_PROPERTY(int badge READ badge WRITE setBadge NOTIFY badgeChanged)

public:
    MessageModel(IrcBuffer* buffer);

    IrcBuffer* buffer() const;

    int count() const;
    int rowCount(const QModelIndex& parent) const;

    int separator() const;
    void setSeparator(int separator);

    bool isCurrent() const;
    void setCurrent(bool current);

    bool isVisible() const;
    void setVisible(bool visible);

    int activeHighlights() const;
    void setActiveHighlights(int highlights);

    int badge() const;
    void setBadge(int badge);

    QHash<int, QByteArray> roleNames() const;

    QVariant data(const QModelIndex& index, int role) const;

public slots:
    void clear();

signals:
    void countChanged();
    void badgeChanged();
    void currentChanged();
    void visibleChanged();
    void separatorChanged();
    void activeHighlightsChanged();
    void received(IrcMessage* message);
    void highlighted(IrcMessage* message);

private slots:
    void receive(IrcMessage* message);

private:
    struct MessageData {
        MessageData() : event(false), hilite(false) { }
        bool event;
        bool hilite;
        QString richtext;
        QString plaintext;
        QString timestamp;
    };

    void append(const MessageData& data, bool seen);

    int m_badge;
    bool m_current;
    bool m_visible;
    int m_separator;
    int m_highlights;
    IrcBuffer* m_buffer;
    MessageFormatter* m_formatter;
    QVector<MessageData> m_messages;
    QBitArray m_seen;
};

#endif // MESSAGEMODEL_H
