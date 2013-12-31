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

#include <QStringListModel>
#include <QVector>

class IrcBuffer;
class IrcMessage;
class MessageFormatter;

class MessageModel : public QStringListModel
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* buffer READ buffer CONSTANT)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool activeHighlight READ activeHighlight WRITE setActiveHighlight NOTIFY activeHighlightChanged)
    Q_PROPERTY(int badge READ badge WRITE setBadge NOTIFY badgeChanged)

public:
    MessageModel(IrcBuffer* buffer);

    IrcBuffer* buffer() const;

    bool isActive() const;
    void setActive(bool active);

    bool activeHighlight() const;
    void setActiveHighlight(bool highlight);

    int badge() const;
    void setBadge(int badge);

    QHash<int, QByteArray> roleNames() const;

    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

public slots:
    void receive(IrcMessage* message);
    void append(const QString& message, bool hilite);
    void clear();

signals:
    void badgeChanged();
    void activeChanged();
    void activeHighlightChanged();
    void highlighted(IrcMessage* message = 0);

private:
    int m_badge;
    bool m_active;
    bool m_highlight;
    IrcBuffer* m_buffer;
    QVector<bool> m_seen;
    QVector<bool> m_highlights;
    MessageFormatter* m_formatter;
};

#endif // MESSAGEMODEL_H
