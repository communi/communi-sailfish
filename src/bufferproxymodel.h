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

#ifndef BUFFERPROXYMODEL_H
#define BUFFERPROXYMODEL_H

#include "RowsJoinerProxy.h"

class IrcBuffer;
class IrcConnection;
class IrcBufferModel;
class IrcServerBuffer;

class BufferProxyModel : public RowsJoinerProxy
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> models READ models NOTIFY modelsChanged)
    Q_PROPERTY(QList<QObject*> servers READ servers NOTIFY serversChanged)
    Q_PROPERTY(QList<QObject*> connections READ connections NOTIFY connectionsChanged)

public:
    BufferProxyModel(QObject* parent = 0);

    Q_INVOKABLE IrcBuffer* get(int index) const;

    QList<QObject*> models() const;
    QList<QObject*> servers() const;
    QList<QObject*> connections() const;

    Q_INVOKABLE QObject* model(IrcConnection* connection) const;
    Q_INVOKABLE QObject* server(IrcConnection* connection) const;

    Q_INVOKABLE void addConnection(IrcConnection* connection);
    Q_INVOKABLE void removeConnection(IrcConnection* connection);

    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE QByteArray saveState() const;
    Q_INVOKABLE bool restoreState(const QByteArray& state);

signals:
    void reseted();
    void modelsChanged();
    void serversChanged();
    void connectionsChanged();
    void bufferAdded(IrcBuffer* buffer);
    void bufferRemoved(IrcBuffer* buffer);
    void bufferAboutToBeAdded(IrcBuffer* buffer);
    void bufferAboutToBeRemoved(IrcBuffer* buffer);

    void nickNameReserved(IrcConnection* connection);
    void channelKeyRequired(IrcConnection* connection, const QString& channel);

private slots:
    void onConnectionEnabledChanged(bool enabled);
    void closeConnection(IrcBuffer* buffer);
    void onChannelKeyRequired(const QString& channel);
    void onNickNameReserved();

private:
    QList<QObject*> m_models;
    QList<QObject*> m_servers;
    QList<QObject*> m_connections;
};

#endif // BUFFERPROXYMODEL_H
