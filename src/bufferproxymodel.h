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

#ifndef BUFFERPROXYMODEL_H
#define BUFFERPROXYMODEL_H

#include "RowsJoinerProxy.h"

class IrcBuffer;
class IrcConnection;

class BufferProxyModel : public RowsJoinerProxy
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount)
    Q_PROPERTY(QList<QObject*> models READ models NOTIFY modelsChanged)
    Q_PROPERTY(QList<QObject*> connections READ connections NOTIFY connectionsChanged)

public:
    BufferProxyModel(QObject* parent = 0);

    QList<QObject*> models() const;
    QList<QObject*> connections() const;
    Q_INVOKABLE void addConnection(IrcConnection* connection);
    Q_INVOKABLE void removeConnection(IrcConnection* connection);

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE QByteArray saveState() const;
    Q_INVOKABLE bool restoreState(const QByteArray& state);

signals:
    void modelsChanged();
    void connectionsChanged();
    void bufferAdded(IrcBuffer* buffer);
    void bufferRemoved(IrcBuffer* buffer);
};

#endif // BUFFERPROXYMODEL_H
