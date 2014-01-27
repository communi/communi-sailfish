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

#ifndef ACTIVITYMODEL_H
#define ACTIVITYMODEL_H

#include <QStringList>
#include <QAbstractListModel>

class IrcBuffer;
class IrcMessage;

class ActivityModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ActivityModel(QObject* parent = 0);

    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;

public slots:
    void clear();

private slots:
    void receive(IrcBuffer* buffer, IrcMessage* message);

private:
    QStringList m_buffers;
};

#endif // ACTIVITYMODEL_H
