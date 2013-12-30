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

#include "bufferproxymodel.h"
#include <IrcBufferModel>
#include <IrcConnection>
#include <IrcBuffer>

class IrcServerBuffer : public IrcBuffer
{
    Q_OBJECT

public:
    explicit IrcServerBuffer(QObject* parent = 0) : IrcBuffer(parent) { }

public slots:
    void close(const QString& reason)
    {
        IrcConnection* c = connection();
        if (c && c->isActive()) {
            c->quit(reason);
            c->close();
        }
        IrcBuffer::close(reason);
    }
};

BufferProxyModel::BufferProxyModel(QObject* parent) : RowsJoinerProxy(parent)
{
}

QList<QObject*> BufferProxyModel::connections() const
{
    QList<QObject*> lst;
    foreach (QAbstractItemModel* aim, models()) {
        IrcBufferModel* model = qobject_cast<IrcBufferModel*>(aim);
        if (model)
            lst += model->connection();
    }
    return lst;
}

void BufferProxyModel::addConnection(IrcConnection* connection)
{
    IrcBufferModel* model = new IrcBufferModel(connection);
    model->setSortMethod(Irc::SortByTitle);
    connect(model, SIGNAL(added(IrcBuffer*)), this, SIGNAL(bufferAdded(IrcBuffer*)));
    connect(model, SIGNAL(removed(IrcBuffer*)), this, SIGNAL(bufferRemoved(IrcBuffer*)));

    IrcBuffer* buffer = model->add(connection->displayName());
    buffer->setSticky(true);

    connect(connection, SIGNAL(displayNameChanged(QString)), buffer, SLOT(setName(QString)));
    // TODO: more fine-grained delivery (WHOIS replies etc. to the current buffer)
    connect(model, SIGNAL(messageIgnored(IrcMessage*)), buffer, SLOT(receiveMessage(IrcMessage*)));

    insertSourceModel(model);
    emit connectionsChanged();
}

void BufferProxyModel::removeConnection(IrcConnection* connection)
{
    IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
    if (model) {
        disconnect(model, SIGNAL(added(IrcBuffer*)), this, SIGNAL(bufferAdded(IrcBuffer*)));
        disconnect(model, SIGNAL(removed(IrcBuffer*)), this, SIGNAL(bufferRemoved(IrcBuffer*)));

        removeSourceModel(model);
        emit connectionsChanged();
    }
}

QHash<int, QByteArray> BufferProxyModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Irc::BufferRole] = "buffer";
    roles[Irc::ChannelRole] = "channel";
    roles[Irc::NameRole] = "name";
    roles[Irc::PrefixRole] = "prefix";
    roles[Irc::TitleRole] = "title";
    return roles;
}

QByteArray BufferProxyModel::saveState() const
{
    //TODO: QVariantList modelStates;
    QVariantList connectionStates;
    foreach (QAbstractItemModel* aim, models()) {
        IrcBufferModel* model = qobject_cast<IrcBufferModel*>(aim);
        if (model) {
            //TODO: modelStates += model->saveState();
            connectionStates += model->connection()->saveState();
        }
    }

    QVariantMap state;
    //TODO: state.insert("models", modelStates);
    state.insert("connections", connectionStates);

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << state;
    return data;
}

bool BufferProxyModel::restoreState(const QByteArray& data)
{
    QVariantMap state;
    QDataStream in(data);
    in >> state;
    if (in.status() != QDataStream::Ok)
        return false;

    //TODO: QVariantList modelStates = state.value("models").toList();
    QVariantList connectionStates = state.value("connections").toList();

    for (int i = 0; i < connectionStates.length(); ++i) {
        IrcConnection* connection = new IrcConnection(this);
        connection->restoreState(connectionStates.at(i).toByteArray());
        addConnection(connection);
        if (connection->isEnabled())
            connection->open();
    }
    return true;
}

#include "bufferproxymodel.moc"
