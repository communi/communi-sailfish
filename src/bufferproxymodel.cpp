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

    void processMessage(IrcMessage* message)
    {
        // deliver targeted ChanServ notices to the target channel
        // ":ChanServ!ChanServ@services. NOTICE myself :[#channel] foo bar..."
        if (message->type() == IrcMessage::Notice && message->prefix() == "ChanServ!ChanServ@services.") {
            QString content = static_cast<IrcNoticeMessage*>(message)->content();
            if (content.startsWith("[")) {
                int i = content.indexOf("]");
                if (i != -1) {
                    QString title = content.mid(1, i - 1);
                    IrcBuffer* buffer = model()->find(title);
                    if (buffer) {
                        buffer->receiveMessage(message);
                        return;
                    }
                }
            }
        }
        receiveMessage(message);
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
    return m_models;
}

QList<QObject*> BufferProxyModel::servers() const
{
    return m_servers;
}

QList<QObject*> BufferProxyModel::connections() const
{
    return m_connections;
}

QObject* BufferProxyModel::model(IrcConnection* connection) const
{
    if (connection)
        return connection->findChild<IrcBufferModel*>();
    return 0;
}

QObject* BufferProxyModel::server(IrcConnection* connection) const
{
    if (connection)
        return connection->findChild<IrcServerBuffer*>();
    return 0;
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
    connect(model, SIGNAL(messageIgnored(IrcMessage*)), buffer, SLOT(processMessage(IrcMessage*)));

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
        bool hasActiveChannels = false;
        foreach (const QString& name, model->channels()) {
            IrcBuffer* channel = model->find(name);
            if (channel && channel->isActive()) {
                hasActiveChannels = true;
                break;
            }
        }
        if (!hasActiveChannels)
            model->restoreState(model->property("savedState").toByteArray());
    });

    m_connections.append(connection);
    m_servers.append(buffer);
    m_models.append(model);

    emit connectionsChanged();
    emit serversChanged();
    emit modelsChanged();

    insertSourceModel(model);
}

void BufferProxyModel::removeConnection(IrcConnection* connection)
{
    IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
    if (model) {
        disconnect(model, SIGNAL(added(IrcBuffer*)), this, SIGNAL(bufferAdded(IrcBuffer*)));
        disconnect(model, SIGNAL(removed(IrcBuffer*)), this, SIGNAL(bufferRemoved(IrcBuffer*)));
        disconnect(model, SIGNAL(aboutToBeAdded(IrcBuffer*)), this, SIGNAL(bufferAboutToBeAdded(IrcBuffer*)));
        disconnect(model, SIGNAL(aboutToBeRemoved(IrcBuffer*)), this, SIGNAL(bufferAboutToBeRemoved(IrcBuffer*)));

        int index = m_connections.indexOf(connection);
        if (index != -1) {
            if (QObject* server = m_servers.takeAt(index))
                server->disconnect(this);
            if (QObject* model = m_models.takeAt(index))
                model->deleteLater();
            if (QObject* connection = m_connections.takeAt(index))
                connection->deleteLater();

            emit connectionsChanged();
            emit serversChanged();
            emit modelsChanged();

            removeSourceModel(model);

            if (m_models.isEmpty())
                emit reseted();
        }
    }
}

QHash<int, QByteArray> BufferProxyModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::UserRole] = "section";
    roles[Irc::BufferRole] = "buffer";
    roles[Irc::ChannelRole] = "channel";
    roles[Irc::NameRole] = "name";
    roles[Irc::PrefixRole] = "prefix";
    roles[Irc::TitleRole] = "title";
    return roles;
}

QVariant BufferProxyModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::UserRole) {
        IrcBuffer* buffer = data(index, Irc::BufferRole).value<IrcBuffer*>();
        if (buffer)
            return m_connections.indexOf(buffer->connection()); // TODO: optimize
    }
    return RowsJoinerProxy::data(index, role);
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
