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

#include "networksession.h"
#include <QNetworkConfigurationManager>

NetworkSession::NetworkSession(QObject* parent) : QObject(parent),
    m_enabled(false), m_session(0), m_manager(new QNetworkConfigurationManager(this))
{
    m_config = m_manager->defaultConfiguration();
    connect(m_manager, SIGNAL(onlineStateChanged(bool)), this, SLOT(onOnlineStateChanged()));
    connect(m_manager, SIGNAL(configurationChanged(QNetworkConfiguration)), this, SLOT(onNetworkConfigurationChanged(QNetworkConfiguration)));
}

bool NetworkSession::isEnabled() const
{
    return m_enabled;
}

void NetworkSession::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged();
    }
}

bool NetworkSession::isOnline() const
{
    return m_manager->isOnline();
}

bool NetworkSession::open()
{
    if (m_manager->capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        if (!m_session || m_session->configuration() != m_config) {
            delete m_session;
            m_session = new QNetworkSession(m_config, this);
        }
        m_session->open();
    }
    // TODO: return value?
    return true;
}

void NetworkSession::onOnlineStateChanged()
{
    if (m_enabled)
        emit onlineStateChanged();
}

void NetworkSession::onNetworkConfigurationChanged(const QNetworkConfiguration& config)
{
    if (m_enabled && config.state() == QNetworkConfiguration::Active && m_config.state() != QNetworkConfiguration::Active) {
        m_config = config;
        emit connectionChanged();
    }
}
