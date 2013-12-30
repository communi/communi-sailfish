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

#include "messagestorage.h"
#include "messagemodel.h"
#include <IrcBuffer>

MessageStorage::MessageStorage(QObject* parent) : QObject(parent)
{
}

MessageStorage* MessageStorage::instance()
{
    static MessageStorage storage;
    return &storage;
}

QObject* MessageStorage::get(IrcBuffer* buffer) const
{
    return m_models.value(buffer);
}

void MessageStorage::add(IrcBuffer* buffer)
{
    if (buffer && !m_models.contains(buffer)) {
        buffer->setPersistent(true);
        MessageModel* model = new MessageModel(buffer);
        connect(model, SIGNAL(highlighted(IrcMessage*)), this, SLOT(onHighlighted(IrcMessage*)));
        m_models.insert(buffer, model);
    }
}

void MessageStorage::remove(IrcBuffer* buffer)
{
    if (buffer && !m_models.contains(buffer))
        delete m_models.take(buffer);
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
