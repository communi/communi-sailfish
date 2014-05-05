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
