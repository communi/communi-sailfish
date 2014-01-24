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

#ifndef MESSAGESTORAGE_H
#define MESSAGESTORAGE_H

#include <QHash>
#include <QObject>
#include <QPointer>

class IrcBuffer;
class IrcMessage;
class MessageModel;

class MessageStorage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* currentBuffer READ currentBuffer WRITE setCurrentBuffer NOTIFY currentBufferChanged)
    Q_PROPERTY(int activeHighlights READ activeHighlights NOTIFY activeHighlightsChanged)

public:
    static MessageStorage* instance();

    Q_INVOKABLE QObject* get(IrcBuffer* buffer) const;

    IrcBuffer* currentBuffer() const;
    void setCurrentBuffer(IrcBuffer* buffer);

    int activeHighlights() const;
    void setActiveHighlights(int highlights);

public slots:
    void add(IrcBuffer* buffer);
    void remove(IrcBuffer* buffer);

signals:
    void activeHighlightsChanged();
    void currentBufferChanged(IrcBuffer* buffer);
    void highlighted(IrcBuffer* buffer, IrcMessage* message);
    void received(IrcBuffer* buffer, IrcMessage* message);

private slots:
    void updateActiveHighlights();
    void onReceived(IrcMessage* message);
    void onHighlighted(IrcMessage* message);

private:
    MessageStorage(QObject* parent = 0);

    int m_highlights;
    QPointer<IrcBuffer> m_current;
    QHash<IrcBuffer*, MessageModel*> m_models;
};

#endif // MESSAGESTORAGE_H
