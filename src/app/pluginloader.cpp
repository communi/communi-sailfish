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

#include "pluginloader.h"
#include "communiplugin.h"
#include <QFileInfo>
#include <QDir>

static QList<CommuniPlugin*> loadPlugins(const QString& path)
{
    QList<CommuniPlugin*> plugins;
    foreach (const QFileInfo& file, QDir(path).entryInfoList(QDir::Files)) {
        QPluginLoader loader(file.absoluteFilePath());
        CommuniPlugin* plugin = qobject_cast<CommuniPlugin*>(loader.instance());
        if (plugin)
            plugins += plugin;
    }
    return plugins;
}

PluginLoader::PluginLoader(QObject* parent) : QObject(parent)
{
}

QString PluginLoader::pluginPath() const
{
    return m_path;
}

void PluginLoader::setPluginPath(const QString& path)
{
    m_path = path;
}

bool PluginLoader::load()
{
    m_plugins.clear();
    if (!m_path.isEmpty())
        m_plugins = loadPlugins(m_path);
    return !m_plugins.isEmpty();
}

void PluginLoader::connectionAdded(IrcConnection* connection)
{
    foreach (CommuniPlugin* plugin, m_plugins)
        plugin->connectionAdded(connection);
}

void PluginLoader::connectionRemoved(IrcConnection* connection)
{
    foreach (CommuniPlugin* plugin, m_plugins)
        plugin->connectionRemoved(connection);
}
