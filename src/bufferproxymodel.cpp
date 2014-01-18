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
#include "zncmanager.h"
#include <QCoreApplication>
#include <QTimer>
#include <IrcBufferModel>
#include <IrcConnection>
#include <IrcBuffer>

class IrcServerBuffer : public IrcBuffer
{
    Q_OBJECT

public:
    explicit IrcServerBuffer(QObject* parent = 0) : IrcBuffer(parent) { }
    ~IrcServerBuffer() { quit(tr("%1 %2").arg(qApp->applicationName(), qApp->applicationVersion())); }

public slots:
    void close(const QString& reason)
    {
        quit(reason);
        IrcBuffer::close(reason);
    }

private slots:
    void quit(const QString& reason)
    {
        IrcConnection* connection = IrcBuffer::connection();
        if (connection && connection->isActive()) {
            connection->quit(reason);
            connection->close();
        }
    }
};

BufferProxyModel::BufferProxyModel(QObject* parent) : RowsJoinerProxy(parent)
{
}

IrcBuffer* BufferProxyModel::get(int index) const
{
    foreach (QAbstractItemModel* aim, RowsJoinerProxy::models()) {
        IrcBufferModel* model = qobject_cast<IrcBufferModel*>(aim);
        if (model) {
            int count = model->count();
            if (index < count)
                return model->get(index);
            index -= count;
        }
    }
    return 0;
}

QList<QObject*> BufferProxyModel::models() const
{
    QList<QObject*> lst;
    foreach (QAbstractItemModel* aim, RowsJoinerProxy::models())
        lst += aim;
    return lst;
}

QObject* BufferProxyModel::model(IrcConnection* connection) const
{
    if (connection)
        return connection->findChild<IrcBufferModel*>();
    return 0;
}

QList<QObject*> BufferProxyModel::connections() const
{
    QList<QObject*> lst;
    foreach (QAbstractItemModel* aim, RowsJoinerProxy::models()) {
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
    connect(model, SIGNAL(aboutToBeAdded(IrcBuffer*)), this, SIGNAL(bufferAboutToBeAdded(IrcBuffer*)));
    connect(model, SIGNAL(aboutToBeRemoved(IrcBuffer*)), this, SIGNAL(bufferAboutToBeRemoved(IrcBuffer*)));

    ZncManager* znc = new ZncManager(model);
    znc->setModel(model);

    IrcServerBuffer* buffer = new IrcServerBuffer(model);
    connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(closeConnection(IrcBuffer*)));
    buffer->setName(connection->displayName());
    buffer->setSticky(true);
    model->add(buffer);

    connection->setReconnectDelay(5); // TODO: settings?
    connect(connection, SIGNAL(displayNameChanged(QString)), buffer, SLOT(setName(QString)));
    // TODO: more fine-grained delivery (WHOIS replies etc. to the current buffer)
    connect(model, SIGNAL(messageIgnored(IrcMessage*)), buffer, SLOT(receiveMessage(IrcMessage*)));

    connect(connection, SIGNAL(enabledChanged(bool)), this, SLOT(onConnectionEnabledChanged(bool)));
    connect(connection, SIGNAL(nickNameReserved(QString*)), this, SLOT(onNickNameReserved()));
    connect(connection, SIGNAL(channelKeyRequired(QString,QString*)), this, SLOT(onChannelKeyRequired(QString)));

    // Give bouncers a sec or two to start joining channels after getting connected.
    // If that happens, the saved buffers shouldn't be restored to avoid restoring
    // a buffer that was closed using another client meanwhile.
    QTimer* timer = new QTimer(connection);
    timer->setSingleShot(true);
    timer->setInterval(2000);
    connect(connection, SIGNAL(connected()), timer, SLOT(start()));
    QObject::connect(timer, &QTimer::timeout, [=]() -> void {
        if (model->count() <= 1)
            model->restoreState(model->property("savedState").toByteArray());
    });

    insertSourceModel(model);
    emit connectionsChanged();
    emit modelsChanged();
}

void BufferProxyModel::removeConnection(IrcConnection* connection)
{
    IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
    if (model) {
        disconnect(model, SIGNAL(added(IrcBuffer*)), this, SIGNAL(bufferAdded(IrcBuffer*)));
        disconnect(model, SIGNAL(removed(IrcBuffer*)), this, SIGNAL(bufferRemoved(IrcBuffer*)));
        disconnect(model, SIGNAL(aboutToBeAdded(IrcBuffer*)), this, SIGNAL(bufferAboutToBeAdded(IrcBuffer*)));
        disconnect(model, SIGNAL(aboutToBeRemoved(IrcBuffer*)), this, SIGNAL(bufferAboutToBeRemoved(IrcBuffer*)));

        removeSourceModel(model);
        emit connectionsChanged();
        emit modelsChanged();
        if (RowsJoinerProxy::models().isEmpty())
            emit reseted();
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
    QVariantList modelStates;
    QVariantList connectionStates;
    foreach (QAbstractItemModel* aim, RowsJoinerProxy::models()) {
        IrcBufferModel* model = qobject_cast<IrcBufferModel*>(aim);
        if (model) {
            connectionStates += model->connection()->saveState();
            // do not save the server buffer - let addConnection() handle it when restoring
            model->remove(model->get(0));
            if (model->connection()->isEnabled())
                modelStates += model->saveState();
            else
                modelStates += model->property("savedState");
        }
    }

    QVariantMap state;
    state.insert("models", modelStates);
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

    QVariantList modelStates = state.value("models").toList();
    QVariantList connectionStates = state.value("connections").toList();

    for (int i = 0; i < connectionStates.length(); ++i) {
        IrcConnection* connection = new IrcConnection(this);
        connection->restoreState(connectionStates.at(i).toByteArray());
        addConnection(connection);
        IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
        model->setProperty("savedState", modelStates.value(i));
    }
    return true;
}

void BufferProxyModel::onConnectionEnabledChanged(bool enabled)
{
    // store the model state when a connection is disabled
    // see #25: Don't save/restore buffers for disabled connections
    if (!enabled) {
        IrcConnection* connection = qobject_cast<IrcConnection*>(sender());
        if (connection) {
            IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
            if (model && model->count() > 1)
                model->setProperty("savedState", model->saveState());
        }
    }
}

void BufferProxyModel::closeConnection(IrcBuffer* buffer)
{
    removeConnection(buffer->connection());
}

void BufferProxyModel::onChannelKeyRequired(const QString& channel)
{
    IrcConnection* connection = qobject_cast<IrcConnection*>(sender());
    if (connection)
        emit channelKeyRequired(connection, channel);
}

void BufferProxyModel::onNickNameReserved()
{
    IrcConnection* connection = qobject_cast<IrcConnection*>(sender());
    if (connection)
        emit nickNameReserved(connection);
}

#include "bufferproxymodel.moc"
