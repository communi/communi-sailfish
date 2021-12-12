// SPDX-License-Identifier: GPL-2.0
/*
  Copyright (C) 2008-2021 The Communi Project
*/

#include "settingsproxy.h"

#include <QStringList>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>


SettingsProxy::SettingsProxy(BufferProxyModel *bufferModel,
                   IgnoreManager* ignore, QObject* parent)
{

    this->migrateSettings();

    m_settings = new QSettings(QCoreApplication::applicationName(),
                               QCoreApplication::applicationName(), parent);

    m_ignoreManager = ignore;
    m_bufferModel = bufferModel;
    this->restoreSettings();
}

void SettingsProxy::migrateSettings()
{
    QString oldConfigFileStr =
        QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first() +
        "/" + "harbour-communi/IRC for Sailfish.conf";
    QString newConfigFileStr =
        QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first() +
        "/" + QCoreApplication::applicationName() +
        "/" + QCoreApplication::applicationName() + ".conf";

    if((!QFileInfo(newConfigFileStr).exists() && !QDir(newConfigFileStr).exists()) &&
        (QFileInfo(oldConfigFileStr).exists() && !QDir(oldConfigFileStr).exists())) {
        QFile::rename(oldConfigFileStr, newConfigFileStr);
    }
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
