// SPDX-License-Identifier: GPL-2.0
/*
  Copyright (C) 2008-2021 The Communi Project
*/

#pragma once

#include <QObject>
#include <QString>

class AboutData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(QString applicationName READ applicationName CONSTANT)
    Q_PROPERTY(QString applicationIcon READ applicationIcon CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QString contributors READ contributors CONSTANT)
    Q_PROPERTY(QString organizationDomain READ organizationDomain CONSTANT)

public:
    AboutData(QObject* parent = nullptr);

    static QString displayName();
    static QString applicationName();
    static QString applicationIcon();
    static QString description();
    static QString version();
    static QString url();

    QString contributors() const;
    QString organizationDomain() const;
    void setApplicationData() const;
};
