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

#ifndef BUFFERPROXYMODEL_H
#define BUFFERPROXYMODEL_H

#include "RowsJoinerProxy.h"
#include <QPointer>
#include <IrcGlobal>

IRC_FORWARD_DECLARE_CLASS(IrcBuffer)
IRC_FORWARD_DECLARE_CLASS(IrcMessage)
IRC_FORWARD_DECLARE_CLASS(IrcConnection)

class BufferProxyModel : public RowsJoinerProxy
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> models READ models NOTIFY modelsChanged)
    Q_PROPERTY(QList<QObject*> servers READ servers NOTIFY serversChanged)
    Q_PROPERTY(QList<QObject*> connections READ connections NOTIFY connectionsChanged)
    Q_PROPERTY(IrcBuffer* currentBuffer READ currentBuffer WRITE setCurrentBuffer NOTIFY currentBufferChanged)
    Q_PROPERTY(int sortMethod READ sortMethod WRITE setSortMethod)

public:
    BufferProxyModel(QObject* parent = 0);

    Q_INVOKABLE IrcBuffer* get(int index) const;
    Q_INVOKABLE int indexOf(IrcBuffer* buffer) const;

    QList<QObject*> models() const;
    QList<QObject*> servers() const;
    QList<QObject*> connections() const;

    IrcBuffer* currentBuffer() const;
    void setCurrentBuffer(IrcBuffer* buffer);

    Q_INVOKABLE QObject* model(IrcConnection* connection) const;
    Q_INVOKABLE QObject* server(IrcConnection* connection) const;

    Q_INVOKABLE void addConnection(IrcConnection* connection);
    Q_INVOKABLE void insertConnection(int index, IrcConnection* connection);
    Q_INVOKABLE void removeConnection(IrcConnection* connection);

    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE QByteArray saveState() const;
    Q_INVOKABLE bool restoreState(const QByteArray& state);

    int sortMethod() const;
    void setSortMethod(int method);

signals:
    void reseted();
    void modelsChanged();
    void serversChanged();
    void connectionsChanged();
    void bufferAdded(IrcBuffer* buffer);
    void bufferRemoved(IrcBuffer* buffer);
    void bufferAboutToBeAdded(IrcBuffer* buffer);
    void bufferAboutToBeRemoved(IrcBuffer* buffer);
    void currentBufferChanged(IrcBuffer* buffer);
    void connectionAdded(IrcConnection* connection);
    void connectionRemoved(IrcConnection* connection);

    void connected(IrcConnection* connection);
    void disconnected(IrcConnection* connection);
    void nickNameRequired(IrcConnection* connection, const QString& reserved);
    void channelKeyRequired(IrcConnection* connection, const QString& channel);

private slots:
    void onConnected();
    void onDisconnected();
    void closeConnection(IrcBuffer* buffer);
    void onChannelKeyRequired(const QString& channel);
    void onNickNameRequired(const QString& reserved);
    void processMessage(IrcMessage* message);

private:
    int m_method;
    QList<QObject*> m_models;
    QList<QObject*> m_servers;
    QList<QObject*> m_connections;
    QPointer<IrcBuffer> m_current;
};

#endif // BUFFERPROXYMODEL_H
