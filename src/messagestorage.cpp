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

#include "messagestorage.h"
#include "bufferproxymodel.h"
#include "messageformatter.h"
#include "messagemodel.h"
#include <IrcBuffer>
#include <QTimerEvent>
#include <QtCore/QDebug>
#include <QtDBus/QDBusConnection>

MessageStorage::MessageStorage(BufferProxyModel* proxy) : QObject(proxy), m_dirty(0), m_highlights(0),
    m_firstHiglight(-1), m_lastHighlight(-1), m_baseColor(QColor::fromHsl(359, 102, 116)), m_proxy(proxy)
{
    // Register the app as a D-Bus service
    if (!QDBusConnection::sessionBus().registerService("com.communi.irc")) {
        qDebug() << Q_FUNC_INFO << "Couldn't register D-Bus service!";
    }
    // Register this MessageStorage instance
    if (!QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportScriptableContents)) {
        qDebug() << Q_FUNC_INFO << "Couldn't register D-Bus object!";
    }
}

MessageModel* MessageStorage::model(IrcBuffer* buffer) const
{
    return m_models.value(buffer);
}

QObject* MessageStorage::get(IrcBuffer* buffer) const
{
    return m_models.value(buffer);
}

IrcBuffer* MessageStorage::currentBuffer() const
{
    return m_current;
}

void MessageStorage::setCurrentBuffer(IrcBuffer* buffer)
{
    if (m_current != buffer) {
        if (m_current) {
            MessageModel* model = m_models.value(m_current);
            if (model)
                model->setCurrent(false);
        }
        if (buffer) {
            MessageModel* model = m_models.value(buffer);
            if (model)
                model->setCurrent(true);
        }
        m_current = buffer;
        emit currentBufferChanged(buffer);
    }
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
        buffer->setPersistent(true);
        MessageModel* model = new MessageModel(buffer);
        model->formatter()->setBaseColor(m_baseColor);
        connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(remove(IrcBuffer*)));
        connect(model, SIGNAL(activeHighlightsChanged()), this, SLOT(updateActiveHighlights()));
        connect(model, SIGNAL(highlighted(IrcMessage*)), this, SLOT(onHighlighted(IrcMessage*)));
        m_models.insert(buffer, model);
        emit added(model);

        if (!m_dirty)
            m_dirty = startTimer(100);
    }
}

void MessageStorage::remove(IrcBuffer* buffer)
{
    if (buffer && m_models.contains(buffer)) {
        MessageModel* model = m_models.take(buffer);
        if (model) {
            emit removed(model);
            delete model;

            if (!m_dirty)
                m_dirty = startTimer(100);
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

void MessageStorage::onHighlighted(IrcMessage* message)
{
    MessageModel* model = qobject_cast<MessageModel*>(sender());
    if (model) {
        IrcBuffer* buffer = model->buffer();
        if (buffer)
            emit highlighted(buffer, message);
    }
}
