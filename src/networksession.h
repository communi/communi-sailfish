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
#include <QNetworkConfigurationManager>

class NetworkSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool online READ isOnline NOTIFY onlineStateChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

public:
    NetworkSession(QObject* parent = 0);

    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool isOnline() const;

public slots:
    bool open();

signals:
    void enabledChanged();
    void connectionChanged();
    void onlineStateChanged();

private slots:
    void onOnlineStateChanged();
    void onNetworkConfigurationChanged(const QNetworkConfiguration& config);

private:
    bool m_enabled;
    QNetworkSession* m_session;
    QNetworkConfiguration m_config;
    QNetworkConfigurationManager* m_manager;
};

#endif // NETWORKSESSION_H
