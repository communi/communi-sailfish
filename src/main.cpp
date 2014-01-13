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

#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

#if QT_VERSION < 0x050200
#include "qqmlsettings_p.h"
#endif

#include <sailfishapp.h>

#include "bufferproxymodel.h"
#include "networksession.h"
#include "messagestorage.h"
#include "messageformatter.h"

#include <IrcCore>
#include <IrcModel>
#include <IrcUtil>

static void registerIrcTypes(const char* uri, int major = 3, int minor = 1)
{
    // IrcCore
    Irc::registerMetaTypes();
    qmlRegisterType<Irc>(uri, major, minor, "Irc");
    qmlRegisterType<IrcCommand>(uri, major, minor, "IrcCommand");
    qmlRegisterType<IrcConnection>(uri, major, minor, "IrcConnection");
    qmlRegisterUncreatableType<IrcMessage>(uri, major, minor, "IrcMessage", "Cannot create an instance of IrcMessage. Use IrcConnection::messageReceived() signal instead.");
    qmlRegisterUncreatableType<IrcNetwork>(uri, major, minor, "IrcNetwork", "Cannot create an instance of IrcNetwork. Use IrcConnection::network property instead.");

    // IrcModel
    qmlRegisterType<IrcBuffer>(uri, major, minor, "IrcBuffer");
    qmlRegisterType<IrcBufferModel>(uri, major, minor, "IrcBufferModel");
    qmlRegisterType<IrcChannel>(uri, major, minor, "IrcChannel");
    qmlRegisterType<IrcUser>(uri, major, minor, "IrcUser");
    qmlRegisterType<IrcUserModel>(uri, major, minor, "IrcUserModel");

    // IrcUtil
    qmlRegisterType<IrcCommandParser>(uri, major, minor, "IrcCommandParser");
    qmlRegisterType<IrcLagTimer>(uri, major, minor, "IrcLagTimer");
    qmlRegisterType<IrcTextFormat>(uri, major, minor, "IrcTextFormat");
    qmlRegisterUncreatableType<IrcPalette>(uri, major, minor, "IrcPalette", "Cannot create an instance of IrcPalette. Use IrcTextFormat::palette property instead.");
    qmlRegisterType<IrcCompleter>(uri, major, minor, "IrcCompleter");
}

Q_DECL_EXPORT int main(int argc, char* argv[])
{
    QGuiApplication::setApplicationName("IRC for Sailfish");
    QGuiApplication::setOrganizationName("harbour-communi");
    QGuiApplication::setApplicationVersion("0.1");

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> viewer(SailfishApp::createView());

    registerIrcTypes("Communi");

#if QT_VERSION < 0x050200
    qmlRegisterType<QQmlSettings>("Qt.labs.settings", 1, 0, "Settings");
#endif

    NetworkSession* session = new NetworkSession(app.data());
    viewer->rootContext()->setContextProperty("NetworkSession", session);

    BufferProxyModel* model = new BufferProxyModel(app.data());
    viewer->rootContext()->setContextProperty("BufferModel", model);

    MessageFormatter* formatter = new MessageFormatter(app.data());
    viewer->rootContext()->setContextProperty("MessageFormatter", formatter);

    viewer->rootContext()->setContextProperty("MessageStorage", MessageStorage::instance());
    QObject::connect(model, SIGNAL(bufferAdded(IrcBuffer*)), MessageStorage::instance(), SLOT(add(IrcBuffer*)));
    QObject::connect(model, SIGNAL(bufferRemoved(IrcBuffer*)), MessageStorage::instance(), SLOT(remove(IrcBuffer*)));

    viewer->setSource(QUrl("qrc:///qml/main.qml"));
    viewer->showFullScreen();

    return app->exec();
}
