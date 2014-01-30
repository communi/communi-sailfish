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

#include "bufferproxymodel.h"
#include "simplecrypt.h"
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
    SimpleCrypt crypto(Q_UINT64_C(0xff610ed9de767b09));

    foreach (QAbstractItemModel* aim, RowsJoinerProxy::models()) {
        IrcBufferModel* model = qobject_cast<IrcBufferModel*>(aim);
        if (model) {
            connectionStates += crypto.encryptToByteArray(model->connection()->saveState());
            // do not save the server buffer - let addConnection() handle it when restoring
            model->remove(model->get(0));
            if (model->connection()->isEnabled())
                modelStates += crypto.encryptToByteArray(model->saveState());
            else
                modelStates += crypto.encryptToByteArray(model->property("savedState").toByteArray());
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
    SimpleCrypt crypto(Q_UINT64_C(0xff610ed9de767b09));

    for (int i = 0; i < connectionStates.length(); ++i) {
        IrcConnection* connection = new IrcConnection(this);
        QByteArray cs = crypto.decryptToByteArray(connectionStates.at(i).toByteArray());
        connection->restoreState(!crypto.lastError() ? cs : connectionStates.at(i).toByteArray());
        addConnection(connection);
        IrcBufferModel* model = connection->findChild<IrcBufferModel*>();
        QByteArray ms = crypto.decryptToByteArray(modelStates.value(i).toByteArray());
        model->setProperty("savedState", !crypto.lastError() ? ms : modelStates.value(i));
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
