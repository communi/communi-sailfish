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

#ifndef MESSAGESTORAGE_H
#define MESSAGESTORAGE_H

#include <QHash>
#include <QColor>
#include <QObject>
#include <QPointer>
#include <IrcGlobal>

class MessageModel;
class BufferProxyModel;

IRC_FORWARD_DECLARE_CLASS(IrcBuffer)
IRC_FORWARD_DECLARE_CLASS(IrcMessage)

class MessageStorage : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.communi.irc")
    Q_PROPERTY(int activeHighlights READ activeHighlights NOTIFY activeHighlightsChanged SCRIPTABLE true)
    Q_PROPERTY(int firstActiveHighlight READ firstActiveHighlight NOTIFY firstActiveHighlightChanged)
    Q_PROPERTY(int lastActiveHighlight READ lastActiveHighlight NOTIFY lastActiveHighlightChanged)
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor)

public:
    MessageStorage(BufferProxyModel* proxy);

    MessageModel* model(IrcBuffer* buffer) const;
    Q_INVOKABLE QObject* get(IrcBuffer* buffer) const;

    int activeHighlights() const;
    void setActiveHighlights(int highlights);

    int firstActiveHighlight() const;
    void setFirstActiveHighlight(int highlight);

    int lastActiveHighlight() const;
    void setLastActiveHighlight(int highlight);

    QColor baseColor() const;
    void setBaseColor(const QColor& color);

public slots:
    void add(IrcBuffer* buffer);
    void remove(IrcBuffer* buffer);

signals:
    void added(MessageModel* model);
    void removed(MessageModel* model);

    void missed(IrcBuffer* buffer, const QString& message);
    void highlighted(IrcBuffer* buffer, const QString& sender, const QString& message);

    Q_SCRIPTABLE void messageMissed(const QString& sender, const QString& message);
    Q_SCRIPTABLE void messageHighlighted(const QString& buffer, const QString& sender, const QString& message);

    Q_SCRIPTABLE void activeHighlightsChanged(int highlights);
    void firstActiveHighlightChanged();
    void lastActiveHighlightChanged();

protected:
    void timerEvent(QTimerEvent* event);

private slots:
    void updateActiveHighlights();
    void onMessageMissed(const QString& message);
    void onMessageHighlighted(const QString& sender, const QString& message);
    void onCurrentBufferChanged(IrcBuffer* buffer);

private:
    int m_dirty;
    int m_highlights;
    int m_firstHiglight;
    int m_lastHighlight;
    QColor m_baseColor;
    BufferProxyModel* m_proxy;
    QPointer<MessageModel> m_current;
    QHash<IrcBuffer*, MessageModel*> m_models;
};

#endif // MESSAGESTORAGE_H
