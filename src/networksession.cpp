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

#include "networksession.h"
#include <QNetworkConfigurationManager>

NetworkSession::NetworkSession(QObject* parent) : QObject(parent), m_session(0)
{
}

bool NetworkSession::open()
{
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        if (!m_session)
            m_session = new QNetworkSession(manager.defaultConfiguration(), this);
        m_session->open();
    }
    // TODO: return value?
    return true;
}
