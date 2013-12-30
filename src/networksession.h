/*
* Copyright (C) 2008-2014 J-P Nurmi <jpnurmi@gmail.com>
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

#ifndef NETWORKSESSION_H
#define NETWORKSESSION_H

#include <QNetworkSession>

class NetworkSession : public QObject
{
    Q_OBJECT

public:
    NetworkSession(QObject* parent = 0);

public slots:
    bool open();

private:
    QNetworkSession* m_session;
};

#endif // NETWORKSESSION_H
