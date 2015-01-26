/*
  Copyright (C) 2013-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
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

#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <QAbstractListModel>
#include <IrcMessage>
#include <QBitArray>
#include <QVector>
#include <QUrl>

class MessageFormatter;
IRC_FORWARD_DECLARE_CLASS(IrcBuffer)
IRC_FORWARD_DECLARE_CLASS(IrcMessage)

class MessageModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* buffer READ buffer CONSTANT)
    Q_PROPERTY(MessageFormatter* formatter READ formatter CONSTANT)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int separator READ separator NOTIFY separatorChanged)
    Q_PROPERTY(bool current READ isCurrent WRITE setCurrent NOTIFY currentChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(int activeHighlights READ activeHighlights WRITE setActiveHighlights NOTIFY activeHighlightsChanged)
    Q_PROPERTY(int badge READ badge WRITE setBadge NOTIFY badgeChanged)

public:
    MessageModel(IrcBuffer* buffer);
    ~MessageModel();

    IrcBuffer* buffer() const;
    MessageFormatter* formatter() const;

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
    void info(const QString& line);

signals:
    void countChanged();
    void badgeChanged();
    void currentChanged();
    void visibleChanged();
    void separatorChanged();
    void activeHighlightsChanged();
    void messageMissed(const QString& message);
    void messageHighlighted(const QString& sender, const QString& message);

private slots:
    void displaySocketError();
    void receive(IrcMessage* message);

private:
    struct MessageData {
        MessageData() : type(IrcMessage::Unknown), own(false), event(false), hilite(false) { }
        int type;
        bool own;
        bool event;
        bool hilite;
        QString richtext;
        QString plaintext;
        QString timestamp;
        QStringList rawUrls;
        QStringList urls;
        QString sender;
        QDate date;
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
