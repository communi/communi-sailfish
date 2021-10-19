// SPDX-License-Identifier: GPL-2.0
/*
  Copyright (C) 2008-2021 The Communi Project
*/

#pragma once
#include <QSettings>

#include "bufferproxymodel.h"

#include "ignoremanager.h"

class SettingsProxy : public QObject
{
    Q_OBJECT

public:
    SettingsProxy(BufferProxyModel *bufferModel,
                  IgnoreManager *ignore, QObject* parent = nullptr);
    ~SettingsProxy();

    void restoreSettings();

public slots:
    void saveSettings();


private:
    QSettings *m_settings;
    IgnoreManager *m_ignoreManager;
    BufferProxyModel *m_bufferModel;
};
