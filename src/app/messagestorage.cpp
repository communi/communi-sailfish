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

#include "messagestorage.h"
#include "bufferproxymodel.h"
#include "messageformatter.h"
#include "messagemodel.h"
#include <IrcBufferModel>
#include <IrcBuffer>
#include <QTimerEvent>
#include <QDebug>
#include <QDBusConnection>
#include <QCoreApplication>

IRC_USE_NAMESPACE

class MessageService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "chat.communi")

public:
    MessageService(QObject* parent = nullptr) : QObject(parent)
    {
        if (!QDBusConnection::sessionBus().registerService(
                QCoreApplication::organizationName() + "."
                + QCoreApplication::applicationName()))
            qWarning() << "MessageService: failed to register "
                       << (QCoreApplication::organizationName() + "."
                           + QCoreApplication::applicationName())
                       << " D-Bus object";
        if (!QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportAllSignals))
            qWarning() << "MessageService: failed to register "
                       << (QCoreApplication::organizationName() + "."
                           + QCoreApplication::applicationName())
                       << " D-Bus object";
    }

signals:
    void activeHighlightsChanged(int highlights);
    void messageMissed(const QString& sender, const QString& message, QDateTime timestamp);
    void messageHighlighted(const QString& buffer, const QString& sender,
                            const QString& message, QDateTime timestamp);

private:
    friend class MessageStorage;
};

MessageStorage::MessageStorage(BufferProxyModel* proxy) : QObject(proxy), m_dirty(0), m_highlights(0),
    m_firstHiglight(-1), m_lastHighlight(-1), m_baseColor(QColor::fromHsl(359, 102, 116)),
    m_service(new MessageService(this)), m_proxy(proxy)
{
    connect(proxy, &BufferProxyModel::currentBufferChanged, this, &MessageStorage::onCurrentBufferChanged);
}

MessageModel* MessageStorage::model(IrcBuffer* buffer) const
{
    return m_models.value(buffer);
}

QObject* MessageStorage::get(IrcBuffer* buffer) const
{
    return m_models.value(buffer);
}

int MessageStorage::activeHighlights() const
{
    return m_highlights;
}

void MessageStorage::setActiveHighlights(int highlights)
{
    if (m_highlights != highlights) {
        m_highlights = highlights;
        emit activeHighlightsChanged(highlights);
        emit m_service->activeHighlightsChanged(highlights);
    }
}

int MessageStorage::firstActiveHighlight() const
{
    return m_firstHiglight;
}

void MessageStorage::setFirstActiveHighlight(int highlight)
{
    if (m_firstHiglight != highlight) {
        m_firstHiglight = highlight;
        emit firstActiveHighlightChanged();
    }
}

int MessageStorage::lastActiveHighlight() const
{
    return m_lastHighlight;
}

void MessageStorage::setLastActiveHighlight(int highlight)
{
    if (m_lastHighlight != highlight) {
        m_lastHighlight = highlight;
        emit lastActiveHighlightChanged();
    }
}

QColor MessageStorage::baseColor() const
{
    return m_baseColor;
}

void MessageStorage::setBaseColor(const QColor& color)
{
    if (m_baseColor != color) {
        m_baseColor = color;
        foreach (MessageModel* model, m_models)
            model->formatter()->setBaseColor(color);
    }
}

void MessageStorage::add(IrcBuffer* buffer)
{
    if (buffer && !m_models.contains(buffer)) {
        if (buffer->isSticky())
            connect(buffer->model(), &IrcBufferModel::buffersChanged, this, &MessageStorage::invalidateActiveHighlights);
        buffer->setPersistent(true);
        MessageModel* model = new MessageModel(buffer);
        model->formatter()->setBaseColor(m_baseColor);
        connect(buffer, &IrcBuffer::destroyed, this, &MessageStorage::remove);
        connect(model, &MessageModel::activeHighlightsChanged, this, &MessageStorage::updateActiveHighlights);
        connect(model, &MessageModel::messageMissed, this, &MessageStorage::onMessageMissed);
        connect(model, &MessageModel::messageHighlighted, this, &MessageStorage::onMessageHighlighted);
        m_models.insert(buffer, model);
        emit added(model);
        invalidateActiveHighlights();
    }
}

void MessageStorage::remove(IrcBuffer* buffer)
{
    if (buffer && m_models.contains(buffer)) {
        MessageModel* model = m_models.take(buffer);
        if (model) {
            emit removed(model);
            delete model;
            invalidateActiveHighlights();
        }
    }
}

void MessageStorage::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == m_dirty) {
        updateActiveHighlights();
        killTimer(m_dirty);
        m_dirty = 0;
    }
}

void MessageStorage::updateActiveHighlights()
{
    int first = -1;
    int last = -1;
    int highlights = 0;
    foreach (MessageModel* model, m_models) {
        const int active = model->activeHighlights();
        highlights += active;
        if (active > 0) {
            const int index = m_proxy->indexOf(model->buffer());
            if (index != -1) {
                first = first == -1 ? index : qMin(first, index);
                last = qMax(last, index);
            }
        }
    }
    setActiveHighlights(highlights);
    setFirstActiveHighlight(first);
    setLastActiveHighlight(last);
}

void MessageStorage::invalidateActiveHighlights()
{
    if (!m_dirty)
        m_dirty = startTimer(100);
}

void MessageStorage::onMessageMissed(const QString& message, QDateTime timestamp)
{
    MessageModel* model = qobject_cast<MessageModel*>(sender());
    if (model) {
        IrcBuffer* buffer = model->buffer();
        if (buffer) {
            emit missed(buffer, message, timestamp);
            emit m_service->messageMissed(buffer->title(), message, timestamp);
        }
    }
}

void MessageStorage::onMessageHighlighted(const QString& sender, const QString& message,
                                          QDateTime timestamp)
{
    MessageModel* model = qobject_cast<MessageModel*>(QObject::sender());
    if (model) {
        IrcBuffer* buffer = model->buffer();
        if (buffer) {
            emit highlighted(buffer, sender, message, timestamp);
            emit m_service->messageHighlighted(buffer->title(), sender, message, timestamp);
        }
    }
}

void MessageStorage::onCurrentBufferChanged(IrcBuffer* buffer)
{
    MessageModel* model = m_models.value(buffer);
    if (m_current != model) {
        if (m_current)
            m_current->setCurrent(false);
        if (model)
            model->setCurrent(true);
        m_current = model;
    }
}

#include "messagestorage.moc"
