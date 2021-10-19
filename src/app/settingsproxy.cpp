// SPDX-License-Identifier: GPL-2.0
/*
  Copyright (C) 2008-2021 The Communi Project
*/

#include "settingsproxy.h"

#include <QStringList>
#include <QCoreApplication>


SettingsProxy::SettingsProxy(BufferProxyModel *bufferModel,
                   IgnoreManager* ignore, QObject* parent)
{
    m_settings = new QSettings(QCoreApplication::applicationName(),
                               QCoreApplication::applicationName(), parent);

    m_ignoreManager = ignore;
    m_bufferModel = bufferModel;
    this->restoreSettings();
}

void SettingsProxy::restoreSettings()
{
    m_bufferModel->restoreState(m_settings->value("state", QByteArray()).toByteArray());
    m_ignoreManager->setIgnores(m_settings->value("ignores").toStringList());
}

void SettingsProxy::saveSettings()
{
    m_settings->setValue("state", m_bufferModel->saveState());
    m_settings->setValue("ignores",  m_ignoreManager->ignores());
    m_settings->sync();
}

SettingsProxy::~SettingsProxy()
{
    delete m_settings;
}
